////////////////////////////////////////////////////////////////////////////////
// Http
////////////////////////////////////////////////////////////////////////////////

#if (HTTPS == HTTPS_axTLS) || (HTTPS == HTTPS_BearSSL)

static const uint8_t GenericCert[] PROGMEM =
{
#include "https-cert.h"
};
static char*    CustomCertFileName = "/cert.der" ;
static int      CustomCertLen      = 0 ;
static uint8_t* CustomCert         = nullptr ;

static const uint8_t GenericKey[]  PROGMEM =
{
#include "https-key.h"
} ;

static char*    CustomKeyFileName  = "/key.der"  ;
static int      CustomKeyLen       = 0 ;
static uint8_t* CustomKey          = nullptr ;
#endif

////////////////////////////////////////////////////////////////////////////////

void HttpSetup()
{
#if (HTTPS == HTTPS_axTLS) || (HTTPS == HTTPS_BearSSL)
  bool customCert = false ;

  if (SPIFFS.exists(CustomCertFileName) && SPIFFS.exists(CustomKeyFileName))
  {
    File certFile = SPIFFS.open(CustomCertFileName, "r") ;
    File keyFile  = SPIFFS.open(CustomKeyFileName , "r") ;
    if (certFile && keyFile)
    {
      CustomCertLen = certFile.size() ;
      CustomCert = (uint8_t*) malloc(CustomCertLen) ;
      certFile.read(CustomCert, CustomCertLen) ;
      certFile.close() ;

      CustomKeyLen = keyFile.size() ;
      CustomKey = (uint8_t*) malloc(CustomKeyLen) ;
      keyFile.read(CustomKey, CustomKeyLen) ;
      keyFile.close() ;

      httpServer.setServerKeyAndCert(CustomKey, CustomKeyLen, CustomCert, CustomCertLen) ;
      customCert = true ;
    }

    if (!customCert)
      Serial.println("cannot read custom https credentials") ;
  }

  if (!customCert) // no SPIFS cert or empty files
  {
    Serial.println("using generic https credentials") ;
    httpServer.setServerKeyAndCert_P(GenericKey, sizeof(GenericKey), GenericCert, sizeof(GenericCert)) ;
  }
#endif
  
  httpServer.on("/", httpOnHome) ;
  httpServer.on("/on", httpOnOn) ;
  httpServer.on("/off", httpOnOff) ;
  httpServer.on("/set", httpOnSet) ;
  httpServer.on("/settings.html", httpOnSettings) ;
  httpServer.on("/clock.css", httpOnCss) ;
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

static const std::map<String, Translate> TranslateWord
{
  { "Switch On/Off"       , { "Switch On/Off"       , "Ein-/Ausschalten"       } },
  { "Off"                 , { "Off"                 , "Aus"                    } },
  { "On"                  , { "On"                  , "Ein"                    } },
  { "Automatic"           , { "Automatic"           , "Automatisch"            } },
  { "Clock"               , { "Clock"               , "Uhr"                    } },
  { "Home"                , { "Home"                , "Startseite"             } },
  { "Settings"            , { "Settings"            , "Einstellungen"          } },
  { "Update Firmware"     , { "Update Firmware"     , "Firmware aktualisieren" } },
  { "Update"              , { "Update"              , "Aktualisieren"          } },
  { "Reboot"              , { "Reboot"              , "Neustart"               } },
  { "manual"              , { "manual"              , "manuell"                } },
  { "Save On/Off"         , { "Save On/Off"         , "Automatik speichern"    } },
  { "Save Color"          , { "Save Color"          , "Farbe speichern"        } },
  { "Save WiFi"           , { "Save WiFi"           , "WLAN speichern"         } },
  { "Save Time"           , { "Save Time"           , "Zeit speichern"         } },
  { "Save NTP"            , { "Save NTP"            , "NTP speichern"          } },
                        
  { "WiFi"                , { "WiFi"                , "WLAN"                   } },
  { "Color"               , { "Color"               , "Farbe"                  } },
  { "Hour"                , { "Hour"                , "Stunde"                 } },
  { "Minute"              , { "Minute"              , "Minute"                 } },
  { "Second"              , { "Second"              , "Sekunde"                } },
  { "Time"                , { "Time"                , "Zeit"                   } },
  { "Timezone"            , { "Timezone"            , "Zeitzone"               } },
  { "Daylight Saving Time", { "Daylight Saving Time", "Sommerzeit"             } },
  { "Standard Time"       , { "Standard Time"       , "Normalzeit"             } },
  { "Start"               , { "Start"               , "Start"                  } },
  { "End"                 , { "End"                 , "Ende"                   } },
  { "Month"               , { "Month"               , "Monat"                  } },
  { "Week"                , { "Week"                , "Woche"                  } },
  { "Day"                 , { "Day"                 , "Tag"                    } },
  { "Hour"                , { "Hour"                , "Stunde"                 } },
  { "Offset to UTC (min)" , { "Offset to UTC (min)" , "Abstand zu UTC (Min)"   } },

    
  { transJanuary  ._lang[0], transJanuary   },
  { transFebruary ._lang[0], transFebruary  },
  { transMarch    ._lang[0], transMarch     },
  { transApril    ._lang[0], transApril     },
  { transMay      ._lang[0], transMay       },
  { transJune     ._lang[0], transJune      },
  { transJuly     ._lang[0], transJuly      },
  { transAugust   ._lang[0], transAugust    },
  { transSeptember._lang[0], transSeptember },
  { transOctober  ._lang[0], transOctober   },
  { transNovember ._lang[0], transNovember  },
  { transDecember ._lang[0], transDecember  },
                  
  { transSunday   ._lang[0], transSunday    },
  { transMonday   ._lang[0], transMonday    },
  { transTuesday  ._lang[0], transTuesday   },
  { transWednesday._lang[0], transWednesday },
  { transThursday ._lang[0], transThursday  },
  { transFriday   ._lang[0], transFriday    },
  { transSaturday ._lang[0], transSaturday  },
                  
  { transFirst    ._lang[0], transFirst     },
  { transSecond   ._lang[0], transSecond    },
  { transThird    ._lang[0], transThird     },
  { transFourth   ._lang[0], transFourth    },
  { transLast     ._lang[0], transLast      },
} ;

static const std::map<String,std::function<String()> > TranslateFunc
{
  { "@Name"        , [](){ return settings._apSsid ; } },
  { "@NtpManual"   , [](){ return ntp.isNtp() ? "NTP" : translate("manual") ; } },
  { "@NtpManual"   , [](){ return ntp.isNtp() ? "NTP" : translate("manual") ; } },
  { "@NtpTime"     , [](){ return ntp.toLocalString() ; } },
  { "@LastSync"    , [](){ return ntp.lastSyncToString() ; } },
  { "@Ntp"         , [](){ return settings._ntp ; } },
  { "@Ssid"        , [](){ return settings._ssid ; } },
  { "@ColHour"     , [](){ return settings._colHour.toString() ; } },
  { "@ColMinute"   , [](){ return settings._colMinute.toString() ; } },
  { "@ColSecond"   , [](){ return settings._colSecond.toString() ; } },
  { "@DstMonth"    , [](){ return inputMonth("tzDstMonth" , settings._tzDstMonth ) ; } },
  { "@DstWeek"     , [](){ return inputWeek ("tzDstWeek"  , settings._tzDstWeek  ) ; } },
  { "@DstDay"      , [](){ return inputDay  ("tzDstDay"   , settings._tzDstDay   ) ; } },
  { "@DstHour"     , [](){ return inputInt  ("tzDstHour"  , settings._tzDstHour  , (uint8_t)   0, (uint8_t) 23) ; } },
  { "@DstOffset"   , [](){ return inputInt  ("tzDstOffset", settings._tzDstOffset, (int16_t)-840, (int16_t)840) ; } },
  { "@StdMonth"    , [](){ return inputMonth("tzStdMonth" , settings._tzStdMonth ) ; } },
  { "@StdWeek"     , [](){ return inputWeek ("tzStdWeek"  , settings._tzStdWeek  ) ; } },
  { "@StdDay"      , [](){ return inputDay  ("tzStdDay"   , settings._tzStdDay   ) ; } },
  { "@StdHour"     , [](){ return inputInt  ("tzStdHour"  , settings._tzStdHour  , (uint8_t)   0, (uint8_t) 23) ; } },
  { "@StdOffset"   , [](){ return inputInt  ("tzStdOffset", settings._tzStdOffset, (int16_t)-840, (int16_t)840) ; } },
  { "@AutoOnOffEa" , [](){ return settings._autoOnOffEnable ? "checked" : "" ; } },
  { "@AutoOnOffOn" , [](){ return timeToString(settings._autoOnOffOn) ; } },
  { "@AutoOnOffOff", [](){ return timeToString(settings._autoOnOffOff) ; } },
  { "@Source"      , [](){ return String("https://github.com/MuellerA/Led-Clock") ; } },
} ;

String translate(const String &orig)
{
  auto iWord = TranslateWord.find(orig) ;
  if (iWord != TranslateWord.end())
    return iWord->second() ;

  auto iFunc = TranslateFunc.find(orig) ;
  if (iFunc != TranslateFunc.end())
    return iFunc->second() ;
  
  if (orig == "#LastSync")
  {
    String s ;
    switch ((unsigned int)settings._lang)
    {
    case 0:
      s += "Last synchronization (" ;
      s += ntp.isNtp() ? "NTP" : translate("manual") ;
      s += ") " ;
      s += ntp.lastSyncToString() ;
      s += " ago" ;
      return s ;
    case 1:
      s += "Letzte Synchronisation (" ;
      s += ntp.isNtp() ? "NTP" : translate("manual") ;
      s += ") vor " ;
      s += ntp.lastSyncToString() ;
      return s ;
    }
  }
  
  return orig ;
}

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
  R"(<!DOCTYPE html>
<html>
 <head>
  <meta charset="utf-8"/>
  <meta name="viewport" content="width=device-width, initial-scale=1.0"/>
  <title>%@Name% - %Page%</title>
  <link rel="stylesheet" href="/clock.css"/>
 </head>
 <body>
  <h1>%@Name% - %Page%</h1>)" ;

