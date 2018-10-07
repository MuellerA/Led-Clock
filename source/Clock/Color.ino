////////////////////////////////////////////////////////////////////////////////
// Color.ino
////////////////////////////////////////////////////////////////////////////////

Color::Color() : _r{0}, _g{0}, _b{0}
{
}

Color::Color(uint8_t r, uint8_t g, uint8_t b) : _r{r}, _g{g}, _b{b}
{
}

void Color::set(uint8_t r, uint8_t g, uint8_t b)
{
  _r = r ;
  _g = g ;
  _b = b ;
}

uint32_t Color::rgb() const
{
  return ((uint32_t)_r << 16) | ((uint32_t)_g << 8) | ((uint32_t)_b << 0) ;
}

void Color::mix(const Color &c)
{
  uint16_t sum ;

  sum = (uint16_t)_r + (uint16_t)c._r ; _r = (sum > 0xff) ? 0xff : sum ;
  sum = (uint16_t)_g + (uint16_t)c._g ; _g = (sum > 0xff) ? 0xff : sum ;
  sum = (uint16_t)_b + (uint16_t)c._b ; _b = (sum > 0xff) ? 0xff : sum ;
}

String Color::toString() const
{
  char buff[16] ;
  sprintf(buff, "#%02x%02x%02x", _r, _g, _b) ;
  return String(buff) ;
}

bool Color::fromString1(const String &s, uint8_t offset, uint8_t &v) const
{
  uint8_t hi, lo ;
  if (ascHex2bin(s[offset+0], hi) && ascHex2bin(s[offset+1], lo))
  {
    v = 16 * hi + lo ;
    return true ;
  }
  return false ;
}

bool Color::fromString(const String &str)
{
  uint8_t r, g, b ;
  
  if ((str.length() == 7) &&
      (str[0] == '#') &&
      fromString1(str, 1, r) &&
      fromString1(str, 3, g) &&
      fromString1(str, 5, b))
  {
    set(r, g, b) ;
    return true ;
  }
  return false ;
}

bool Color::operator!() const
{
  return !_r && !_g && !_b ;
}

////////////////////////////////////////////////////////////////////////////////
// EOF
////////////////////////////////////////////////////////////////////////////////