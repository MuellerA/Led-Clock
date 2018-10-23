////////////////////////////////////////////////////////////////////////////////
// Http
////////////////////////////////////////////////////////////////////////////////

void HttpSetup()
{
  httpServer.on("/", httpOnHome) ;
  httpServer.on("/settings.html", httpOnSettings) ;
  httpServer.on("/reboot", httpOnReboot) ;
  httpUpdater.setup(&httpServer, "/update", settings._apSsid.c_str(), settings._apPsk.c_str());
  httpServer.begin() ;
}

////////////////////////////////////////////////////////////////////////////////

class HttpBuff
{
public:
  HttpBuff()
  {
    _buff.reserve(512) ;
  }
  ~HttpBuff()
  {
    if (_buff.length())
      httpServer.sendContent(_buff) ;
  }
  void operator+=(char c)
  {
    _buff += c ;
    if (_buff.length() == 512)
    {
      httpServer.sendContent(_buff) ;
      _buff = "" ;
    }
  }
  void operator+=(const String &s)
  {
    _buff += s ;
    if (_buff.length() >= 512)
    {
      httpServer.sendContent(_buff) ;
      _buff = "" ;
    }
  }

private:
  String _buff ;
} ;

////////////////////////////////////////////////////////////////////////////////

struct Translate
{
  String _lang[(unsigned int)Lang::Size] ;
  const String& operator()() const { return _lang[(unsigned int)settings._lang] ; }
  bool operator==(const String &cmp)
  {
    for (auto l : _lang)
      if (l == cmp)
        return true ;
    return false ;
  }
  bool operator==(const char *cmp)
  {
    for (auto l : _lang)
      if (l == cmp)
        return true ;
    return false ;
  }
} ;

Translate transJanuary   { "January"  , "Januar"    } ;
Translate transFebruary  { "February" , "Februar"   } ;
Translate transMarch     { "March"    , "März"      } ;
Translate transApril     { "April"    , "April"     } ;
Translate transMay       { "May"      , "Mail"      } ;
Translate transJune      { "June"     , "Juni"      } ;
Translate transJuly      { "July"     , "Juli"      } ;
Translate transAugust    { "August"   , "August"    } ;
Translate transSeptember { "September", "September" } ;
Translate transOctober   { "October"  , "Oktober"   } ;
Translate transNovember  { "November" , "November"  } ;
Translate transDecember  { "December" , "Dezember"  } ;

Translate transSunday    { "Sunday"   , "Sonntag"   } ;
Translate transMonday    { "Monday"   , "Montag"    } ;
Translate transTuesday   { "Tuesday"  , "Dienstag"  } ;
Translate transWednesday { "Wednesday", "Mittwoch"  } ;
Translate transThursday  { "Thursday" , "Donnerstag"} ;
Translate transFriday    { "Friday"   , "Freitag"   } ;
Translate transSaturday  { "Saturday" , "Samstag"   } ;

Translate transFirst     { "First"    , "Erster"    } ;
Translate transSecond    { "Second"   , "Zweiter"   } ;
Translate transThird     { "Third"    , "Dritter"   } ;
Translate transFourth    { "Fourth"   , "Vierter"   } ;
Translate transLast      { "Last"     , "Letzter"   } ;

Translate translateWords[] =
  {
    { "Clock"           , "Uhr"                     },
    { "Home"            , "Startseite"              },
    { "Settings"        , "Einstellungen"           },
    { "Update"          , "Aktualisieren"           },
    { "Reboot"          , "Neustart"                },
    { "manual"          , "manuell"                 },
    { "Save Settings"   , "Einstellungen speichern" },
    { "Save Color"      , "Farbe speichern"         },
    { "Save Time"       , "Zeit speichern"          },
    { "Save NTP"        , "NTP speichern"           },
    
    { "WiFi"            , "WLAN"                },
    { "Color"           , "Farbe"               },
    { "Hour"            , "Stunde"              },
    { "Minute"          , "Minute"              },
    { "Second"          , "Sekunde"             },
    { "Time"            , "Zeit"                },
    { "Timezone"        , "Zeitzone"            },
    { "Daylight Saving Time", "Sommerzeit"      },
    { "Start"           , "Start"               },
    { "End"             , "Ende"                },
    { "Month"           , "Monat"               },
    { "Week"            , "Woche"               },
    { "Day"             , "Tag"                 },
    { "Hour"            , "Stunde"              },
    { "Offset to UTC (min)" , "Abstand zu UTC (Min)" },

    transJanuary,
    transFebruary,
    transMarch,
    transApril,
    transMay,
    transJune,
    transJuly,
    transAugust,
    transSeptember,
    transOctober,
    transNovember,
    transDecember,
                  
    transSunday,
    transMonday,
    transTuesday,
    transWednesday,
    transThursday,
    transFriday,
    transSaturday,
                  
    transFirst,
    transSecond,
    transThird,
    transFourth,
    transLast,
} ;

