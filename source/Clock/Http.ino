////////////////////////////////////////////////////////////////////////////////
// Http
////////////////////////////////////////////////////////////////////////////////

void HttpSetup()
{
  httpServer.on("/", httpOnRoot) ;
  httpServer.on("/settings.html", httpOnSettings) ;
  httpServer.on("/reset", httpOnReset) ;
  httpUpdater.setup(&httpServer, "/update", settings._apSsid.c_str(), settings._apPsk.c_str());
  httpServer.begin() ;
}

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
                  String("<p><span style=\"font-size: large\">") + ntp.toLocalString() + String("</span>") +
                  String(ntp.active() ? "(NTP)" : "(manual)") + String("</p>\n") +
                  String("<p style=\"font-size: small\"><a href=\"/settings.html\">Einstellungen</a></p>\n") +
                  String("<p style=\"font-size: small\"><a href=\"/update\">Aktualisieren</a></p>\n") +
                  String("<p style=\"font-size: small\"><a href=\"/reset\">Neu starten</a></p>\n") +
                  httpFooter()) ;
}

void httpOnReset()
{
  httpServer.send(200, "text/html",
                  httpHeader("Uhr") +
                  String("<p>Starte neu...</p>\n") +
                  httpFooter()) ;
  Shutdown = true ;
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

bool httpOnSettings_Month(const String &month, uint8_t &value)
{
  if (month == "Januar"   ) { value = Jan ; return true ; }
  if (month == "Februar"  ) { value = Feb ; return true ; }
  if (month == "März"     ) { value = Mar ; return true ; }
  if (month == "April"    ) { value = Apr ; return true ; }
  if (month == "Mail"     ) { value = May ; return true ; }
  if (month == "Juni"     ) { value = Jun ; return true ; }
  if (month == "Juli"     ) { value = Jul ; return true ; }
  if (month == "August"   ) { value = Aug ; return true ; }
  if (month == "September") { value = Sep ; return true ; }
  if (month == "Oktober"  ) { value = Oct ; return true ; }
  if (month == "November" ) { value = Nov ; return true ; }
  if (month == "Dezember" ) { value = Dec ; return true ; }
  return false ;
}

bool httpOnSettings_WeekOfMonth(const String &weekOfMonth, uint8_t &value)
{
  if (weekOfMonth == "Erster"  ) { value = First  ; return true ; }
  if (weekOfMonth == "Zweiter" ) { value = Second ; return true ; }
  if (weekOfMonth == "Dritter" ) { value = Third  ; return true ; }
  if (weekOfMonth == "Vierter" ) { value = Fourth ; return true ; }
  if (weekOfMonth == "Letzter" ) { value = Last   ; return true ; }
  return false ;
}

bool httpOnSettings_DayOfWeek(const String &dayOfWeek, uint8_t &value)
{
  if (dayOfWeek == "Sonntag"    ) { value = Sun ; return true ; }
  if (dayOfWeek == "Montag"     ) { value = Mon ; return true ; }
  if (dayOfWeek == "Dienstag"   ) { value = Tue ; return true ; }
  if (dayOfWeek == "Mittwoch"   ) { value = Wed ; return true ; }
  if (dayOfWeek == "Donnerstag" ) { value = Thu ; return true ; }
  if (dayOfWeek == "Freitag"    ) { value = Fri ; return true ; }
  if (dayOfWeek == "Samstag"    ) { value = Sat ; return true ; }
}

String inputMonth(const char *name, uint8_t month)
{
  return
    String("<select name=\"") +
    String(name) +
    String("\" size=\"1\"><option"     ) + String(month == Jan ? " selected" : "") +
    String(">Januar</option><option"   ) + String(month == Feb ? " selected" : "") +
    String(">Februar</option><option"  ) + String(month == Mar ? " selected" : "") +
    String(">März</option><option"     ) + String(month == Apr ? " selected" : "") +
    String(">April</option><option"    ) + String(month == May ? " selected" : "") +
    String(">Mai</option><option"      ) + String(month == Jun ? " selected" : "") +
    String(">Juni</option><option"     ) + String(month == Jul ? " selected" : "") +
    String(">Juli</option><option"     ) + String(month == Aug ? " selected" : "") +
    String(">August</option><option"   ) + String(month == Sep ? " selected" : "") +
    String(">September</option><option") + String(month == Oct ? " selected" : "") +
    String(">Oktober</option><option"  ) + String(month == Nov ? " selected" : "") +
    String(">November</option><option" ) + String(month == Dec ? " selected" : "") +
    String(">Dezember</option></select>") ;
}
String inputWeekOfMonth(const char *name, uint8_t weekOfMonth)
{
  return
    String("<select name=\"") +
    String(name) +
    String("\" size=\"1\"><option"  ) + String(weekOfMonth == First  ? " selected" : "") +
    String(">Erste</option><option" ) + String(weekOfMonth == Second ? " selected" : "") +
    String(">Zweite</option><option") + String(weekOfMonth == Third  ? " selected" : "") +
    String(">Dritte</option><option") + String(weekOfMonth == Fourth ? " selected" : "") +
    String(">Vierte</option><option") + String(weekOfMonth == Last   ? " selected" : "") +
    String(">Letzte</option></select>") ;
}
String inputDayOfWeek(const char *name, uint8_t dayOfWeek)
{
  return
    String("<select name=\"") +
    String(name) +
    String("\" size=\"1\"><option"      ) + String(dayOfWeek == Sun ? " selected" : "") +
    String(">Sonntag</option><option"   ) + String(dayOfWeek == Mon ? " selected" : "") +
    String(">Montag</option><option"    ) + String(dayOfWeek == Tue ? " selected" : "") +
    String(">Dienstag</option><option"  ) + String(dayOfWeek == Wed ? " selected" : "") +
    String(">Mittwoch</option><option"  ) + String(dayOfWeek == Thu ? " selected" : "") +
    String(">Donnerstag</option><option") + String(dayOfWeek == Fri ? " selected" : "") +
    String(">Freitag</option><option"   ) + String(dayOfWeek == Sat ? " selected" : "") +
    String(">Samstag</option></select>") ;
}
template<typename T>
String inputInt(const char *name, T val, T min, T max)
{
  return
    String("<input type=\"number\" name=\"") +
    String(name) +
    String("\" min=\"") +
    String(min) +
    String("\" max=\"") +
    String(max) +
    String("\" value=\"") +
    String(val) +
    String("\">") ;
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
      ntp.fromLocalString(httpServer.arg("time")) ;
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
      String tzDstMonth       = httpServer.arg("tzDstMonth"      ) ;
      String tzDstWeekOfMonth = httpServer.arg("tzDstWeekOfMonth") ;
      String tzDstDayOfWeek   = httpServer.arg("tzDstDayOfWeek"  ) ;
      String tzDstHour        = httpServer.arg("tzDstHour"       ) ;
      String tzDstOffset      = httpServer.arg("tzDstOffset"     ) ;
      String tzStdMonth       = httpServer.arg("tzStdMonth"      ) ;
      String tzStdWeekOfMonth = httpServer.arg("tzStdWeekOfMonth") ;
      String tzStdDayOfWeek   = httpServer.arg("tzStdDayOfWeek"  ) ;
      String tzStdHour        = httpServer.arg("tzStdHour"       ) ;
      String tzStdOffset      = httpServer.arg("tzStdOffset"     ) ;

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

      uint8_t dstMonth       ;
      uint8_t dstWeekOfMonth ;
      uint8_t dstDayOfWeek   ;
      uint8_t dstHour        ;
      int16_t dstOffset      ;
      uint8_t stdMonth       ;
      uint8_t stdWeekOfMonth ;
      uint8_t stdDayOfWeek   ;
      uint8_t stdHour        ;
      int16_t stdOffset      ;
      if (httpOnSettings_Month      (tzDstMonth      , dstMonth      ) &&
          httpOnSettings_WeekOfMonth(tzDstWeekOfMonth, dstWeekOfMonth) &&
          httpOnSettings_DayOfWeek  (tzDstDayOfWeek  , dstDayOfWeek  ) &&
          ascInt2bin                (tzDstHour       , dstHour       , (uint8_t)0   , (uint8_t) 23) &&
          ascInt2bin                (tzDstOffset     , dstOffset     , (int16_t)-840, (int16_t)840) &&
          httpOnSettings_Month      (tzStdMonth      , stdMonth      ) &&
          httpOnSettings_WeekOfMonth(tzStdWeekOfMonth, stdWeekOfMonth) &&
          httpOnSettings_DayOfWeek  (tzStdDayOfWeek  , stdDayOfWeek  ) &&
          ascInt2bin                (tzStdHour       , stdHour       , (uint8_t)0   , (uint8_t) 23) &&
          ascInt2bin                (tzStdOffset     , stdOffset     , (int16_t)-840, (int16_t)840))
      {
        settings._tzDstMonth       = dstMonth       ;
        settings._tzDstWeekOfMonth = dstWeekOfMonth ;
        settings._tzDstDayOfWeek   = dstDayOfWeek   ;
        settings._tzDstHour        = dstHour        ;
        settings._tzDstOffset      = dstOffset      ;
        settings._tzStdMonth       = stdMonth       ;
        settings._tzStdWeekOfMonth = stdWeekOfMonth ;
        settings._tzStdDayOfWeek   = stdDayOfWeek   ;
        settings._tzStdHour        = stdHour        ;
        settings._tzStdOffset      = stdOffset      ;

        TimeChangeRule dst = { "dst", dstWeekOfMonth, dstDayOfWeek, dstMonth, dstHour, dstOffset } ;
        TimeChangeRule std = { "std", stdWeekOfMonth, stdDayOfWeek, stdMonth, stdHour, stdOffset } ;
        tz = Timezone(dst, std) ;

        settingsDirty = true ;
        ntpDirty = true ; // todo
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
                  ntp.toLocalString() +
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
                         "</table>\n"
                         "<table>\n"
                         "  <tr><th>Zeitzone</th><th>Monat</th><th>Woche</th><th>Wochentag</th><th>Stunde</th><th>Minuten zu UTC</th></tr>\n"
                         "  <tr><th>Start Sommerzeit</th><td>") +
                  inputMonth("tzDstMonth", settings._tzDstMonth) +
                  String("</td><td>") +
                  inputWeekOfMonth("tzDstWeekOfMonth", settings._tzDstWeekOfMonth) +
                  String("</td><td>") +
                  inputDayOfWeek("tzDstDayOfWeek", settings._tzDstDayOfWeek) +
                  String("</td><td>") +
                  inputInt("tzDstHour", settings._tzDstHour, (uint8_t)0, (uint8_t)23) +
                  String("</td><td>") +
                  inputInt("tzDstOffset", settings._tzDstOffset, (int16_t)-840, (int16_t)840) +
                  String("</td></tr>"
                         "  <th>Ende Sommerzeit</th><td>") +
                  inputMonth("tzStdMonth", settings._tzStdMonth) +
                  String("</td><td>") +
                  inputWeekOfMonth("tzStdWeekOfMonth", settings._tzStdWeekOfMonth) +
                  String("</td><td>") +
                  inputDayOfWeek("tzStdDayOfWeek", settings._tzStdDayOfWeek) +
                  String("</td><td>") +
                  inputInt("tzStdHour", settings._tzStdHour, (uint8_t)0, (uint8_t)23) +
                  String("</td><td>") +
                  inputInt("tzStdOffset", settings._tzStdOffset, (int16_t)-840, (int16_t)840) +
                  String("</td></tr>"
                         "</table>\n"
                         "<button type=\"submit\" name=\"action\" value=\"wifi\">Zugangsdaten setzen</button>\n"
                         "</form>\n"
                         "</div>\n") +
                  httpFooter()) ;
}

////////////////////////////////////////////////////////////////////////////////
// EOF
////////////////////////////////////////////////////////////////////////////////
