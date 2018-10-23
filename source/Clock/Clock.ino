#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPUpdateServer.h>
#include <WiFiUdp.h>
#include <FS.h>

#include <Adafruit_NeoPixel.h>

#include <functional>

#include "timezone.h"
#include "Clock.h"

////////////////////////////////////////////////////////////////////////////////

char HostName[16] ;

const int WS2812_NUM = 60 ;
const int WS2812_PIN = 12 ;

Adafruit_NeoPixel ws2812 = Adafruit_NeoPixel(WS2812_NUM, WS2812_PIN, NEO_GRB + NEO_KHZ800);

ESP8266WebServer httpServer ( 80 ) ;
ESP8266HTTPUpdateServer httpUpdater;

const uint16_t UDP_PORT = 1123 ;
WiFiUDP udp ;
Ntp ntp ;

Settings settings ;
Brightness brightness{0} ;

bool WifiConnected = false ;

TZ::TimeZone tz ;

bool Shutdown = false ;

////////////////////////////////////////////////////////////////////////////////
// util

bool ascDec2bin(char c, uint8_t &d)
{
  if (('0' <= c) && (c <= '9')) { d = c - '0'      ; return true ; }
  return false ;
}

bool ascHex2bin(char c, uint8_t &h) 
{
  if (('0' <= c) && (c <= '9')) { h = c - '0'      ; return true ; }
  if (('a' <= c) && (c <= 'f')) { h = c - 'a' + 10 ; return true ; }
  if (('A' <= c) && (c <= 'F')) { h = c - 'A' + 10 ; return true ; }
  return false ;
}

template<typename T>
bool ascInt2bin(String str, T &val)
{
  if (str.length() > 5)
    return false ;

  uint8_t e = str.length() ;
  if (e < 1)
    return false ;

  uint8_t idx = 0 ;
  bool neg = false ;

  if      (str[0] == '-') { neg = true ; idx = 1 ; }
  else if (str[0] == '+') {              idx = 1 ; }

  if (e < (idx + 1))
    return false ;
  
  T v = 0 ;
  for ( ; idx < e ; ++idx)
  {
    uint8_t d ;
    if (!ascDec2bin(str[idx], d))
      return false ;
    v = v*10 + d ;
  }
  val = neg ? -v : v ;

  return true ;
}

template<typename T>
bool ascInt2bin(String str, T &val, T min, T max)
{
  T val0 ;
  if (ascInt2bin(str, val0) &&
      (min <= val0) && (val0 <= max))
  {
    val = val0 ;
    return true ;
  }
  return false ;
}

////////////////////////////////////////////////////////////////////////////////

Brightness::Brightness(unsigned int pin) : _pin(pin)
{
}

void Brightness::init()
{
  unsigned long v = analogRead(_pin) ;
  
  for (unsigned int i = 0 ; i < 16 ; ++i)
    _val[i] = v ;
  _sum = v * 16 ;
  _idx = 0 ;
}

unsigned long Brightness::update()
{
  unsigned long v = analogRead(_pin) ;
  _sum -= _val[_idx] ;
  _val[_idx] = v ;
  _sum += _val[_idx] ;
  _idx = (_idx+1) % 16 ;
  
  return _sum/16 ;
}

////////////////////////////////////////////////////////////////////////////////

void updateClock(uint64_t localTime)
{
  uint8_t second = localTime % 60 ;
  uint8_t minute = localTime / 60 % 60 ;
  uint8_t hour   = localTime / 60 / 60 % 12 ;
    
  for (unsigned char i = 0 ; i < 60 ; ++i)
  {
    Color c ;

    if (i == (hour*5 + minute/12)) c = settings._colHour ;
    if (i == minute)               c.mix(settings._colMinute) ;
    if (i == second)               c.mix(settings._colSecond) ;

    if (!c)
    {
      if      ((i % 15) == 0) c.set(0x18, 0x18, 0x18) ;
      else if ((i %  5) == 0) c.set(0x08, 0x08, 0x08) ;
      else                    c.set(0x00, 0x00, 0x00) ;
    }

    ws2812.setPixelColor(i, c.rgb()) ;
  }
  ws2812.show() ;
}

void updateClock()
{
  static uint8_t cnt ;
  static uint8_t idx ;
    
  uint32_t rgb ;
  switch (cnt)
  {
  case 0: rgb = settings._colHour  .rgb() ; break ;
  case 1: rgb = settings._colMinute.rgb() ; break ;
  case 2: rgb = settings._colSecond.rgb() ; break ;
  }

  for (uint8_t i = 0 ; i < 60 ; ++i)
  {
    ws2812.setPixelColor(i, ((i % 5) == idx) ? rgb : 0) ;
  }
  cnt = (cnt + 1) % 3 ;
  idx = (idx + 1) % 5 ;

  ws2812.show() ;
}

////////////////////////////////////////////////////////////////////////////////
// Arduino Main
////////////////////////////////////////////////////////////////////////////////

void setup()
{
  sprintf(HostName, "clock-%06x", ESP.getChipId()) ;
  
  Serial.begin ( 115200 );
  Serial.printf("\n") ;
  //Serial.setDebugOutput(true) ;
  
  SPIFFS.begin() ;
  
  settings.load() ;
  Serial.printf("AP: SSID=%s / PSK=%s / Channel=%d\n", settings._apSsid.c_str(), settings._apPsk.c_str(), settings._apChan) ;
  
  WifiSetup() ;
  HttpSetup() ;
  
  udp.begin(UDP_PORT) ;

  ws2812.begin() ;
}

////////////////////////////////////////////////////////////////////////////////

void loop()
{
  if (Shutdown)
  {
    static uint8_t  cnt  = 0 ;
    static uint32_t next = 0 ;

    uint32_t now = millis() ;
    if (now > next)
    {
      for (uint8_t i = 0 ; i < 60 ; ++i)
      {
        if ((i <= 0 + cnt) || (i >= 60 - cnt))
          ws2812.setPixelColor(i, 0x0f0000) ;
        else
          ws2812.setPixelColor(i, 0x000000) ;
      }
      ws2812.show() ;
      cnt = cnt + 1 ;
      next = now + 20 ;

      if (cnt > 30)
        ESP.restart() ;
    }
    
    return ;
  }
  
  int udpLen = udp.parsePacket() ;
  if (udpLen)
  {
    if ((udp.remotePort() == Ntp::port()) &&
        (udpLen == Ntp::size()))
      ntp.rx(udp) ;
  }
  
  WifiLoop() ;

  // Clock
  {
    if (ntp.valid())
    {
      if (ntp.inc())
      {
        uint64_t local = ntp.local() ;
        updateClock(local) ;
      }
    }
    else
    {
      static uint32_t next = 0 ;
      uint32_t now = millis() ;
      if (now > next)
      {
        updateClock() ;
        next = now + 1000 ;
      }
    }
  }

  // NTP
  if (WifiConnected)
    ntp.tx(udp, settings._ntp) ;
  
  httpServer.handleClient() ;

  delay(40) ;
}

////////////////////////////////////////////////////////////////////////////////
// EOF
////////////////////////////////////////////////////////////////////////////////