String translate(const String &orig)
{
  for (const Translate &t : translateWords)
    if (t._lang[0] == orig) return t() ;

  if      (orig == "@NtpManual"   ) return ntp.active() ? "(NTP)" : translate("(manual)") ;
  else if (orig == "@NtpTime"     ) return ntp.toLocalString() ;
  else if (orig == "@Ntp"         ) return settings._ntp ;
  else if (orig == "@Ssid"        ) return settings._ssid ;
  else if (orig == "@ColHour"     ) return settings._colHour.toString() ;
  else if (orig == "@ColMinute"   ) return settings._colMinute.toString() ;
  else if (orig == "@ColSecond"   ) return settings._colSecond.toString() ;
  else if (orig == "@DstMonth"    ) return inputMonth("tzDstMonth" , settings._tzDstMonth ) ;
  else if (orig == "@DstWeek"     ) return inputWeek ("tzDstWeek"  , settings._tzDstWeek  ) ;
  else if (orig == "@DstDay"      ) return inputDay  ("tzDstDay"   , settings._tzDstDay   ) ;
  else if (orig == "@DstHour"     ) return inputInt  ("tzDstHour"  , settings._tzDstHour  , (uint8_t)   0, (uint8_t) 23) ;
  else if (orig == "@DstOffset"   ) return inputInt  ("tzDstOffset", settings._tzDstOffset, (int16_t)-840, (int16_t)840) ;
  else if (orig == "@StdMonth"    ) return inputMonth("tzStdMonth" , settings._tzStdMonth ) ;
  else if (orig == "@StdWeek"     ) return inputWeek ("tzStdWeek"  , settings._tzStdWeek  ) ;
  else if (orig == "@StdDay"      ) return inputDay  ("tzStdDay"   , settings._tzStdDay   ) ;
  else if (orig == "@StdHour"     ) return inputInt  ("tzStdHour"  , settings._tzStdHour  , (uint8_t)   0, (uint8_t) 23) ;
  else if (orig == "@StdOffset"   ) return inputInt  ("tzStdOffset", settings._tzStdOffset, (int16_t)-840, (int16_t)840) ;

  return orig ;
}

String translateHome    (const String &orig) { return (orig == "Page") ? translate("Home"    ) : translate(orig) ; }
String translateReboot  (const String &orig) { return (orig == "Page") ? translate("Reboot"  ) : translate(orig) ; }
String translateSettings(const String &orig) { return (orig == "Page") ? translate("Settings") : translate(orig) ; }



////////////////////////////////////////////////////////////////////////////////

void httpTemplateReplace_P(PGM_P temp, HttpBuff &httpBuff, unsigned int &idx, std::function<String(const String&)> translate)
{
  char ch ;
  String buff ;
  buff.reserve(32) ;

  while (true)
  {
    ch = (char) pgm_read_byte(temp + idx++) ;

    switch (ch)
    {
    case 0:
      httpBuff += buff ;
      --idx ;
      return ;

    case '%':
      httpBuff += translate(buff) ;
      return ;

    default:
      {
        buff += ch ;
        if (buff.length() == 64)
        {
          httpBuff += buff ;
          return ;
        }
      }
      break ;
    }
  }
}

void httpTemplate_P(PGM_P temp, std::function<String(const String&)> translate)
{
  unsigned int idx = 0 ;
  char ch ;

  HttpBuff httpBuff ;

  while (true)
  {
    ch = (char) pgm_read_byte(temp + idx++) ;

    switch (ch)
    {
    case 0:
      return ;

    case '%':
      if ((char)pgm_read_byte(temp + idx) == '%')
      {
        httpBuff += '%' ;
        idx++ ;
        break ;
      }

      httpTemplateReplace_P(temp, httpBuff, idx, translate) ;
      break ;

    default:
      httpBuff += ch ;
      break ;
    }
  }
}

