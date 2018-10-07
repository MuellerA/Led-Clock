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

   // magic aendern!
 }

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

  // magic aendern!
  
  cfg.close() ;
}

////////////////////////////////////////////////////////////////////////////////
// EOF
////////////////////////////////////////////////////////////////////////////////
