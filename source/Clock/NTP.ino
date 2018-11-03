////////////////////////////////////////////////////////////////////////////////
// NTP.ino
////////////////////////////////////////////////////////////////////////////////

void Ntp::start()
{
  _state       = State::force ;
  _next59      = false ;
  _next61      = false ;
  _lastRequest = 0 ;
  _tsReceived  = 0 ;
}

void Ntp::stop()
{
  _state = State::off ;
}

////////////////////////////////////////////////////////////////////////////////

bool Ntp::tx(WiFiUDP &udp, const String &ntpServer)
{
  unsigned long now = millis() ;
  
  switch (_state)
  {
  case State::off:
    return true ;
  case State::wait:
    if ((now - _lastRequest) < _delayWait)
      return true ;
    break ;
  case State::success:
    if ((now - _lastRequest) < _delaySuccess)
      return true ;
    break ;
  case State::force:
    break ;
  default:
    return false ;
  }

  Serial.println("Ntp::tx") ;
  memset(&_txData, 0, sizeof(_txData)) ;

  _txData.liVnMode  = 0b00100011 ;

  if (_tsReceived)
  {
    _tsTransmit  = _tsReceived ;
    _tsTransmit += ((uint64_t)now - (uint64_t)_lastRequest) * 4310345ULL ;

    _txData.transmitTsSec  = htonl(_tsTransmit >> 32) ;
    _txData.transmitTsFrac = htonl(_tsTransmit >>  0) ;
  }
  _lastRequest = now ;
  _state = State::wait ;
  
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

////////////////////////////////////////////////////////////////////////////////

bool Ntp::rx(WiFiUDP &udp)
{
  uint32_t now = millis() ;

  Serial.println("Ntp::rx") ;
  if (!udp.read((uint8_t*)&_rxData, sizeof(_rxData)))
  {
    Serial.println("read() failed") ;
    return false ;
  }
  //printSerial(_rxData) ;

  _state  = State::success ;
  _next59 = _rxData.liVnMode & 0x80 ;
  _next61 = _rxData.liVnMode & 0x40 ;
  
  uint64_t origin = ((uint64_t)ntohl(_rxData.originTsSec) << 32) + ((uint64_t)ntohl(_rxData.originTsFrac) << 0) ;
  if (origin != _tsTransmit)
    return false ;
  
  //uint64_t receive   = ((uint64_t)ntohl(_rxData.receiveTsSec  ) << 32) + ((uint64_t)ntohl(_rxData.receiveTsFrac  ) << 0) ;
  uint64_t transmit  = ((uint64_t)ntohl(_rxData.transmitTsSec ) << 32) + ((uint64_t)ntohl(_rxData.transmitTsFrac ) << 0) ;

  // keep it simple
  _tsReceived = transmit ;
  uint64_t utc = (_tsReceived >> 32) ; // ntp pico second to utc second conversion
  if (utc < 0x80000000) // assume ntp era 1
    utc += 0x100000000ULL ;
  utc -= 2208988800UL ;
  _current = utc ;
  _isUtc = true ;
  _valid = true ;
  _lastInc = now ; // todo: take transmitTsFrac into account
  
  return true ;
}

////////////////////////////////////////////////////////////////////////////////

bool Ntp::inc()
{
  if (!_valid)
    return false ;

    uint32_t now = millis() ;
    if (now - _lastInc < 1000)
      return false ;

    if (_isUtc)
    {
      while (now - _lastInc >= 1000)
      {  
        uint64_t secondsOfDay = _current % (24 * 60 * 60) ;
        if      ((secondsOfDay == (24 * 60 * 60 - 2)) && _next59) { _current += 2 ; _next59 = false ; }
        else if ((secondsOfDay == (24 * 60 * 60 + 0)) && _next61) { _current += 0 ; _next61 = false ; }
        else                                                      { _current += 1 ;                   }
        _lastInc += 1000 ;
      }
      return true ;
    }
    else
    {
      while (now - _lastInc >= 1000)
      {
        _current += 1 ;
        _lastInc += 1000 ;
      }
      return true ;
    }
}

uint64_t Ntp::local()
{
  return _isUtc ? tz.utcToLoc(_current) : _current ;
}

void Ntp::setLocal(uint64_t local)
{
  uint32_t now = millis() ;
  _current    = local ;
  _isUtc      = false ;
  _valid      = true ;
  _lastInc    = now ;
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
  Serial.printf("Origin    %08x.%08x\n", ntohl(data.originTsSec   ), ntohl(data.originTsFrac   )) ;
  Serial.printf("Receive   %08x.%08x\n", ntohl(data.receiveTsSec  ), ntohl(data.receiveTsFrac  )) ;
  Serial.printf("Transmit  %08x.%08x\n", ntohl(data.transmitTsSec ), ntohl(data.transmitTsFrac )) ;
}

////////////////////////////////////////////////////////////////////////////////

String Ntp::toLocalString()
{
  uint64_t t = local() ;
  
  char buff[16] ;
  sprintf(buff, "%02ld:%02ld:%02ld", (long)(t / 60 / 60 % 24), (long)(t / 60 % 60), (long)(t % 60)) ;
  return String(buff) ;
}

bool Ntp::fromLocalString1(const String &s, uint8_t offset, uint8_t &v) const
{
  uint8_t hi, lo ;

  if (ascDec2bin(s[offset+0], hi) && ascDec2bin(s[offset+1], lo))
  {
    v = 10 * hi + lo ;
    return true ;
  }
  return false ;
}

bool Ntp::fromLocalString(const String &str)
{
  uint8_t h, m, s ;

  if ((str.length() == 8) &&
      (str[2] == ':') &&
      (str[5] == ':') &&
      fromLocalString1(str, 0, h) && (h <= 23) &&
      fromLocalString1(str, 3, m) && (m <= 59) &&
      fromLocalString1(str, 6, s) && (s <= 59))
  {
    setLocal(h*60*60 + m*60 + s) ;
    return true ;
  }
  return false ;
}

////////////////////////////////////////////////////////////////////////////////
// EOF
////////////////////////////////////////////////////////////////////////////////
