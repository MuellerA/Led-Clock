////////////////////////////////////////////////////////////////////////////////
// timezone.cpp
////////////////////////////////////////////////////////////////////////////////
//
//  valid range: 2001-01-01 - 2100-02-28 utc
//
// all calculations are based on 2001-01-01
//
////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include "timezone.h"

namespace TZ
{
  static const uint64_t SecPerDay = 24 * 60 * 60 ;
  static const uint64_t NTP2001 = 36890 * SecPerDay ;
  static const uint64_t UTC2001 = 11323 * SecPerDay ;

  static const uint32_t SecPerYear     = (365        ) * SecPerDay ;
  static const uint32_t SecPerYearLeap = (366        ) * SecPerDay ;
  static const uint32_t SecPer4Year    = (3*365 + 366) * SecPerDay ;
  static const uint32_t SecPer3Year    = (3*365      ) * SecPerDay ;
  static const uint32_t SecPer28Year   = SecPer4Year * 7 ;
  
  static const Day FirstDayOfMonth[7*4][13] = // [ year-in-28-year-cycle-starting-from-2001 ] [ month ]
    {
      { Day::Mon, Day::Thu, Day::Thu, Day::Sun, Day::Tue, Day::Fri,             /* 0 */
        Day::Sun, Day::Wed, Day::Sat, Day::Mon, Day::Thu, Day::Sat, Day::Tue },
      { Day::Tue, Day::Fri, Day::Fri, Day::Mon, Day::Wed, Day::Sat,             /* 1 */
        Day::Mon, Day::Thu, Day::Sun, Day::Tue, Day::Fri, Day::Sun, Day::Wed },
      { Day::Wed, Day::Sat, Day::Sat, Day::Tue, Day::Thu, Day::Sun,             /* 2 */
        Day::Tue, Day::Fri, Day::Mon, Day::Wed, Day::Sat, Day::Mon, Day::Thu },
      { Day::Thu, Day::Sun, Day::Mon, Day::Thu, Day::Sat, Day::Tue,             /* 3 */
        Day::Thu, Day::Sun, Day::Wed, Day::Fri, Day::Mon, Day::Wed, Day::Sat },
      { Day::Sat, Day::Tue, Day::Tue, Day::Fri, Day::Sun, Day::Wed,             /* 4 */
        Day::Fri, Day::Mon, Day::Thu, Day::Sat, Day::Tue, Day::Thu, Day::Sun },
      { Day::Sun, Day::Wed, Day::Wed, Day::Sat, Day::Mon, Day::Thu,             /* 5 */
        Day::Sat, Day::Tue, Day::Fri, Day::Sun, Day::Wed, Day::Fri, Day::Mon },
      { Day::Mon, Day::Thu, Day::Thu, Day::Sun, Day::Tue, Day::Fri,             /* 6 */
        Day::Sun, Day::Wed, Day::Sat, Day::Mon, Day::Thu, Day::Sat, Day::Tue },
      { Day::Tue, Day::Fri, Day::Sat, Day::Tue, Day::Thu, Day::Sun,             /* 7 */
        Day::Tue, Day::Fri, Day::Mon, Day::Wed, Day::Sat, Day::Mon, Day::Thu },
      { Day::Thu, Day::Sun, Day::Sun, Day::Wed, Day::Fri, Day::Mon,             /* 8 */
        Day::Wed, Day::Sat, Day::Tue, Day::Thu, Day::Sun, Day::Tue, Day::Fri },
      { Day::Fri, Day::Mon, Day::Mon, Day::Thu, Day::Sat, Day::Tue,             /* 9 */
        Day::Thu, Day::Sun, Day::Wed, Day::Fri, Day::Mon, Day::Wed, Day::Sat },
      { Day::Sat, Day::Tue, Day::Tue, Day::Fri, Day::Sun, Day::Wed,             /* 10 */
        Day::Fri, Day::Mon, Day::Thu, Day::Sat, Day::Tue, Day::Thu, Day::Sun },
      { Day::Sun, Day::Wed, Day::Thu, Day::Sun, Day::Tue, Day::Fri,             /* 11 */
        Day::Sun, Day::Wed, Day::Sat, Day::Mon, Day::Thu, Day::Sat, Day::Tue },
      { Day::Tue, Day::Fri, Day::Fri, Day::Mon, Day::Wed, Day::Sat,             /* 12 */
        Day::Mon, Day::Thu, Day::Sun, Day::Tue, Day::Fri, Day::Sun, Day::Wed },
      { Day::Wed, Day::Sat, Day::Sat, Day::Tue, Day::Thu, Day::Sun,             /* 13 */
        Day::Tue, Day::Fri, Day::Mon, Day::Wed, Day::Sat, Day::Mon, Day::Thu },
      { Day::Thu, Day::Sun, Day::Sun, Day::Wed, Day::Fri, Day::Mon,             /* 14 */
        Day::Wed, Day::Sat, Day::Tue, Day::Thu, Day::Sun, Day::Tue, Day::Fri },
      { Day::Fri, Day::Mon, Day::Tue, Day::Fri, Day::Sun, Day::Wed,             /* 15 */
        Day::Fri, Day::Mon, Day::Thu, Day::Sat, Day::Tue, Day::Thu, Day::Sun },
      { Day::Sun, Day::Wed, Day::Wed, Day::Sat, Day::Mon, Day::Thu,             /* 16 */
        Day::Sat, Day::Tue, Day::Fri, Day::Sun, Day::Wed, Day::Fri, Day::Mon },
      { Day::Mon, Day::Thu, Day::Thu, Day::Sun, Day::Tue, Day::Fri,             /* 17 */
        Day::Sun, Day::Wed, Day::Sat, Day::Mon, Day::Thu, Day::Sat, Day::Tue },
      { Day::Tue, Day::Fri, Day::Fri, Day::Mon, Day::Wed, Day::Sat,             /* 18 */
        Day::Mon, Day::Thu, Day::Sun, Day::Tue, Day::Fri, Day::Sun, Day::Wed },
      { Day::Wed, Day::Sat, Day::Sun, Day::Wed, Day::Fri, Day::Mon,             /* 19 */
        Day::Wed, Day::Sat, Day::Tue, Day::Thu, Day::Sun, Day::Tue, Day::Fri },
      { Day::Fri, Day::Mon, Day::Mon, Day::Thu, Day::Sat, Day::Tue,             /* 20 */
        Day::Thu, Day::Sun, Day::Wed, Day::Fri, Day::Mon, Day::Wed, Day::Sat },
      { Day::Sat, Day::Tue, Day::Tue, Day::Fri, Day::Sun, Day::Wed,             /* 21 */
        Day::Fri, Day::Mon, Day::Thu, Day::Sat, Day::Tue, Day::Thu, Day::Sun },
      { Day::Sun, Day::Wed, Day::Wed, Day::Sat, Day::Mon, Day::Thu,             /* 22 */
        Day::Sat, Day::Tue, Day::Fri, Day::Sun, Day::Wed, Day::Fri, Day::Mon },
      { Day::Mon, Day::Thu, Day::Fri, Day::Mon, Day::Wed, Day::Sat,             /* 23 */
        Day::Mon, Day::Thu, Day::Sun, Day::Tue, Day::Fri, Day::Sun, Day::Wed },
      { Day::Wed, Day::Sat, Day::Sat, Day::Tue, Day::Thu, Day::Sun,             /* 24 */
        Day::Tue, Day::Fri, Day::Mon, Day::Wed, Day::Sat, Day::Mon, Day::Thu },
      { Day::Thu, Day::Sun, Day::Sun, Day::Wed, Day::Fri, Day::Mon,             /* 25 */
        Day::Wed, Day::Sat, Day::Tue, Day::Thu, Day::Sun, Day::Tue, Day::Fri },
      { Day::Fri, Day::Mon, Day::Mon, Day::Thu, Day::Sat, Day::Tue,             /* 26 */
        Day::Thu, Day::Sun, Day::Wed, Day::Fri, Day::Mon, Day::Wed, Day::Sat },
      { Day::Sat, Day::Tue, Day::Wed, Day::Sat, Day::Mon, Day::Thu,             /* 27 */
        Day::Sat, Day::Tue, Day::Fri, Day::Sun, Day::Wed, Day::Fri, Day::Mon },
    } ;

