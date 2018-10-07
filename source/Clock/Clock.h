////////////////////////////////////////////////////////////////////////////////
// Clock.h
////////////////////////////////////////////////////////////////////////////////

class Time
{
public:
  Time() ;
  
  void set(uint8_t hour, uint8_t minute, uint8_t second) ;  
  void inc() ;
  String toString() ;
  bool fromString(const String &str) ;
  bool valid() const { return _valid ; }
  uint8_t hour  () const { return _hour   ; }
  uint8_t minute() const { return _minute ; }
  uint8_t second() const { return _second ; }

private:
  bool fromString1(const String &s, uint8_t offset, uint8_t &v) const ;
  
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

class Color
{
public:
  Color() ;
  Color(uint8_t r, uint8_t g, uint8_t b) ;
  
  void set(uint8_t r, uint8_t g, uint8_t b) ;
  uint32_t rgb() const ;
  void mix(const Color &c) ;
  bool operator!() const ;
  String toString() const ;
  bool fromString(const String &str) ;

private:
  bool fromString1(const String &s, uint8_t offset, uint8_t &v) const ;
  
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
  int    _tz  { 120 } ;

  // Color  
  Color _colHour   { 0xff, 0x00, 0x00 } ; // rgb
  Color _colMinute { 0x00, 0xff, 0x00 } ; // rgb
  Color _colSecond { 0x00, 0x00, 0xff } ; // rgb
} ;

////////////////////////////////////////////////////////////////////////////////

struct Ntp
{
#pragma pack(push, 1)
  struct NtpData
  {
    uint8_t liVnMode ;        // 0 [2:3:3]
    uint8_t stratum ;
    uint8_t poll ;
    uint8_t precision ;
    uint32_t rootDelay ;      // 4 
    uint32_t rootDispersion ;
    uint32_t referenceId ;
    uint32_t referenceTsSec ; // 16
    uint32_t referenceTsFrac ;
    uint32_t originateTsSec ; // 24
    uint32_t originateTsFrac ;
    uint32_t receiveTsSec ;   // 32
    uint32_t receiveTsFrac ;
    uint32_t transmitTsSec ;  // 40
    uint32_t transmitTsFrac ;
  } ;
#pragma pop

  NtpData _txData ;
  NtpData _rxData ;

  bool tx(WiFiUDP &udp, const String &ntpServer) ;
  bool rx(WiFiUDP &udp) ;

  void printSerial(const NtpData &data) const ;

  uint32_t _delay  = 20 ;
  
  bool     _next59 = false ;
  bool     _next61 = false ;
  uint64_t _ts     = 0 ; // last received ntp time
  uint32_t _millis = 0 ; // system time when received
  
} ;

////////////////////////////////////////////////////////////////////////////////
// EOF
////////////////////////////////////////////////////////////////////////////////