////////////////////////////////////////////////////////////////////////////////

const char HttpHeader_P[] PROGMEM =
  "<!DOCTYPE html>\n"
  "<html>\n"
  " <head>\n"
  "  <meta charset=\"utf-8\"/>\n"
  "  <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\"/>\n"
  "  <title>%Clock% - %Page%</title>\n"
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
  "  <h1>%Clock% - %Page%</h1>\n" ;

const char HttpFooter_P[] PROGMEM =
  " </body>\n"
  "</html>\n" ;

////////////////////////////////////////////////////////////////////////////////

const char HttpHome_P[] PROGMEM =
  "<p style=\"font-size: small\">"
  "<a href=\"/settings.html\">%Settings%</a> <a href=\"/update\">%Update%</a> <a href=\"/reboot\">%Reboot%</a>"
  "</p>\n"
  "<div>\n"
  "<span class=\"border\" style=\"font-size: 500%%\">%@NtpTime%</span> %@NtpManual%"
  "</div>\n" ;

////////////////////////////////////////////////////////////////////////////////

const char HttpReboot_P[] PROGMEM =
  "<p>Starte neu...</p>\n" ;

////////////////////////////////////////////////////////////////////////////////

const char HttpSettings_P[] PROGMEM =
  "<p style=\"font-size: small\"><a href=\"/\">%Home%</a></p>\n"
  "\n"
  "<div class=\"border\">\n" // Language
  "<h2>Sprache / Language</h2>\n"
  "<form action=\"settings.html\" method=\"post\" autocomplete=\"off\">\n"
  "<button type=\"submit\" name=\"action\" value=\"deutsch\">Deutsch</button>\n"
  "<button type=\"submit\" name=\"action\" value=\"english\">English</button>\n"
  "</form>\n"
  "</div>\n"
  "\n"
  "<div class=\"border\">\n" // WiFi
  "<h2>%WiFi%</h2>\n"
  "<form action=\"settings.html\" method=\"post\" autocomplete=\"off\">\n"
  "<table>\n"
  "  <tr><td>SSID</td><td><input type=\"text\" name=\"ssid\" size=\"32\" value=\"%@Ssid%\"/></td></tr>\n"
  "  <tr><td>PSK</td><td><input type=\"password\" name=\"psk\" size=\"32\" value=\"\"/></td></tr>\n"
  "</table>\n"
  "<button type=\"submit\" name=\"action\" value=\"wifi\">%Save Settings%</button>\n"
  "</form>\n"
  "</div>\n"
  "\n"
  "<div class=\"border\">\n" // Color
  "<h2>%Color%</h2>\n"
  "<form action=\"settings.html\" method=\"post\" autocomplete=\"off\">\n"
  "<table>\n"
  " <tr><td>%Hour%</td><td><input type=\"color\" name=\"colorHour\" value=\"%@ColHour%\"/></td></tr>\n"
  " <tr><td>%Minute%</td><td><input type=\"color\" name=\"colorMinute\" value=\"%@ColMinute%\"/></td></tr>\n"
  " <tr><td>%Second%</td><td><input type=\"color\" name=\"colorSecond\" value=\"%@ColSecond%\"/></td></tr>\n"
  "</table>\n"
  "<button type=\"submit\" name=\"action\" value=\"color\">%Save Color%</button>\n"
  "</form>\n"
  "</div>\n"
  "\n"
  "<div class=\"border\">\n" // Timezone
  "<h2>%Time% (NTP)</h2>\n"
  "<form action=\"settings.html\" method=\"post\" autocomplete=\"off\">\n"
  "<table>\n"
  "  <tr><td>NTP</td><td><input type=\"text\" name=\"ntp\" size=\"32\" value=\"%@Ntp%\" placeholder=\"pool.ntp.org\"/></td></tr>\n"
  "</table>\n"
  "<table>\n"
  "  <tr><th>%Timezone%</th><th>%Month%</th><th>%Week%</th><th>%Day%</th><th>%Hour%</th><th>%Offset to UTC (min)%</th></tr>\n"
  "  <tr><th>%Start% %Daylight Saving Time%</th><td>%@DstMonth%</td><td>%@DstWeek%</td><td>%@DstDay%</td><td>%@DstHour%</td><td>%@DstOffset%</td></tr>\n"
  "  <tr><th>%End% %Daylight Saving Time%</th><td>%@StdMonth%</td><td>%@StdWeek%</td><td>%@StdDay%</td><td>%@StdHour%</td><td>%@StdOffset%</td></tr>\n"
  "</table>\n"
  "<button type=\"submit\" name=\"action\" value=\"timezone\">%Save NTP%</button>\n"
  "</form>\n"
  "</div>\n"
  "\n"
  "<div class=\"border\">\n" // Time
  "<h2>%Time% (%manual%)</h2>\n"
  "<form action=\"settings.html\" method=\"post\" autocomplete=\"off\">\n"
  "<input type=\"time\" name=\"time\" step=\"1\" value=\"%@NtpTime%\"/>\n"
  "<button type=\"submit\" name=\"action\" value=\"time\">%Save Time%</button>\n"
  "</form>\n"
  "</div>\n" ;