const char HttpFooter_P[] PROGMEM =
  R"( </body>
</html>)" ;

////////////////////////////////////////////////////////////////////////////////

const char HttpHome_P[] PROGMEM =
  R"(<form action="/set" method="post">
  <button class="button" type="submit" name="action" value="on" style="font-size: 300%%">%On%</button>
  <button class="button" type="submit" name="action" value="off" style="font-size: 300%%">%Off%</button>
</form>
<p><span class="border" style="font-size: 500%%">%@NtpTime%</span></p>
<p><span style="font-size: 66%%">%#LastSync%</span></p>
<p style="font-size: small"><a href="/settings.html">%Settings%</a> <a href="/reboot">%Reboot%</a></p>
)" ;

////////////////////////////////////////////////////////////////////////////////

const char HttpCss_P[] PROGMEM =
  R"(.border
{
  border-width: 1px;
  border-color: black;
  border-style: solid;
  padding: 5px;
  margin: 8px 0px;
}
.button
{
  padding: 1ex;
  margin: 0.4ex;
}
)" ;
  
////////////////////////////////////////////////////////////////////////////////

const char HttpReboot_P[] PROGMEM =
  R"(<!DOCTYPE html>
<html>
 <head>
  <meta charset="utf-8"/>
  <meta name="viewport" content="width=device-width, initial-scale=1.0"/>
  <meta http-equiv="refresh" content="10;URL=/"/>
  <title>%@Name% - %Page%</title>
 </head>
 <body>
  <h1>%@Name% - %Page%</h1>
  <p>%Reboot%...</p>
 </body>
