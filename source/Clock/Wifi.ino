////////////////////////////////////////////////////////////////////////////////
// Wifi.ino
////////////////////////////////////////////////////////////////////////////////

bool     WifiConnected = false ;
uint32_t WifiConnectionLostMs ;

void WifiStationConnected(const WiFiEventStationModeConnected &connected)
{
  Serial.printf("station connected\n") ;

  WifiConnected = true ;
  WiFi.setAutoReconnect(true) ;
  WiFi.enableAP(false) ;
}
WiFiEventHandler wifiStationConnectedCb ;

void WifiStationGotIP(const WiFiEventStationModeGotIP &gotIp)
{
  Serial.printf("got ip\n") ;

  Serial.printf("Address: %d.%d.%d.%d\n", ((gotIp.ip  >>0) & 0xff), ((gotIp.ip  >>8) & 0xff), ((gotIp.ip  >>16) & 0xff), ((gotIp.ip  >>24) & 0xff)) ;
  Serial.printf("Mask   : %d.%d.%d.%d\n", ((gotIp.mask>>0) & 0xff), ((gotIp.mask>>8) & 0xff), ((gotIp.mask>>16) & 0xff), ((gotIp.mask>>24) & 0xff)) ;
  Serial.printf("Gateway: %d.%d.%d.%d\n", ((gotIp.gw  >>0) & 0xff), ((gotIp.gw  >>8) & 0xff), ((gotIp.gw  >>16) & 0xff), ((gotIp.gw  >>24) & 0xff)) ;
}
WiFiEventHandler wifiStationGotIPCb ;

void WifiStationDisconnected(const WiFiEventStationModeDisconnected &disconnected)
{
  Serial.printf("station disconnected:\n") ;
  Serial.printf("reason: %d\n", disconnected.reason) ;

  WifiConnected = false ;
  WifiConnectionLostMs = millis() ;

  if (disconnected.reason == WIFI_DISCONNECT_REASON_AUTH_FAIL)
  {
    WiFi.setAutoReconnect(false) ;
    WifiApStart() ;
  }
}
WiFiEventHandler wifiStationDisconnectedCb ;

void WifiStationStart()
{
  if (settings._ssid.length())
    WiFi.begin(settings._ssid.c_str(), settings._psk.c_str()) ;
}

void WifiApStart()
{
  WiFi.softAP(settings._apSsid.c_str(), settings._apPsk.c_str(), settings._apChan) ;
}

void WifiSetup()
{
  WiFi.mode(WIFI_OFF) ;
  WiFi.persistent(false) ;
  WiFi.setAutoConnect(false) ;
  WiFi.setAutoReconnect(false) ;

  wifiStationConnectedCb    = WiFi.onStationModeConnected(WifiStationConnected) ;
  wifiStationGotIPCb        = WiFi.onStationModeGotIP(WifiStationGotIP) ;
  wifiStationDisconnectedCb = WiFi.onStationModeDisconnected(WifiStationDisconnected) ;

  WifiApStart() ;
  WifiStationStart() ;
}

void WifiLoop()
{
  if (!(WiFi.getMode() & WIFI_AP) &&
      !WifiConnected &&
      ((millis() - WifiConnectionLostMs) > 5000)) // start AP 5sec after Station disconnect
  {
    WifiApStart() ;
  }
}

////////////////////////////////////////////////////////////////////////////////
// EOF
////////////////////////////////////////////////////////////////////////////////
