////////////////////////////////////////////////////////////////////////////////
// Clock.h
////////////////////////////////////////////////////////////////////////////////

struct Time
{
  Time() ;
  
  void set(uint8_t hour, uint8_t minute, uint8_t second) ;  
  void inc() ;
  String toString() ;

  bool    _valid ;
  uint8_t _hour ;
  uint8_t _minute ;
  uint8_t _second ;
  bool _next59 ; // last minute of day has 59 seconds
  bool _next61 ; // last minute of day has 61 seconds
} ;

////////////////////////////////////////////////////////////////////////////////

struct Brightness
{
  Brightness(unsigned int pin) ;
  
  void init() ;
  unsigned long update() ;

  unsigned long _pin ;
  unsigned long _val[16] ;
  unsigned long _sum ;
  unsigned long _idx ;
} ;

////////////////////////////////////////////////////////////////////////////////

struct Color
{
  uint32_t rgb() ;
  void mix(const Color &c) ;
  
  uint8_t _r ;
  uint8_t _g ;
  uint8_t _b ;
} ;

////////////////////////////////////////////////////////////////////////////////

struct Settings
{
  Settings() = default ;
  Settings(const Settings &) = delete ;
  Settings& operator=(const Settings &) = delete ;

  void load() ;
  void save() const ;
  
  // Wifi
  String _apSsid ;
  String _apPsk  ;
  int    _apChan ;

  // NTP
  String _ssid ;
  String _psk ;
  String _server { "pool.ntp.org" } ;

  // Color  
  Color _colHour   { 0xff, 0x00, 0x00 } ; // rgb
  Color _colMinute { 0x00, 0xff, 0x00 } ; // rgb
  Color _colSecond { 0x00, 0x00, 0xff } ; // rgb
} ;

////////////////////////////////////////////////////////////////////////////////
// EOF
////////////////////////////////////////////////////////////////////////////////