</html>)" ;

////////////////////////////////////////////////////////////////////////////////

const char HttpSettings_P[] PROGMEM =
  R"(<p style="font-size: small"><a href="/">%Home%</a></p>

<!-- Language -->
<div class="border">
  <h2>Sprache / Language</h2>
  <form action="settings.html" method="post" autocomplete="off">
    <button type="submit" name="action" value="deutsch">Deutsch</button>
    <button type="submit" name="action" value="english">English</button>
  </form>
</div>

<!-- Name -->
<div class="border">
  <h2>Name</h2>
  <form action="settings.html" method="post">
    <input type="text" name="apSsid" size="32" value="%@Name%"/>
    <button type="submit" name="action" value="ap">%Save Name%</button>
  </form>
</div>

<!-- Switch On/Off -->
<div class="border">
  <h2>%Switch On/Off%</h2>
  <form action="settings.html" method="post">
    <button type="submit" name="action" value="on">%On%</button>
    <button type="submit" name="action" value="off">%Off%</button>
  </form>
  <hr/>
  <form action="settings.html" method="post">
    <table>
      <tr><td>%Automatic%</td><td><input type="checkbox" name="autoOnOffEa" value="enable" %@AutoOnOffEa%></td></tr>
      <tr><td>%On%</td><td><input type="time" step="1" name="autoOnOffOn" value="%@AutoOnOffOn%"></td></tr>
      <tr><td>%Off%</td><td><input type="time" step="1" name="autoOnOffOff" value="%@AutoOnOffOff%"></td></tr>
    </table>
    <button type="submit" name="action" value="auto">%Save On/Off%</button>
  </form>
