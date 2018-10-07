#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WiFiUdp.h>
#include <FS.h>

#include <Adafruit_NeoPixel.h>

#include "Clock.h"

const int WS2812_NUM = 60 ;
const int WS2812_PIN = 12 ;

Adafruit_NeoPixel ws2812 = Adafruit_NeoPixel(WS2812_NUM, WS2812_PIN, NEO_GRB + NEO_KHZ800);
ESP8266WebServer httpServer ( 80 ) ;

const uint16_t UDP_PORT = 1123 ;
WiFiUDP udp ;
Ntp ntp ;

Settings settings ;
Time t ;
Brightness brightness{0} ;

bool WifiConnected = false ;

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

void updateClock()
{
  if (t.valid())
  {
    for (unsigned char i = 0 ; i < 60 ; ++i)
    {
      Color c ;

      if (i == ((t.hour()%12)*5 + t.minute()/12)) c = settings._colHour ;
      if (i == t.minute()) c.mix(settings._colMinute) ;
      if (i == t.second()) c.mix(settings._colSecond) ;

      if (!c)
      {
        if      ((i % 15) == 0) c.set(0x80, 0x80, 0x80) ;
        else if ((i %  5) == 0) c.set(0x08, 0x08, 0x08) ;
        else                    c.set(0x00, 0x00, 0x00) ;
      }

      ws2812.setPixelColor(i, c.rgb()) ;
    }
  }
  else
  {
    static uint8_t i0 ;
    
    uint32_t rgb ;
    switch (t.second() % 3)
    {
    case 0: rgb = settings._colHour  .rgb() ; break ;
    case 1: rgb = settings._colMinute.rgb() ; break ;
    case 2: rgb = settings._colSecond.rgb() ; break ;
    }

    for (uint8_t i = 0 ; i < 60 ; ++i)
    {
      ws2812.setPixelColor(i, ((i % 5) == i0) ? rgb : 0) ;
    }
    i0 = (i0 + 1) % 5 ;
  }
  ws2812.show() ;
}

////////////////////////////////////////////////////////////////////////////////
// Arduino Main
////////////////////////////////////////////////////////////////////////////////

void setup()
{
  Serial.begin ( 115200 );
  Serial.printf("\n") ;
  //Serial.setDebugOutput(true) ;
  
  SPIFFS.begin() ;
  
  settings.load() ;
  Serial.printf("AP: %s / %s / %d\n", settings._apSsid.c_str(), settings._apPsk.c_str(), settings._apChan) ;
  
  WifiSetup() ;
  
  httpServer.on("/", httpOnRoot) ;
  httpServer.on("/settings.html", httpOnSettings) ;
  httpServer.begin() ;

  udp.begin(UDP_PORT) ;

  ws2812.begin() ;
}

////////////////////////////////////////////////////////////////////////////////

void loop()
{
  int udpLen = udp.parsePacket() ;
  if (udpLen)
  {
    if ((udp.remotePort() == 123) &&
        (udpLen == sizeof(Ntp::NtpData)) &&
        ntp.rx(udp))
    {
      uint32_t ts = (ntp._ts >> 32) + (settings._tz * 60) ;
      uint32_t sec  = ts % 60 ;
      uint32_t min  = ts / 60 % 60 ;
      uint32_t hour = ts / 60 / 60 % 24 ;

      t.set(hour, min, sec) ;
    }
  }
  
  WifiLoop() ;

  // Clock
  {
    static unsigned long lastMs = 0 ;
    unsigned long ms = millis() ;

    if ((ms - lastMs) > 1000)
    {
      lastMs = ms ;
      t.inc() ;
      brightness.update() ;

      updateClock() ;
      Serial.println(t.toString()) ;
    }
  }

  // NTP
  if (WifiConnected)
  {
    static unsigned long lastMs = 0 ;
    unsigned long ms = millis() ;

    if (((ms - lastMs) > (ntp._delay * 1000)))
    {
      lastMs = ms ;
      ntp.tx(udp, settings._ntp) ;
    }   
  }
  
  httpServer.handleClient() ;
}

////////////////////////////////////////////////////////////////////////////////
// EOF
////////////////////////////////////////////////////////////////////////////////
