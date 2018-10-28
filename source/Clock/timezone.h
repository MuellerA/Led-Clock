////////////////////////////////////////////////////////////////////////////////
// timezone.h
////////////////////////////////////////////////////////////////////////////////
//
//  valid range: 2001-01-01 - 2100-02-28 utc
//
////////////////////////////////////////////////////////////////////////////////

#include <cstdint>

namespace TZ
{
  enum class Month { Jan, Feb, Mar, Apr, May, Jun, Jul, Aug, Sep, Oct, Nov, Dec } ;
  enum class Week  { First, Second, Third, Fourth, Last } ;
  enum class Day   { Sun, Mon, Tue, Wed, Thu, Fri, Sat } ;

  uint64_t ntpToUtc(uint64_t ntp) ; // ntp seconds including era
  uint64_t utcToNtp(uint64_t utc) ; // utc seconds
  bool     isLeapYear(uint64_t utc) ; // utc seconds

  class Rule
  {
  public:
    Rule() ;
    Rule(Month month, Week week, Day day, uint8_t hour, int16_t minOffset) ;

    bool operator<=(uint64_t utc) const ;
    int16_t secOffset() const ;
    
  private:
    uint32_t startSecInYear(uint64_t utc) const ;

    Week    _week ;
    Day     _day ;
    Month   _month ;
    uint8_t _hour ;
    int16_t _secHourOffset ; // start hour offset to utc in seconds
    int16_t _secOffset ;     // time zone offset to utc in seconds

    mutable uint32_t _startSecInYear28 ; // cached value
    mutable uint8_t  _year28 ;           // year in 28 year cycle
  } ;
  
  class TimeZone
  {
  public:
    TimeZone() ;
    TimeZone(const Rule &r1) ;
    TimeZone(const Rule &r1, const Rule &r2) ;
    TimeZone(const Rule &r1, const Rule &r2, const Rule &r3) ;
    TimeZone(const Rule &r1, const Rule &r2, const Rule &r3, const Rule &r4) ;
    
    uint64_t utcToLoc(uint64_t utc) ;
    uint64_t locToUtc(uint64_t loc) ;
    uint64_t ntpToLoc(uint64_t ntp) ;

    void resetRules() ;
    void addRule(const Rule &rule) ;

  private:
    uint8_t _nRules ;
    Rule    _rules[4] ;
  } ;
  
}

////////////////////////////////////////////////////////////////////////////////
// EOF
////////////////////////////////////////////////////////////////////////////////
