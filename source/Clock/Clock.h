////////////////////////////////////////////////////////////////////////////////
// Clock.h
////////////////////////////////////////////////////////////////////////////////

extern char HostName[] ;

////////////////////////////////////////////////////////////////////////////////

class HttpBuff ;

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

enum class Lang
{
  EN,
  DE,
  Size // last
} ;

struct Settings
{
  Settings() = default ;
  Settings(const Settings &) = delete ;
  Settings& operator=(const Settings &) = delete ;

  void load() ;
  void save() const ;

  // Language
  Lang _lang { Lang::DE } ;
  
  // Wifi Access Point
  String _apSsid ;
  String _apPsk  ;
  int    _apChan ;

  // Wifi Station
  String _ssid ;
  String _psk ;

  // NTP
  String    _ntp { "pool.ntp.org" } ;
  TZ::Month _tzDstMonth  { TZ::Month:: Mar } ;
  TZ::Week  _tzDstWeek   { TZ::Week:: Last } ;
  TZ::Day   _tzDstDay    { TZ::Day::   Sun } ;
  uint8_t   _tzDstHour   {               2 } ;
  int16_t   _tzDstOffset {             120 } ;
  TZ::Month _tzStdMonth  { TZ::Month:: Oct } ;
  TZ::Week  _tzStdWeek   { TZ::Week:: Last } ;
  TZ::Day   _tzStdDay    { TZ::Day::   Sun } ;
  uint8_t   _tzStdHour   {               2 } ;
  int16_t   _tzStdOffset {              60 } ;

  // Color  
  Color _colHour   { 0xff, 0x00, 0x00 } ; // rgb
  Color _colMinute { 0x00, 0xff, 0x00 } ; // rgb
  Color _colSecond { 0x00, 0x00, 0xff } ; // rgb
} ;

////////////////////////////////////////////////////////////////////////////////

class Ntp
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
    uint32_t originTsSec ;    // 24
    uint32_t originTsFrac ;
    uint32_t receiveTsSec ;   // 32
    uint32_t receiveTsFrac ;
    uint32_t transmitTsSec ;  // 40
    uint32_t transmitTsFrac ;
  } ;
#pragma pop

  enum class State
  {
    off     = 0,
    wait    = 1,
    success = 2,
    force   = 3,
  } ;

public:
  void     start() ; // enable transmitting ntp messages
  void     stop() ;  // disable transmitting ntp messages
  
  bool     valid()  { return _valid ; } // a time was set (manually or by NTP)
  bool     active() { return _isUtc ; } // time was received via NTP
  bool     inc() ; // time changed
  uint64_t local() ; // current local time
  void     setLocal(uint64_t local) ;
  
  void printSerial(const NtpData &data) const ;

  String toLocalString() ;
  bool   fromLocalString(const String &str) ;
  
  static uint16_t port() { return 123 ; }
  static uint16_t size() { return sizeof(NtpData) ; }
  bool tx(WiFiUDP &udp, const String &ntpServer) ;
  bool rx(WiFiUDP &udp) ;

private:
  bool fromLocalString1(const String &s, uint8_t offset, uint8_t &v) const ;

private:
  static const int _delayWait    =      10 * 1000 ; // 10 sec
  static const int _delaySuccess = 30 * 60 * 1000 ; // 30 min

  NtpData _txData ;
  NtpData _rxData ;

  State    _state   = State::off ;
  bool     _valid   = false ;
  bool     _next59  = false ;
  bool     _next61  = false ;
  uint32_t _nextInc     = 0 ; // next millis when inc current time
  uint32_t _lastRequest = 0 ; // millis of last request
  uint64_t _tsReceived  = 0 ; // last received ntp time
  uint64_t _tsTransmit  = 0 ; // last tx timestamp
  bool     _isUtc    = false ;
  uint64_t _current  = 0 ;    // current utc or local time
  
} ;

////////////////////////////////////////////////////////////////////////////////
// EOF
////////////////////////////////////////////////////////////////////////////////
