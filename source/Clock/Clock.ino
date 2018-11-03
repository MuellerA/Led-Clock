////////////////////////////////////////////////////////////////////////////////
// Clock.ino
////////////////////////////////////////////////////////////////////////////////

// HTTPS axTLS or BearSSL or none (= unsecured http)
#define HTTPS_None    0
#define HTTPS_axTLS   1
#define HTTPS_BearSSL 2
#define HTTPS HTTPS_axTLS

#include <ESP8266WiFi.h>
#if (HTTPS == HTTPS_axTLS) || (HTTPS == HTTPS_BearSSL)
#include <ESP8266WebServerSecure.h>
#else
#include <ESP8266WebServer.h>
#endif
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

#if HTTPS == HTTPS_axTLS
axTLS::ESP8266WebServerSecure httpServer ( 443 ) ;
#elif HTTPS == HTTPS_BearSSL
BearSSL::ESP8266WebServerSecure httpServer ( 443 ) ;
#else
ESP8266WebServer httpServer ( 80 ) ;
#endif
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

const double Brightness::Min = 0.1 ;

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

void Brightness::update()
{
  unsigned long v = analogRead(_pin) ;
  _sum -= _val[_idx] ;
  _val[_idx] = v ;
  _sum += _val[_idx] ;
  _idx = (_idx+1) % 16 ;
}

double Brightness::operator()() const
{
  double v = (double)(_sum) / (16.0*1024.0) ;
  return (v < Min) ? Min : v ;    
}

////////////////////////////////////////////////////////////////////////////////

void updateClock(uint64_t localTime)
{
  static Color c15(0x18, 0x18, 0x18) ;
  static Color c05(0x08, 0x08, 0x08) ;
  static Color c01(0x00, 0x00, 0x00) ;
  
  uint8_t second = localTime % 60 ;
  uint8_t minute = localTime / 60 % 60 ;
  uint8_t hour   = localTime / 60 / 60 % 12 ;

  double b = brightness() ;

  Color c15b = c15.brightness(b) ;
  Color c05b = c05.brightness(b) ;
  Color c01b = c01.brightness(b) ;
  Color hb   = settings._colHour  .brightness(b) ;
  Color mb   = settings._colMinute.brightness(b) ;
  Color sb   = settings._colSecond.brightness(b) ;
  
  for (unsigned char i = 0 ; i < 60 ; ++i)
  {
    Color c ;

    if (i == (hour*5 + minute/12)) c = hb ;
    if (i == minute)               c.mix(mb) ;
    if (i == second)               c.mix(sb) ;

    if (!c)
    {
      if      ((i % 15) == 0) c = c15b ;
      else if ((i %  5) == 0) c = c05b ;
      else                    c = c01b ;
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

  brightness.init() ;
}

////////////////////////////////////////////////////////////////////////////////

void loop()
{
  uint32_t now = millis() ;
  
  if (Shutdown)
  {
    static uint8_t  cnt  = 0 ;
    static uint32_t last = 0 ;

    if (now - last > 25)
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
      last = now ;

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
      static uint32_t last = 0 ;
      if (now - last > 1000)
      {
        updateClock() ;
        last = now ;
      }
    }
  }

  {
    static uint32_t last = 0 ;
    if (now - last > 333)
    {
      brightness.update() ;
      last = now ;                                                                               
    }
  }
  
  // NTP
  if (WifiConnected)
    ntp.tx(udp, settings._ntp) ;
  
  httpServer.handleClient() ;

  delay(50) ;
}

////////////////////////////////////////////////////////////////////////////////
// EOF
////////////////////////////////////////////////////////////////////////////////
