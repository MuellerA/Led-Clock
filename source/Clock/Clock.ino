#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <FS.h>

#include <Adafruit_NeoPixel.h>

#include "Clock.h"

const int WS2812_NUM = 60 ;
const int WS2812_PIN = 12 ;

Adafruit_NeoPixel ws2812 = Adafruit_NeoPixel(WS2812_NUM, WS2812_PIN, NEO_GRB + NEO_KHZ800);
ESP8266WebServer httpServer ( 80 ) ;

Settings settings ;
Time t ;
Brightness brightness{0} ;

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

Color::Color() : _r{0}, _g{0}, _b{0}
{
}

Color::Color(uint8_t r, uint8_t g, uint8_t b) : _r{r}, _g{g}, _b{b}
{
}

void Color::set(uint8_t r, uint8_t g, uint8_t b)
{
  _r = r ;
  _g = g ;
  _b = b ;
}

uint32_t Color::rgb() const
{
  return ((uint32_t)_r << 16) | ((uint32_t)_g << 8) | ((uint32_t)_b << 0) ;
}

void Color::mix(const Color &c)
{
  uint16_t sum ;

  sum = (uint16_t)_r + (uint16_t)c._r ; _r = (sum > 0xff) ? 0xff : sum ;
  sum = (uint16_t)_g + (uint16_t)c._g ; _g = (sum > 0xff) ? 0xff : sum ;
  sum = (uint16_t)_b + (uint16_t)c._b ; _b = (sum > 0xff) ? 0xff : sum ;
}

String Color::toString() const
{
  char buff[16] ;
  sprintf(buff, "#%02x%02x%02x", _r, _g, _b) ;
  return String(buff) ;
}

bool Color::operator!() const
{
  return !_r && !_g && !_b ;
}

////////////////////////////////////////////////////////////////////////////////

void updateClock()
{
  if (t._valid)
  {
    for (unsigned char i = 0 ; i < 60 ; ++i)
    {
      Color c ;

      if (i == ((t._hour%12)*5 + t._minute/12)) c = settings._colHour ;
      if (i == t._minute) c.mix(settings._colMinute) ;
      if (i == t._second) c.mix(settings._colSecond) ;

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
    switch (t._second % 3)
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
  
  SPIFFS.begin() ;
  
  settings.load() ;
  Serial.printf("AP: %s / %s / %d\n", settings._apSsid.c_str(), settings._apPsk.c_str(), settings._apChan) ;
  
  WifiSetup() ;
  
  httpServer.on("/", httpOnRoot) ;
  httpServer.on("/settings.html", httpOnSettings) ;
  httpServer.begin() ;

  ws2812.begin() ;
}

////////////////////////////////////////////////////////////////////////////////

void loop()
{
  WifiLoop() ;
  
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
