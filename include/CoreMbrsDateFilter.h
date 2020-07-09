/////////////////////////////////////////
// CoreMbrsDateFilter.h
// Copyright (c) 2020 TXPCo Ltd
/////////////////////////////////////////

#ifndef COREMBRSDATEFILTER_INCLUDED
#define COREMBRSDATEFILTER_INCLUDED

#include "Common.h"
#include "Host.h"
#include "HostException.h"
#include "HostLocks.h"
#include "Core.h"

///////////////////////////////////////////////////////////////////////////////
// CoreMbrsDateFilterOptions
///////////////////////////////////////////////////////////////////////////////

class CORE_API CoreMbrsDateFilterOptions 
{
public:
   enum EPeriod { kNone = 0, kDay = 1, kMonth = 2, kYear = 3};
   //using filetime_period = std::ratio_multiply<std::ratio<100>, std::nano>;
   //using CoreDateTimeSpan = std::chrono::duration<int64_t, filetime_period>;
   //using CoreDateTimeOption = std::chrono::system_clock::time_point;

   // Constructors
   CoreMbrsDateFilterOptions(); 
   CoreMbrsDateFilterOptions(const std::chrono::system_clock::time_point& date, const EPeriod& period);
   virtual ~CoreMbrsDateFilterOptions(void);

   // Attributes
   std::chrono::system_clock::time_point date() const;
   EPeriod period() const;

   // Operations
   CoreMbrsDateFilterOptions& operator=(const CoreMbrsDateFilterOptions& copyMe);
   bool operator==(const CoreMbrsDateFilterOptions& rhs) const;
   bool operator!=(const CoreMbrsDateFilterOptions& rhs) const;
   bool save() const;
   bool load();

protected:

private:
   std::string format (const std::chrono::system_clock::time_point& t) const;
   std::chrono::system_clock::time_point parse(const std::string& d) const;

   std::chrono::system_clock::time_point m_date;
   EPeriod            m_period;
};



#endif // COREMBRSDATEFILTER_INCLUDED