  static const uint32_t SecToMonth[13] =
    {
      ( 0                                                        ) * SecPerDay,
      ( 31                                                       ) * SecPerDay,
      ( 31 + 28                                                  ) * SecPerDay,
      ( 31 + 28 + 31                                             ) * SecPerDay,
      ( 31 + 28 + 31 + 30                                        ) * SecPerDay,
      ( 31 + 28 + 31 + 30 + 31                                   ) * SecPerDay,
      ( 31 + 28 + 31 + 30 + 31 + 30                              ) * SecPerDay,
      ( 31 + 28 + 31 + 30 + 31 + 30 + 31                         ) * SecPerDay,
      ( 31 + 28 + 31 + 30 + 31 + 30 + 31 + 31                    ) * SecPerDay,
      ( 31 + 28 + 31 + 30 + 31 + 30 + 31 + 31 + 30               ) * SecPerDay,
      ( 31 + 28 + 31 + 30 + 31 + 30 + 31 + 31 + 30 + 31          ) * SecPerDay,
      ( 31 + 28 + 31 + 30 + 31 + 30 + 31 + 31 + 30 + 31 + 30 + 31) * SecPerDay,
    } ;
  static const uint32_t SecToMonthLeap[13] =
    {
      ( 0                                                        ) * SecPerDay,
      ( 31                                                       ) * SecPerDay,
      ( 31 + 29                                                  ) * SecPerDay,
      ( 31 + 29 + 31                                             ) * SecPerDay,
      ( 31 + 29 + 31 + 30                                        ) * SecPerDay,
      ( 31 + 29 + 31 + 30 + 31                                   ) * SecPerDay,
      ( 31 + 29 + 31 + 30 + 31 + 30                              ) * SecPerDay,
      ( 31 + 29 + 31 + 30 + 31 + 30 + 31                         ) * SecPerDay,
      ( 31 + 29 + 31 + 30 + 31 + 30 + 31 + 31                    ) * SecPerDay,
      ( 31 + 29 + 31 + 30 + 31 + 30 + 31 + 31 + 30               ) * SecPerDay,
      ( 31 + 29 + 31 + 30 + 31 + 30 + 31 + 31 + 30 + 31          ) * SecPerDay,
      ( 31 + 29 + 31 + 30 + 31 + 30 + 31 + 31 + 30 + 31 + 30 + 31) * SecPerDay,
    } ;
  