</div>

<!-- Color -->
<div class="border">
  <h2>%Color%</h2>
  <form action="settings.html" method="post" autocomplete="off">
    <table>
      <tr><td>%Hour%</td><td><input type="color" name="colorHour" value="%@ColHour%"/></td></tr>
      <tr><td>%Minute%</td><td><input type="color" name="colorMinute" value="%@ColMinute%"/></td></tr>
      <tr><td>%Second%</td><td><input type="color" name="colorSecond" value="%@ColSecond%"/></td></tr>
    </table>
    <button type="submit" name="action" value="color">%Save Color%</button>
  </form>
</div>

<!-- WiFi -->
<div class="border">
  <h2>%WiFi%</h2>
  <form action="settings.html" method="post" autocomplete="off">
    <table>
      <tr><td>SSID</td><td><input type="text" name="ssid" size="32" value="%@Ssid%"/></td></tr>
      <tr><td>PSK</td><td><input type="password" name="psk" size="32" value=""/></td></tr>
    </table>
    <button type="submit" name="action" value="wifi">%Save WiFi%</button>
  </form>
</div>

<!-- Time NTP -->
<div class="border">
  <h2>%Time% (NTP)</h2>
  <form action="settings.html" method="post" autocomplete="off">
    <table>
      <tr><td>NTP</td><td><input type="text" name="ntp" size="32" value="%@Ntp%" placeholder="pool.ntp.org"/></td></tr>
    </table>
    <table>
      <tr><th>%Timezone%</th><th>%Month%</th><th>%Week%</th><th>%Day%</th><th>%Hour%</th><th>%Offset to UTC (min)%</th></tr>
      <tr><th>%Start% %Daylight Saving Time%</th><td>%@DstMonth%</td><td>%@DstWeek%</td><td>%@DstDay%</td><td>%@DstHour%</td><td>%@DstOffset%</td></tr>
      <tr><th>%Start% %Standard Time%</th><td>%@StdMonth%</td><td>%@StdWeek%</td><td>%@StdDay%</td><td>%@StdHour%</td><td>%@StdOffset%</td></tr>
    </table>
    <button type="submit" name="action" value="timezone">%Save NTP%</button>
  </form>
</div>

<!-- Time Manual -->
<div class="border">
  <h2>%Time% (%manual%)</h2>
  <form action="settings.html" method="post" autocomplete="off">
    <input type="time" name="time" step="1" value="%@NtpTime%"/>
    <button type="submit" name="action" value="time">%Save Time%</button>
  </form>
</div>

<!-- Update -->
<div class="border">
  <h2>%Update%</h2>
  <ul>
    <li><a href="%@Source%">Source Code</a></li>
    <li><a href="/update">%Update Firmware%</a></li>
  </ul>
</div>
)" ;

////////////////////////////////////////////////////////////////////////////////

void httpOk()
{
  httpServer.setContentLength(CONTENT_LENGTH_UNKNOWN);
  httpServer.sendHeader("Content-Type","text/html",true);
  httpServer.sendHeader("Cache-Control","no-cache");
  httpServer.send(200);
}

void httpOkCss()
{
  httpServer.setContentLength(CONTENT_LENGTH_UNKNOWN);
  httpServer.sendHeader("Content-Type","text/css",true);
  httpServer.sendHeader("Cache-Control","no-cache");
  httpServer.send(200);
}

