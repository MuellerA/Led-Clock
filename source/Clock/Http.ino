////////////////////////////////////////////////////////////////////////////////
// Http
////////////////////////////////////////////////////////////////////////////////

String httpHeader(const String &title)
{
  return
    String("<!DOCTYPE html>\n"
           "<html>\n"
           " <head>\n"
           "  <meta charset=\"utf-8\"/>\n"
           "  <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\"/>\n"
           "  <title>") +
    title +
    String("</title>\n"
           " <style>\n"
           ".border\n"
           "{\n"
           "  border-width: 1px;\n"
           "  border-color: black;\n"
           "  border-style: solid;\n"
           "  padding: 5px;\n"
           "  margin: 8px 0px;\n"
           "}\n"
           " </style>\n"
           " </head>\n"
           " <body>\n"
           "  <h1>") + title + String("</h1>\n") ;
}
String httpFooter()
{
  return
    String(" </body>\n"
           "</html>\n") ;
}

void httpOnRoot()
{
  httpServer.send(200, "text/html",
                  httpHeader("Uhr") +
                  String("<p style=\"font-size: large\">") + t.toString() + String("</p>\n") +
                  String("<p style=\"font-size: small\"><a href=\"/settings.html\">Einstellungen</a></p>\n") +
                  httpFooter()) ;
}

bool httpOnSettings_Time(const String &s, uint8_t offset, uint8_t &v, uint8_t max)
{
  uint8_t t ;

  if (('0' <= s[offset+0]) && (s[offset+0] <= '9') &&
      ('0' <= s[offset+1]) && (s[offset+1] <= '9'))
  {
    t = (s[offset+0] - '0') * 10 + (s[offset+1] - '0') ;
    if (t <= max)
    {
      v = t ;
      return true ;
    }
  }
  return false ;
}

bool ascHex2bin(char c, uint8_t &h)
{
  if (('0' <= c) && (c <= '9')) { h = c - '0'      ; return true ; }
  if (('a' <= c) && (c <= 'f')) { h = c - 'a' + 10 ; return true ; }
  if (('A' <= c) && (c <= 'F')) { h = c - 'A' + 10 ; return true ; }
  return false ;
}

bool httpOnSettings_Color1(const String &s, uint8_t offset, uint8_t &v)
{
  uint8_t hi, lo ;
  if (ascHex2bin(s[offset+0], hi) && ascHex2bin(s[offset+1], lo))
  {
    v = (hi << 4) | (lo << 0) ;
    return true ;
  }
  return false ;
}

bool httpOnSettings_Color(const String &argName, Color &c)
{
  uint8_t r, g, b ;

  if (httpServer.hasArg(argName))
  {
    String color = httpServer.arg(argName) ;

    if ((color.length() == 7) &&
        (color[0] == '#') &&
        httpOnSettings_Color1(color, 1, r) &&
        httpOnSettings_Color1(color, 3, g) &&
        httpOnSettings_Color1(color, 5, b))
    {
      c.set(r, g, b) ;
      return true ;
    }
  }
  return false ;  
}

void httpOnSettings()
{
  if (httpServer.hasArg("action"))
  {
    bool settingsDirty = false ;
    String action = httpServer.arg("action") ;

    if (action == "time")
    {
      String time = httpServer.arg("time") ;
      uint8_t h, m, s ;
      
      if ((time.length() == 8) &&
          (time[2] == ':') &&
          (time[5] == ':') &&
          httpOnSettings_Time(time, 0, h, 23) &&
          httpOnSettings_Time(time, 3, m, 59) &&
          httpOnSettings_Time(time, 6, s, 59))
      {
        t.set(h, m, s) ;
      }
    }
    else if (action == "color")
    {
      String color = httpServer.arg("color") ;
      Color colHour, colMinute, colSecond ;

      if (httpOnSettings_Color("colorHour"  , colHour  ) &&
          httpOnSettings_Color("colorMinute", colMinute) &&
          httpOnSettings_Color("colorSecond", colSecond))
      {
        settings._colHour   = colHour   ;
        settings._colMinute = colMinute ;
        settings._colSecond = colSecond ;
        settingsDirty = true ;
      }
    }
    else if (action == "wifi")
    {
      String ssid = httpServer.arg("ssid") ;
      String psk  = httpServer.arg("psk" ) ;
      String ntp  = httpServer.arg("ntp" ) ;
      
      if (ssid.length() && (ssid != settings._ssid))
      {
        settings._ssid = ssid ;
        settings._psk  = psk ;
        settingsDirty = true ;
        WifiStationStart() ;
      }
      else if (psk.length() && (psk != settings._psk))
      {
        settings._psk = psk ;
        settingsDirty = true ;
        WifiStationStart() ;
      }

      if (ntp != settings._ntp)
      {
        settings._ntp = ntp ;
        settingsDirty = true ;
      }
    }

    if (settingsDirty)
      settings.save() ;
  }
  
  if (httpServer.hasArg("action"))
    Serial.println(httpServer.arg("action")) ;
  
  httpServer.send(200, "text/html",
                  httpHeader("Uhr - Einstellungen") +
                  String("<p style=\"font-size: small\"><a href=\"/\">Startseite</a></p>\n"
                         "<div class=\"border\">"
                         "<h2>Zeit</h2>\n"
                         "<form action=\"settings.html\" method=\"post\" autocomplete=\"off\">\n"
                         "<input type=\"time\" name=\"time\" step=\"1\" value=\"") +
                  t.toString() +
                  String("\">\n"
                         "<button type=\"submit\" name=\"action\" value=\"time\">Zeit setzen</button>\n"
                         "</form>\n"
                         "</div>\n"                         
                         
                         "<div class=\"border\">"
                         "<h2>Farbe</h2>\n"
                         "<form action=\"settings.html\" method=\"post\" autocomplete=\"off\">\n"
                         "<table>\n"
                         " <tr><td>Stunde</td><td><input type=\"color\" name=\"colorHour\" value=\"") +
                  settings._colHour.toString() +
                  String("\">\n"
                         " <tr><td>Minute</td><td><input type=\"color\" name=\"colorMinute\" value=\"") +
                  settings._colMinute.toString() +
                  String("\">\n"
                         " <tr><td>Sekunde</td><td><input type=\"color\" name=\"colorSecond\" value=\"") +
                  settings._colSecond.toString() +
                  String("\">\n"
                         "</table>\n"
                         "<button type=\"submit\" name=\"action\" value=\"color\">Farbe setzen</button>\n"
                         "</form>\n"
                         "</div>\n"                         

                         "<div class=\"border\">"
                         "<h2>WLAN</h2>\n"
                         "<form action=\"settings.html\" method=\"post\" autocomplete=\"off\">\n"
                         "<table>\n"
                         "  <tr><td>SSID</td><td><input type=\"text\" name=\"ssid\" size=\"32\" value=\"") +
                  settings._ssid +
                  String("\"></td></tr>\n"
                         "  <tr><td>PSK</td><td><input type=\"password\" name=\"psk\" size=\"32\" value=\"\"></td></tr>\n"
                         "  <tr><td>NTP</td><td><input type=\"text\" name=\"ntp\" size=\"32\" value=\"") +
                  settings._ntp +
                  String("\"></td></tr>\n"
                         "</table>\n"
                         "<button type=\"submit\" name=\"action\" value=\"wifi\">Zugangsdaten setzen</button>\n"
                         "</form>\n"
                         "</div>\n") +
                  httpFooter()) ;
}

////////////////////////////////////////////////////////////////////////////////
// EOF
////////////////////////////////////////////////////////////////////////////////
