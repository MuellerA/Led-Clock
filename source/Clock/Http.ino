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
                  String("<p><span style=\"font-size: large\">") + ntp.toLocalString() + String("</span> ") +
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

bool httpOnSettings_Month(const String &month, TZ::Month &value)
{
  if (month == "Januar"   ) { value = TZ::Month::Jan ; return true ; }
  if (month == "Februar"  ) { value = TZ::Month::Feb ; return true ; }
  if (month == "März"     ) { value = TZ::Month::Mar ; return true ; }
  if (month == "April"    ) { value = TZ::Month::Apr ; return true ; }
  if (month == "Mail"     ) { value = TZ::Month::May ; return true ; }
  if (month == "Juni"     ) { value = TZ::Month::Jun ; return true ; }
  if (month == "Juli"     ) { value = TZ::Month::Jul ; return true ; }
  if (month == "August"   ) { value = TZ::Month::Aug ; return true ; }
  if (month == "September") { value = TZ::Month::Sep ; return true ; }
  if (month == "Oktober"  ) { value = TZ::Month::Oct ; return true ; }
  if (month == "November" ) { value = TZ::Month::Nov ; return true ; }
  if (month == "Dezember" ) { value = TZ::Month::Dec ; return true ; }
  return false ;
}

bool httpOnSettings_Week(const String &week, TZ::Week &value)
{
  if (week == "Erster"  ) { value = TZ::Week::First  ; return true ; }
  if (week == "Zweiter" ) { value = TZ::Week::Second ; return true ; }
  if (week == "Dritter" ) { value = TZ::Week::Third  ; return true ; }
  if (week == "Vierter" ) { value = TZ::Week::Fourth ; return true ; }
  if (week == "Letzter" ) { value = TZ::Week::Last   ; return true ; }
  return false ;
}

bool httpOnSettings_Day(const String &day, TZ::Day &value)
{
  if (day == "Sonntag"    ) { value = TZ::Day::Sun ; return true ; }
  if (day == "Montag"     ) { value = TZ::Day::Mon ; return true ; }
  if (day == "Dienstag"   ) { value = TZ::Day::Tue ; return true ; }
  if (day == "Mittwoch"   ) { value = TZ::Day::Wed ; return true ; }
  if (day == "Donnerstag" ) { value = TZ::Day::Thu ; return true ; }
  if (day == "Freitag"    ) { value = TZ::Day::Fri ; return true ; }
  if (day == "Samstag"    ) { value = TZ::Day::Sat ; return true ; }
}

