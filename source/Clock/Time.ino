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

////////////////////////////////////////////////////////////////////////////////
// EOF
////////////////////////////////////////////////////////////////////////////////