////////////////////////////////////////////////////////////////////////////////

void httpOk()
{
  httpServer.setContentLength(CONTENT_LENGTH_UNKNOWN);
  httpServer.sendHeader("Content-Type","text/html",true);
  httpServer.sendHeader("Cache-Control","no-cache");
  httpServer.send(200);
}

void httpOnHome()
{
  httpOk() ;
  httpTemplate_P(HttpHeader_P, translateHome) ;
  httpTemplate_P(HttpHome_P  , translateHome) ;
  httpTemplate_P(HttpFooter_P, translateHome) ;
  httpServer.sendContent("") ;
}

void httpOnReboot()
{
  httpOk() ;
  httpTemplate_P(HttpHeader_P, translateReboot) ;
  httpTemplate_P(HttpReboot_P, translateReboot) ;
  httpTemplate_P(HttpFooter_P, translateReboot) ;
  httpServer.sendContent("") ;

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
  if (transJanuary   == month) { value = TZ::Month::Jan ; return true ; }
  if (transFebruary  == month) { value = TZ::Month::Feb ; return true ; }
  if (transMarch     == month) { value = TZ::Month::Mar ; return true ; }
  if (transApril     == month) { value = TZ::Month::Apr ; return true ; }
  if (transMay       == month) { value = TZ::Month::May ; return true ; }
  if (transJune      == month) { value = TZ::Month::Jun ; return true ; }
  if (transJuly      == month) { value = TZ::Month::Jul ; return true ; }
  if (transAugust    == month) { value = TZ::Month::Aug ; return true ; }
  if (transSeptember == month) { value = TZ::Month::Sep ; return true ; }
  if (transOctober   == month) { value = TZ::Month::Oct ; return true ; }
  if (transNovember  == month) { value = TZ::Month::Nov ; return true ; }
  if (transDecember  == month) { value = TZ::Month::Dec ; return true ; }
  return false ;
}

bool httpOnSettings_Week(const String &week, TZ::Week &value)
{
  if (transFirst  == week) { value = TZ::Week::First  ; return true ; }
  if (transSecond == week) { value = TZ::Week::Second ; return true ; }
  if (transThird  == week) { value = TZ::Week::Third  ; return true ; }
  if (transFourth == week) { value = TZ::Week::Fourth ; return true ; }
  if (transLast   == week) { value = TZ::Week::Last   ; return true ; }
  return false ;
}

bool httpOnSettings_Day(const String &day, TZ::Day &value)
{
  if (transSunday    == day) { value = TZ::Day::Sun ; return true ; }
  if (transMonday    == day) { value = TZ::Day::Mon ; return true ; }
  if (transTuesday   == day) { value = TZ::Day::Tue ; return true ; }
  if (transWednesday == day) { value = TZ::Day::Wed ; return true ; }
  if (transThursday  == day) { value = TZ::Day::Thu ; return true ; }
  if (transFriday    == day) { value = TZ::Day::Fri ; return true ; }
  if (transSaturday  == day) { value = TZ::Day::Sat ; return true ; }
}

template<typename T>
const String inputOption(const String &opt, T a, T b)
{
  static const String optionBegin { "<option" } ;
  static const String optionEnd   { "</option>" } ;
  static const String selected    { " selected>" } ;
  static const String notSelected { ">" } ;

  return
    optionBegin +
    ((a == b) ? selected : notSelected) +
    opt +
    optionEnd ;
}

