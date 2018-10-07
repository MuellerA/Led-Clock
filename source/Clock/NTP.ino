////////////////////////////////////////////////////////////////////////////////
// NTP.ino
////////////////////////////////////////////////////////////////////////////////

bool Ntp::tx(WiFiUDP &udp, const String &ntpServer)
{
  _delay = 10 ;
  Serial.println("Ntp::tx") ;
  Serial.println(_millis) ;
  memset(&_txData, 0, sizeof(_txData)) ;

  if (_millis)
  {
    uint64_t ts ;
    
    ts  = _ts ;
    ts += ((uint64_t)millis() - (uint64_t)_millis) * 4310345ULL ;

    _txData.liVnMode  = 0b00100011 ;
    _txData.originateTsSec  = htonl(ts >> 32) ;
    _txData.originateTsFrac = htonl(ts >>  0) ;
  }
  else
  {
    _txData.liVnMode  = 0b11100011 ;
  }
  
  //printSerial(_txData) ;

  if (!udp.beginPacket(ntpServer.c_str(), 123))
  {
    Serial.println("beginPacket() failed") ;
    return false ;
  }
  if (!udp.write((uint8_t*)&_txData, sizeof(_txData)))
  {
    Serial.println("write() failed") ;
    return false ;
  }
  if (!udp.endPacket())
  {
    Serial.println("endPacket() failed") ;
    return false ;
  }
  return true ;
}

bool Ntp::rx(WiFiUDP &udp)
{
  uint32_t m = millis() ;

  Serial.println("Ntp::rx") ;
  if (!udp.read((uint8_t*)&_rxData, sizeof(_rxData)))
  {
    Serial.println("read() failed") ;
    return false ;
  }
  //printSerial(_rxData) ;

  _millis = m ;
  _delay = 60 * 60 ;
  _next59 = _rxData.liVnMode & 0x80 ;
  _next61 = _rxData.liVnMode & 0x40 ;
  
  //uint64_t originate = ((uint64_t)ntohl(_rxData.originateTsSec) << 32) + ((uint64_t)ntohl(_rxData.originateTsFrac) << 0) ;
  //uint64_t receive   = ((uint64_t)ntohl(_rxData.receiveTsSec  ) << 32) + ((uint64_t)ntohl(_rxData.receiveTsFrac  ) << 0) ;
  uint64_t transmit  = ((uint64_t)ntohl(_rxData.transmitTsSec ) << 32) + ((uint64_t)ntohl(_rxData.transmitTsFrac ) << 0) ;

  // keep it simple
  _ts = transmit ;

  return true ;
}

void Ntp::printSerial(const Ntp::NtpData &data) const
{
  Serial.printf("Li Vn Mode %02x\n", data.liVnMode) ;
  Serial.printf("Startum    %02x\n", data.stratum) ;
  Serial.printf("Poll       %02x\n", data.poll) ;
  Serial.printf("Precision  %02x\n", data.precision) ;
  Serial.printf("Root Delay      %08x\n", ntohl(data.rootDelay)) ;
  Serial.printf("Root Dispersion %08x\n", ntohl(data.rootDispersion)) ;
  Serial.printf("Reference Id    %08x\n", ntohl(data.referenceId)) ;
  Serial.printf("Reference %08x.%08x\n", ntohl(data.referenceTsSec), ntohl(data.referenceTsFrac)) ;
  Serial.printf("Originate %08x.%08x\n", ntohl(data.originateTsSec), ntohl(data.originateTsFrac)) ;
  Serial.printf("Receive   %08x.%08x\n", ntohl(data.receiveTsSec  ), ntohl(data.receiveTsFrac  )) ;
  Serial.printf("Transmit  %08x.%08x\n", ntohl(data.transmitTsSec ), ntohl(data.transmitTsFrac )) ;
}

////////////////////////////////////////////////////////////////////////////////
// EOF
////////////////////////////////////////////////////////////////////////////////
