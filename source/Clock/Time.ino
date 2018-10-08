////////////////////////////////////////////////////////////////////////////////
// Time.ino
////////////////////////////////////////////////////////////////////////////////

Time::Time()
{
  _valid  = false ;
  _hour   = 0 ;
  _minute = 0 ;
  _second = 0 ;
  _next59 = false ;
  _next61 = false ;
}

void Time::set(uint8_t hour, uint8_t minute, uint8_t second)
{
  _valid  = true ;
  _hour   = hour ;
  _minute = minute ;
  _second = second ;
}
  
void Time::inc()
{
  _second += 1 ;

  if ((_second == 59) &&   _next59 && (_minute == 59) && (_hour == 23)  ||
      (_second == 60) && !(_next61 && (_minute == 59) && (_hour == 23)) ||
      (_second == 61))
  {
    _second = 0 ;
    _minute += 1 ;
    if (_minute == 60)
    {
      _minute = 0 ;
      _hour += 1 ;
      if (_hour == 24)
      {
        _hour = 0 ;
      }
    }
  }
}

String Time::toString()
{
  char buff[16] ;
  sprintf(buff, "%02d:%02d:%02d", t._hour, t._minute, t._second) ;
  return String(buff) ;
}

bool Time::fromString1(const String &s, uint8_t offset, uint8_t &v) const
{
  uint8_t hi, lo ;

  if (ascDec2bin(s[offset+0], hi) && ascDec2bin(s[offset+1], lo))
  {
    v = 10 * hi + lo ;
    return true ;
  }
  return false ;
}

bool Time::fromString(const String &str)
{
  uint8_t h, m, s ;

  if ((str.length() == 8) &&
      (str[2] == ':') &&
      (str[5] == ':') &&
      fromString1(str, 0, h) && (h <= 23) &&
      fromString1(str, 3, m) && (m <= 59) &&
      fromString1(str, 6, s) && (s <= 59))
  {
    set(h, m, s) ;
    return true ;
  }
  return false ;
}

////////////////////////////////////////////////////////////////////////////////
// EOF
////////////////////////////////////////////////////////////////////////////////