String inputMonth(const char *name, TZ::Month month)
{
  return
    String("<select name=\"") + String(name) + String("\" size=\"1\">") +
    inputOption(transJanuary()  , month, TZ::Month::Jan) +
    inputOption(transFebruary() , month, TZ::Month::Feb) +
    inputOption(transMarch()    , month, TZ::Month::Mar) +
    inputOption(transApril()    , month, TZ::Month::Apr) +
    inputOption(transMay()      , month, TZ::Month::May) +
    inputOption(transJune()     , month, TZ::Month::Jun) +
    inputOption(transJuly()     , month, TZ::Month::Jul) +
    inputOption(transAugust()   , month, TZ::Month::Aug) +
    inputOption(transSeptember(), month, TZ::Month::Sep) +
    inputOption(transOctober()  , month, TZ::Month::Oct) +
    inputOption(transNovember() , month, TZ::Month::Nov) +
    inputOption(transDecember() , month, TZ::Month::Dec) +
    String("</select>") ;
}
String inputWeek(const char *name, TZ::Week week)
{
  return
    String("<select name=\"") + String(name) + String("\" size=\"1\">") +
    inputOption(transFirst() , week, TZ::Week::First ) +
    inputOption(transSecond(), week, TZ::Week::Second) +
    inputOption(transThird() , week, TZ::Week::Third ) +
    inputOption(transFourth(), week, TZ::Week::Fourth) +
    inputOption(transLast()  , week, TZ::Week::Last  ) +
    String("</select>") ;
}
String inputDay(const char *name, TZ::Day day)
{
  return
    String("<select name=\"") + String(name) + String("\" size=\"1\">") +
    inputOption(transSunday()   , day, TZ::Day::Sun) +
    inputOption(transMonday()   , day, TZ::Day::Mon) +
    inputOption(transTuesday()  , day, TZ::Day::Tue) +
    inputOption(transWednesday(), day, TZ::Day::Wed) +
    inputOption(transThursday() , day, TZ::Day::Thu) +
    inputOption(transFriday()   , day, TZ::Day::Fri) +
    inputOption(transSaturday() , day, TZ::Day::Sat) +
    String("</select>") ;
}
template<typename T>
String inputInt(const char *name, T val, T min, T max)
{
  return
    String("<input type=\"number\" size=\"3\" name=\"") +
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

    if (action == "deutsch")
    {
      settings._lang = Lang::DE ;
      settingsDirty = true ;
    }
    else if (action == "english")
    {
      settings._lang = Lang::EN ;
      settingsDirty = true ;
    }
    else if (action == "wifi")
    {
      String ssid = httpServer.arg("ssid") ;
      String psk  = httpServer.arg("psk" ) ;

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
    else if (action == "time")
    {
      ntp.fromLocalString(httpServer.arg("time")) ;
    }
    else if (action == "timezone")
    {
      String ntp  = httpServer.arg("ntp" ) ;
      String tzDstMonth  = httpServer.arg("tzDstMonth" ) ;
      String tzDstWeek   = httpServer.arg("tzDstWeek"  ) ;
      String tzDstDay    = httpServer.arg("tzDstDay"   ) ;
      String tzDstHour   = httpServer.arg("tzDstHour"  ) ;
      String tzDstOffset = httpServer.arg("tzDstOffset") ;
      String tzStdMonth  = httpServer.arg("tzStdMonth" ) ;
      String tzStdWeek   = httpServer.arg("tzStdWeek"  ) ;
      String tzStdDay    = httpServer.arg("tzStdDay"   ) ;
      String tzStdHour   = httpServer.arg("tzStdHour"  ) ;
      String tzStdOffset = httpServer.arg("tzStdOffset") ;

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
        ntpDirty = true ;
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

  httpOk() ;
  httpTemplate_P(HttpHeader_P, translateSettings) ;
  httpTemplate_P(HttpSettings_P, translateSettings) ;
  httpTemplate_P(HttpFooter_P, translateSettings) ;
  httpServer.sendContent("") ;
}

////////////////////////////////////////////////////////////////////////////////
// EOF
////////////////////////////////////////////////////////////////////////////////
