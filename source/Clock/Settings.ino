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

  char buff[16] ;
  sprintf(buff, "%06x", ESP.getChipId()) ;
 _apSsid = "Clock" ;
 _apPsk  = "Clock" ;
 _apPsk += buff ;
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

   ascInt2bin(fileRead(cfg), _tzDstMonth      ) ;
   ascInt2bin(fileRead(cfg), _tzDstWeekOfMonth) ;
   ascInt2bin(fileRead(cfg), _tzDstDayOfWeek  ) ;
   ascInt2bin(fileRead(cfg), _tzDstHour       ) ;
   ascInt2bin(fileRead(cfg), _tzDstOffset     ) ;
   ascInt2bin(fileRead(cfg), _tzStdMonth      ) ;
   ascInt2bin(fileRead(cfg), _tzStdWeekOfMonth) ;
   ascInt2bin(fileRead(cfg), _tzStdDayOfWeek  ) ;
   ascInt2bin(fileRead(cfg), _tzStdHour       ) ;
   ascInt2bin(fileRead(cfg), _tzStdOffset     ) ;

   // magic aendern!
   
   TimeChangeRule dst = { "dst", _tzDstWeekOfMonth, _tzDstDayOfWeek, _tzDstMonth, _tzDstHour, _tzDstOffset } ;
   TimeChangeRule std = { "std", _tzStdWeekOfMonth, _tzStdDayOfWeek, _tzStdMonth, _tzStdHour, _tzStdOffset } ;
   tz = Timezone(dst, std) ;
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

  cfg.println(_tzDstMonth      ) ;
  cfg.println(_tzDstWeekOfMonth) ;
  cfg.println(_tzDstDayOfWeek  ) ;
  cfg.println(_tzDstHour       ) ;
  cfg.println(_tzDstOffset     ) ;
  cfg.println(_tzStdMonth      ) ;
  cfg.println(_tzStdWeekOfMonth) ;
  cfg.println(_tzStdDayOfWeek  ) ;
  cfg.println(_tzStdHour       ) ;
  cfg.println(_tzStdOffset     ) ;

  // magic aendern!
  
  cfg.close() ;
}

////////////////////////////////////////////////////////////////////////////////
// EOF
////////////////////////////////////////////////////////////////////////////////