  uint64_t ntpToUtc(uint64_t ntp)
  {
    return ntp + UTC2001 - NTP2001 ;
  }
  
  uint64_t utcToNtp(uint64_t utc)
  {
    return utc + NTP2001 - UTC2001 ;
  }

  uint64_t utcToT2001(uint64_t utc)
  {
    return utc - UTC2001 ;
  }

  bool isLeapYear(uint64_t utc)
  {
    return (utcToT2001(utc) / SecPerYear) >= 3 ;
  }

  ////////////////////////////////////////////////////////////////////////////////

  Rule::Rule() :
    _week(Week::First), _day(Day::Sun), _month(Month::Jan), _hour(0), _secHourOffset(0), _secOffset(0),
    _startSecInYear28(~0U), _year28(~0)
  {
  }
  
  Rule::Rule(Month month, Week week, Day day, uint8_t hour, int16_t minOffset) :
    _month(month), _week(week), _day(day), _hour(hour), _secHourOffset(_hour*60*60 - minOffset*60), _secOffset(60*minOffset),
    _startSecInYear28(~0U), _year28(~0)
  {
  }

  bool Rule::operator<=(uint64_t sec) const
  {
    uint32_t s28 = utcToT2001(sec) % SecPer28Year ;

    return startSecInYear(s28) <= s28 ;
  }

