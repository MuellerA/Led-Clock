////////////////////////////////////////////////////////////////////////////////
// Settings
////////////////////////////////////////////////////////////////////////////////

#define SettingsMagic    "Clock Settings V0.1"
#define SettingsFileName "/clock.settings"

String fileRead(File &cfg)
{
  String str = cfg.readStringUntil('\n') ;
  str.trim() ;
  return str ;
}

void Settings::load()
{
  Serial.println("Settings::load") ;

  _state = State::On ;
  _apSsid = "Clock" ;
  _apPsk  = HostName ;
  _apChan = 8 ;

  String str ;
  File cfg = SPIFFS.open(SettingsFileName, "r") ;

  if (!cfg)
    return ;

  str = fileRead(cfg) ;
 
  if (str == SettingsMagic)
  {
    _ssid = fileRead(cfg) ;
    _psk  = fileRead(cfg) ;
    _ntp  = fileRead(cfg) ;

    _colHour  .fromString(fileRead(cfg)) ;
    _colMinute.fromString(fileRead(cfg)) ;
    _colSecond.fromString(fileRead(cfg)) ;

    uint8_t tmp ;
    ascInt2bin(fileRead(cfg), tmp         ) ; _tzDstMonth = (TZ::Month)tmp ;
    ascInt2bin(fileRead(cfg), tmp         ) ; _tzDstWeek  = (TZ::Week) tmp ;
    ascInt2bin(fileRead(cfg), tmp         ) ; _tzDstDay   = (TZ::Day)  tmp ;
    ascInt2bin(fileRead(cfg), _tzDstHour  ) ;
    ascInt2bin(fileRead(cfg), _tzDstOffset) ;
    ascInt2bin(fileRead(cfg), tmp         ) ; _tzStdMonth = (TZ::Month)tmp ;
    ascInt2bin(fileRead(cfg), tmp         ) ; _tzStdWeek  = (TZ::Week) tmp ;
    ascInt2bin(fileRead(cfg), tmp         ) ; _tzStdDay   = (TZ::Day)  tmp ;
    ascInt2bin(fileRead(cfg), _tzStdHour  ) ;
    ascInt2bin(fileRead(cfg), _tzStdOffset) ;

    ascInt2bin(fileRead(cfg), tmp) ; _lang = (Lang)tmp ;
   
    ascInt2bin(fileRead(cfg), tmp) ; _autoOnOffEnable = tmp != 0 ;
    ascInt2bin(fileRead(cfg), _autoOnOffOn ) ;
    ascInt2bin(fileRead(cfg), _autoOnOffOff) ;

    // magic aendern!

    tz.resetRules() ;
    tz.addRule( { _tzDstMonth, _tzDstWeek, _tzDstDay, _tzDstHour, _tzDstOffset } ) ;
    tz.addRule( { _tzStdMonth, _tzStdWeek, _tzStdDay, _tzStdHour, _tzStdOffset } ) ;
  }

  if (_ntp.length())
    ntp.start() ;
  else
    ntp.stop() ;
 
  cfg.close() ;
}

void Settings::save() const
{
  Serial.println("Settings::save") ;

  File cfg = SPIFFS.open(SettingsFileName, "w") ;

  if (!cfg)
    return ;

  cfg.println(SettingsMagic) ;
  
  cfg.println(_ssid) ;
  cfg.println(_psk) ;
  cfg.println(_ntp) ;
  
  cfg.println(_colHour  .toString()) ;
  cfg.println(_colMinute.toString()) ;
  cfg.println(_colSecond.toString()) ;

  cfg.println((uint8_t)_tzDstMonth ) ;
  cfg.println((uint8_t)_tzDstWeek  ) ;
  cfg.println((uint8_t)_tzDstDay   ) ;
  cfg.println(         _tzDstHour  ) ;
  cfg.println(         _tzDstOffset) ;
  cfg.println((uint8_t)_tzStdMonth ) ;
  cfg.println((uint8_t)_tzStdWeek  ) ;
  cfg.println((uint8_t)_tzStdDay   ) ;
  cfg.println(         _tzStdHour  ) ;
  cfg.println(         _tzStdOffset) ;

  cfg.println((uint8_t)_lang) ;

  cfg.println(_autoOnOffEnable ? 1 : 0) ;
  cfg.println(_autoOnOffOn ) ;
  cfg.println(_autoOnOffOff) ;

  // magic aendern!
  
  cfg.close() ;
}

////////////////////////////////////////////////////////////////////////////////
// EOF
////////////////////////////////////////////////////////////////////////////////