void httpOnHome()
{
  std::function<String(const String&)> translateHome = [](const String& orig)
  { return (orig == "Page") ? translate("Home") : translate(orig) ; } ;
  
  httpOk() ;
  httpTemplate_P(HttpHeader_P, translateHome) ;
  httpTemplate_P(HttpHome_P  , translateHome) ;
  httpTemplate_P(HttpFooter_P, translateHome) ;
  httpServer.sendContent("") ;
}

void httpOnCss()
{
  httpOkCss() ;
  httpServer.sendContent_P(HttpCss_P) ;
  httpServer.sendContent("") ;
}

void httpOnOn()
{
  httpServer.sendHeader("location", "/") ;
  httpServer.send(302);
  settings._state = State::On ;
}

void httpOnOff()
{
  httpServer.sendHeader("location", "/") ;
  httpServer.send(302);
  settings._state = State::Off ;
}

void httpOnSet()
{
  httpServer.sendHeader("location", "/") ;
  httpServer.send(302);

  if (httpServer.hasArg("action"))
  {
    String action = httpServer.arg("action") ;

    if      (action == "on"    ) settings._state = State::On  ;
    else if (action == "off"   ) settings._state = State::Off ;
  }
}
void httpOnReboot()
{
  std::function<String(const String&)> translateReboot = [](const String& orig)
    { return (orig == "Page") ? translate("Reboot") : translate(orig) ; } ;

  httpOk() ;
  httpTemplate_P(HttpReboot_P, translateReboot) ;
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
    else if (action == "ap")
    {
      String ssid = httpServer.arg("apSsid") ;
      if ((3 <= ssid.length()) && (ssid.length() <= 32))
      {
        settings._apSsid = httpServer.arg("apSsid") ;
        httpUpdater.updateCredentials(settings._apSsid.c_str(), settings._apPsk.c_str()) ;
        settingsDirty = true ;
      }
    }
    else if (action == "on")
    {
      settings._state = State::On ;
    }
    else if (action == "off")
    {
      settings._state = State::Off ;
    }
    else if (action == "auto")
    {
      String sEa  = httpServer.arg("autoOnOffEa" ) ;
      String sOn  = httpServer.arg("autoOnOffOn" ) ;
      String sOff = httpServer.arg("autoOnOffOff") ;
      
      if (sOn.length() && sOff.length())
      {
        bool     ea  = sEa == "enable" ;
        uint32_t on  = stringToTime(sOn) ;
        uint32_t off = stringToTime(sOff) ;

        if ((ea  != settings._autoOnOffEnable) ||
            (on  != settings._autoOnOffOn    ) ||
            (off != settings._autoOnOffOff   ))
        {
          settings._autoOnOffEnable = ea  ;
          settings._autoOnOffOn     = on  ;
          settings._autoOnOffOff    = off ;
          settingsDirty = true ;
        }
      }
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
          ascIntToBin         (tzDstHour  , dstHour  , (uint8_t)0   , (uint8_t) 23) &&
          ascIntToBin         (tzDstOffset, dstOffset, (int16_t)-840, (int16_t)840) &&
          httpOnSettings_Month(tzStdMonth , stdMonth ) &&
          httpOnSettings_Week (tzStdWeek  , stdWeek  ) &&
          httpOnSettings_Day  (tzStdDay   , stdDay   ) &&
          ascIntToBin         (tzStdHour  , stdHour  , (uint8_t)0   , (uint8_t) 23) &&
          ascIntToBin         (tzStdOffset, stdOffset, (int16_t)-840, (int16_t)840))
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

  std::function<String(const String&)> translateSettings = [](const String& orig)
    { return (orig == "Page") ? translate("Settings") : translate(orig) ; } ;

  httpOk() ;
  httpTemplate_P(HttpHeader_P, translateSettings) ;
  httpTemplate_P(HttpSettings_P, translateSettings) ;
  httpTemplate_P(HttpFooter_P, translateSettings) ;
  httpServer.sendContent("") ;
}

////////////////////////////////////////////////////////////////////////////////
// EOF
////////////////////////////////////////////////////////////////////////////////
