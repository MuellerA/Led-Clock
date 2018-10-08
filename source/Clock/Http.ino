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
                  String("<p style=\"font-size: small\"><a href=\"/update\">Aktualisieren</a></p>\n") +
                  httpFooter()) ;
}

bool httpOnSettings_Color(const String &argName, Color &c)
{
  Color color ;

  if (httpServer.hasArg(argName) &&
      color.fromString(httpServer.arg(argName)))
  {
    c = color ;
    return true ;
  }
  return false ;  
}

void httpOnSettings()
{
  if (httpServer.hasArg("action"))
  {
    bool settingsDirty = false ;
    bool ntpDirty      = false ;
    String action = httpServer.arg("action") ;

    Serial.printf("settings: %s\n", action.c_str()) ;

    if (action == "time")
    {
      t.fromString(httpServer.arg("time")) ;
    }
    else if (action == "color")
    {
      Color colHour, colMinute, colSecond ;

      if (colHour  .fromString(httpServer.arg("colorHour"  )) &&
          colMinute.fromString(httpServer.arg("colorMinute")) &&
          colSecond.fromString(httpServer.arg("colorSecond")))
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
      String tz   = httpServer.arg("tz"  ) ;

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
        ntpDirty      = true ;
      }

      int32_t tzI ;
      if (ascInt2bin(tz, tzI) &&
          (tzI != settings._tz))
      {
        settings._tz = tzI ;
        settingsDirty = true ;
        ntpDirty      = true ;
      }
    }

    if (settingsDirty)
      settings.save() ;
    if (ntpDirty)
    {
      if (settings._ntp.length())
        ntp.start() ;
      else
        ntp.stop() ;
    }
  }
  
  httpServer.send(200, "text/html",
                  httpHeader("Uhr - Einstellungen") +
                  String("<p style=\"font-size: small\"><a href=\"/\">Startseite</a></p>\n"
                         "<div class=\"border\">\n"
                         "<h2>Zeit</h2>\n"
                         "<form action=\"settings.html\" method=\"post\" autocomplete=\"off\">\n"
                         "<input type=\"time\" name=\"time\" step=\"1\" value=\"") +
                  t.toString() +
                  String("\"/>\n"
                         "<button type=\"submit\" name=\"action\" value=\"time\">Zeit setzen</button>\n"
                         "</form>\n"
                         "</div>\n"                         
                         
                         "<div class=\"border\">\n"
                         "<h2>Farbe</h2>\n"
                         "<form action=\"settings.html\" method=\"post\" autocomplete=\"off\">\n"
                         "<table>\n"
                         " <tr><td>Stunde</td><td><input type=\"color\" name=\"colorHour\" value=\"") +
                  settings._colHour.toString() +
                  String("\"/></td></tr>\n"
                         " <tr><td>Minute</td><td><input type=\"color\" name=\"colorMinute\" value=\"") +
                  settings._colMinute.toString() +
                  String("\"/></td></tr>\n"
                         " <tr><td>Sekunde</td><td><input type=\"color\" name=\"colorSecond\" value=\"") +
                  settings._colSecond.toString() +
                  String("\"/></td></tr>\n"
                         "</table>\n"
                         "<button type=\"submit\" name=\"action\" value=\"color\">Farbe setzen</button>\n"
                         "</form>\n"
                         "</div>\n"                         

                         "<div class=\"border\">\n"
                         "<h2>WLAN</h2>\n"
                         "<form action=\"settings.html\" method=\"post\" autocomplete=\"off\">\n"
                         "<table>\n"
                         "  <tr><td>SSID</td><td><input type=\"text\" name=\"ssid\" size=\"32\" value=\"") +
                  settings._ssid +
                  String("\"/></td></tr>\n"
                         "  <tr><td>PSK</td><td><input type=\"password\" name=\"psk\" size=\"32\" value=\"\"/></td></tr>\n"
                         "  <tr><td>NTP</td><td><input type=\"text\" name=\"ntp\" size=\"32\" value=\"") +
                  settings._ntp +
                  String("\" placeholder=\"pool.ntp.org\"/></td></tr>\n"
                         "  <tr><td>Timezone (min)</td><td><input type=\"number\" name=\"tz\" min=\"-840\" max=\"+840\" step=\"15\" value=\"") +
                  String(settings._tz) +
                  String("\"/></td></tr>\n"
                         "</table>\n"
                         "<button type=\"submit\" name=\"action\" value=\"wifi\">Zugangsdaten setzen</button>\n"
                         "</form>\n"
                         "</div>\n") +
                  httpFooter()) ;
}

////////////////////////////////////////////////////////////////////////////////
// EOF
////////////////////////////////////////////////////////////////////////////////
