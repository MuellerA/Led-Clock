#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <Adafruit_NeoPixel.h>

extern "C" {
#include <user_interface.h>
}

#include "Clock.h"

const int WS2812_NUM = 60 ;
const int WS2812_PIN = 12 ;

Adafruit_NeoPixel ws2812 = Adafruit_NeoPixel(WS2812_NUM, WS2812_PIN, NEO_GRB + NEO_KHZ800);
ESP8266WebServer httpServer ( 80 ) ;

Settings settings ;
Time t ;
Brightness brightness{0} ;

// TODO
// WiFi Station connect
// NTP
// FS save/load settings
// color::mix

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

uint32_t Color::rgb()
{
  return (_r << 16) | (_g << 8) | (_b << 0) ;
}

void Color::mix(const Color &c)
{
  *this = c ; // todo
}

////////////////////////////////////////////////////////////////////////////////

void updateClock()
{
  if (t._valid)
  {
    for (unsigned char i = 0 ; i < 60 ; ++i)
    {
      Color c ;

      if ((i % 5) == 0) c._r = c._g = c._b = 0x0f ;
      else              c._r = c._g = c._b = 0x00 ;

      if (i == ((t._hour%12)*5 + t._minute/12)) c = settings._colHour ;
      if (i == t._minute) c.mix(settings._colMinute) ;
      if (i == t._second) c.mix(settings._colSecond) ;

      ws2812.setPixelColor(i, c.rgb()) ;
    }
  }
  else
  {
    uint32_t rgb ;
    switch (t._second %3)
    {
    case 0: rgb = settings._colHour  .rgb() ; break ;
    case 1: rgb = settings._colMinute.rgb() ; break ;
    case 2: rgb = settings._colSecond.rgb() ; break ;
    }

    for (uint8_t i = 0 ; i < 60 ; ++i)
    {
      if (i % 5)
        ws2812.setPixelColor(i%60, 0) ;
      else
        ws2812.setPixelColor(i%60, rgb) ;
    }
  }
  ws2812.show() ;
}

////////////////////////////////////////////////////////////////////////////////
// Arduino Main
////////////////////////////////////////////////////////////////////////////////

void setup()
{
  Serial.begin ( 115200 );

  settings.load() ;
  Serial.printf("\nAP: %s / %s / %d\n", settings._apSsid.c_str(), settings._apPsk.c_str(), settings._apChan) ;
  
  WiFi.mode(WIFI_OFF) ;
  httpServer.on("/", httpOnRoot) ;
  httpServer.on("/clock.html", httpOnClock) ;
  httpServer.begin() ;

  ws2812.begin() ;

  WiFi.softAP(settings._apSsid.c_str(), settings._apPsk.c_str(), settings._apChan) ;
}

////////////////////////////////////////////////////////////////////////////////

void loop()
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

  
  httpServer.handleClient() ;
}

////////////////////////////////////////////////////////////////////////////////
// EOF
////////////////////////////////////////////////////////////////////////////////