String inputMonth(const char *name, TZ::Month month)
{
  return
    String("<select name=\"") +
    String(name) +
    String("\" size=\"1\"><option"     ) + String(month == TZ::Month::Jan ? " selected" : "") +
    String(">Januar</option><option"   ) + String(month == TZ::Month::Feb ? " selected" : "") +
    String(">Februar</option><option"  ) + String(month == TZ::Month::Mar ? " selected" : "") +
    String(">März</option><option"     ) + String(month == TZ::Month::Apr ? " selected" : "") +
    String(">April</option><option"    ) + String(month == TZ::Month::May ? " selected" : "") +
    String(">Mai</option><option"      ) + String(month == TZ::Month::Jun ? " selected" : "") +
    String(">Juni</option><option"     ) + String(month == TZ::Month::Jul ? " selected" : "") +
    String(">Juli</option><option"     ) + String(month == TZ::Month::Aug ? " selected" : "") +
    String(">August</option><option"   ) + String(month == TZ::Month::Sep ? " selected" : "") +
    String(">September</option><option") + String(month == TZ::Month::Oct ? " selected" : "") +
    String(">Oktober</option><option"  ) + String(month == TZ::Month::Nov ? " selected" : "") +
    String(">November</option><option" ) + String(month == TZ::Month::Dec ? " selected" : "") +
    String(">Dezember</option></select>") ;
}
String inputWeek(const char *name, TZ::Week week)
{
  return
    String("<select name=\"") +
    String(name) +
    String("\" size=\"1\"><option"   ) + String(week == TZ::Week::First  ? " selected" : "") +
    String(">Erster</option><option" ) + String(week == TZ::Week::Second ? " selected" : "") +
    String(">Zweiter</option><option") + String(week == TZ::Week::Third  ? " selected" : "") +
    String(">Dritter</option><option") + String(week == TZ::Week::Fourth ? " selected" : "") +
    String(">Vierter</option><option") + String(week == TZ::Week::Last   ? " selected" : "") +
    String(">Letzter</option></select>") ;
}
String inputDay(const char *name, TZ::Day day)
{
  return
    String("<select name=\"") +
    String(name) +
    String("\" size=\"1\"><option"      ) + String(day == TZ::Day::Sun ? " selected" : "") +
    String(">Sonntag</option><option"   ) + String(day == TZ::Day::Mon ? " selected" : "") +
    String(">Montag</option><option"    ) + String(day == TZ::Day::Tue ? " selected" : "") +
    String(">Dienstag</option><option"  ) + String(day == TZ::Day::Wed ? " selected" : "") +
    String(">Mittwoch</option><option"  ) + String(day == TZ::Day::Thu ? " selected" : "") +
    String(">Donnerstag</option><option") + String(day == TZ::Day::Fri ? " selected" : "") +
    String(">Freitag</option><option"   ) + String(day == TZ::Day::Sat ? " selected" : "") +
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
      String tzDstMonth  = httpServer.arg("tzDstMonth"      ) ;
      String tzDstWeek   = httpServer.arg("tzDstWeekOfMonth") ;
      String tzDstDay    = httpServer.arg("tzDstDayOfWeek"  ) ;
      String tzDstHour   = httpServer.arg("tzDstHour"       ) ;
      String tzDstOffset = httpServer.arg("tzDstOffset"     ) ;
      String tzStdMonth  = httpServer.arg("tzStdMonth"      ) ;
      String tzStdWeek   = httpServer.arg("tzStdWeekOfMonth") ;
      String tzStdDay    = httpServer.arg("tzStdDayOfWeek"  ) ;
      String tzStdHour   = httpServer.arg("tzStdHour"       ) ;
      String tzStdOffset = httpServer.arg("tzStdOffset"     ) ;

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

      TZ::Month dstMonth       ;
      TZ::Week  dstWeek        ;
      TZ::Day   dstDay         ;
      uint8_t   dstHour        ;
      int16_t   dstOffset      ;
      TZ::Month stdMonth       ;
      TZ::Week  stdWeek        ;
      TZ::Day   stdDay         ;
      uint8_t   stdHour        ;
      int16_t   stdOffset      ;
      if (httpOnSettings_Month(tzDstMonth , dstMonth ) &&
          httpOnSettings_Week (tzDstWeek  , dstWeek  ) &&
          httpOnSettings_Day  (tzDstDay   , dstDay   ) &&
          ascInt2bin          (tzDstHour  , dstHour  , (uint8_t)0   , (uint8_t) 23) &&
          ascInt2bin          (tzDstOffset, dstOffset, (int16_t)-840, (int16_t)840) &&
          httpOnSettings_Month(tzStdMonth , stdMonth ) &&
          httpOnSettings_Week (tzStdWeek  , stdWeek  ) &&
          httpOnSettings_Day  (tzStdDay   , stdDay   ) &&
          ascInt2bin          (tzStdHour  , stdHour  , (uint8_t)0   , (uint8_t) 23) &&
          ascInt2bin          (tzStdOffset, stdOffset, (int16_t)-840, (int16_t)840))
      {
        settings._tzDstMonth  = dstMonth  ;
        settings._tzDstWeek   = dstWeek   ;
        settings._tzDstDay    = dstDay    ;
        settings._tzDstHour   = dstHour   ;
        settings._tzDstOffset = dstOffset ;
        settings._tzStdMonth  = stdMonth  ;
        settings._tzStdWeek   = stdWeek   ;
        settings._tzStdDay    = stdDay    ;
        settings._tzStdHour   = stdHour   ;
        settings._tzStdOffset = stdOffset ;

        tz.resetRules() ;
        tz.addRule( { dstMonth, dstWeek, dstDay, dstHour, dstOffset } ) ;
        tz.addRule( { stdMonth, stdWeek, stdDay, stdHour, stdOffset } ) ;

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
                  inputWeek("tzDstWeekOfMonth", settings._tzDstWeek) +
                  String("</td><td>") +
                  inputDay("tzDstDayOfWeek", settings._tzDstDay) +
                  String("</td><td>") +
                  inputInt("tzDstHour", settings._tzDstHour, (uint8_t)0, (uint8_t)23) +
                  String("</td><td>") +
                  inputInt("tzDstOffset", settings._tzDstOffset, (int16_t)-840, (int16_t)840) +
                  String("</td></tr>"
                         "  <th>Ende Sommerzeit</th><td>") +
                  inputMonth("tzStdMonth", settings._tzStdMonth) +
                  String("</td><td>") +
                  inputWeek("tzStdWeekOfMonth", settings._tzStdWeek) +
                  String("</td><td>") +
                  inputDay("tzStdDayOfWeek", settings._tzStdDay) +
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
