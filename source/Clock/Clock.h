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
  Color() ;
  Color(uint8_t r, uint8_t g, uint8_t b) ;
  
  void set(uint8_t r, uint8_t g, uint8_t b) ;
  uint32_t rgb() const ;
  void mix(const Color &c) ;
  bool operator!() const ;
  String toString() const ;
  
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
  
  // Wifi Access Point
  String _apSsid ;
  String _apPsk  ;
  int    _apChan ;

  // Wifi Station
  String _ssid ;
  String _psk ;

  // NTP
  String _ntp { "pool.ntp.org" } ;

  // Color  
  Color _colHour   { 0xff, 0x00, 0x00 } ; // rgb
  Color _colMinute { 0x00, 0xff, 0x00 } ; // rgb
  Color _colSecond { 0x00, 0x00, 0xff } ; // rgb
} ;

////////////////////////////////////////////////////////////////////////////////
// EOF
////////////////////////////////////////////////////////////////////////////////