  int16_t Rule::secOffset() const
  {
    return _secOffset ;
  }

  uint32_t Rule::startSecInYear(uint64_t s28) const
  {
    uint32_t y4  = s28 / SecPer4Year ;  // cycle[0..6]  in  28 year cycle
    uint32_t s4  = s28 % SecPer4Year ;
    uint32_t y1  = s4 / SecPerYear ;    // [0..3]
    if (y1 > 3) y1 = 3 ; // 0003-12-31 would be year 4
    uint32_t y28 = y4 * 4 + y1 ;        // year in 28 year cycle

    if (y28 != _year28)
    {
      // recalculate
      bool isLeap = y1 == 3 ;
      const uint32_t *secToMonth  = isLeap ? SecToMonthLeap  : SecToMonth  ;
    
      if (_week != Week::Last)
      {
        Day firstDayOfMonth = FirstDayOfMonth[y28][(uint8_t)_month] ;
        uint8_t daysInMonth = (7 + (uint8_t)_day - (uint8_t)firstDayOfMonth) % 7 ;
        _startSecInYear28 = y4 * SecPer4Year + y1 * SecPerYear + secToMonth[(uint8_t)_month] + daysInMonth * SecPerDay +
          (uint8_t)_week*7*SecPerDay + _secHourOffset ;
      }
      else
      {
        Day firstDayOfNextMonth = FirstDayOfMonth[y28][(uint8_t)_month+1] ;
        uint8_t daysBeforeMonth = (7 + (uint8_t)firstDayOfNextMonth - (uint8_t)_day) % 7 ;
        if (!daysBeforeMonth)
          daysBeforeMonth = 7 ;
        _startSecInYear28 = y4 * SecPer4Year + y1 * SecPerYear + secToMonth[(uint8_t)_month+1] - daysBeforeMonth * SecPerDay +
          _secHourOffset ;
      }
    }

    return _startSecInYear28 ;
  }

  ////////////////////////////////////////////////////////////////////////////////

  TimeZone::TimeZone() : _nRules(0), _rules {}
  {
  }
  TimeZone::TimeZone(const Rule &r1) : _nRules(1), _rules{ r1 }
  {
  }                           
  TimeZone::TimeZone(const Rule &r1, const Rule &r2) : _nRules(2), _rules{ r1, r2 }
  {
  }
  TimeZone::TimeZone(const Rule &r1, const Rule &r2, const Rule &r3) : _nRules(3), _rules{ r1, r2, r3 }
  {
  }
  TimeZone::TimeZone(const Rule &r1, const Rule &r2, const Rule &r3, const Rule &r4) : _nRules(4), _rules{ r1, r2, r3, r4 }
  {
  }

  uint64_t TimeZone::utcToLoc(uint64_t utc)
  {
    if (!_nRules)
      return utc ;
    
    for (uint8_t iRule = _nRules ; iRule != 0 ; --iRule)
    {
      if (_rules[iRule-1] <= utc)
        return utc + _rules[iRule-1].secOffset() ;
    }
    return utc + _rules[_nRules-1].secOffset() ;
  }

  uint64_t TimeZone::locToUtc(uint64_t loc)
  {
    if (!_nRules)
      return loc ;

    for (uint8_t iRule = _nRules ; iRule != 0 ; --iRule)
    {
      if (_rules[iRule - 1] <= (loc - _rules[iRule-1].secOffset()))
        return loc - _rules[iRule-1].secOffset() ;
    }
    return loc - _rules[_nRules-1].secOffset() ;
  }
  
  uint64_t TimeZone::ntpToLoc(uint64_t ntp)
  {
    return utcToLoc(ntpToUtc(ntp)) ;
  }
  
  void TimeZone::resetRules()
  {
    _nRules = 0 ;
  }
  void TimeZone::addRule(const Rule &rule)
  {
    if (_nRules < 4)
    {
      _rules[_nRules++] = rule ;
    }
  }

}

////////////////////////////////////////////////////////////////////////////////
// EOF
////////////////////////////////////////////////////////////////////////////////
