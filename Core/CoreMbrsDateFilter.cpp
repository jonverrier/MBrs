/////////////////////////////////////////
// CoreMbrsDateFilter.cpp
// Copyright (c) 2020 TXPCo Ltd
/////////////////////////////////////////

#include "Common.h"
#include "CoreMbrsDateFilter.h"
#include "HostUserData.h"

using namespace std;

////////////////////////////////////////////////////////////////////////////
// Utilities - TODO move to header at some point
////////////////////////////////////////////////////////////////////////////
static string convertToNarrow(const HString& orig)
{
   size_t newsize = orig.size() + 1;

   unique_ptr<char> pNew(COMMON_NEW char[newsize]);

   // Convert w_char_t* string to a char* string.
   size_t convertedChars = 0;
   wcstombs_s(&convertedChars, pNew.get(), newsize, orig.c_str(), _TRUNCATE);

   string returnString(pNew.get());

   return returnString;
}

static HString convertToWide(const string& orig)
{
   size_t newsize = orig.size() + 1;

   unique_ptr<wchar_t> pNew(COMMON_NEW wchar_t[newsize]);

   // Convert char* string to a wchar_t* string.
   size_t convertedChars = 0;
   mbstowcs_s(&convertedChars, pNew.get(), newsize, orig.c_str(), _TRUNCATE);

   HString returnString(pNew.get());

   return returnString;
}

////////////////////////////////////////////////////////////////////////////
// CoreMbrsDateFilter
////////////////////////////////////////////////////////////////////////////

CoreMbrsDateFilterOptions::CoreMbrsDateFilterOptions()
   : m_period(EPeriod::kNone)
{
}

CoreMbrsDateFilterOptions::CoreMbrsDateFilterOptions(const std::chrono::system_clock::time_point& date, const EPeriod& period)
   : m_date (parse(format(date))), m_period (period)
{
}

CoreMbrsDateFilterOptions::~CoreMbrsDateFilterOptions(void)
{
}

std::chrono::system_clock::time_point CoreMbrsDateFilterOptions::date() const
{
   return m_date;
}

CoreMbrsDateFilterOptions::EPeriod CoreMbrsDateFilterOptions::period() const
{
   return m_period;
}

CoreMbrsDateFilterOptions& CoreMbrsDateFilterOptions::operator=(const CoreMbrsDateFilterOptions& copyMe)
{
   m_date = copyMe.m_date;
   m_period = copyMe.m_period;

   return *this;
}

bool CoreMbrsDateFilterOptions::operator==(const CoreMbrsDateFilterOptions& rhs) const
{
   return m_date.time_since_epoch() == rhs.m_date.time_since_epoch() && m_period == rhs.m_period;
}

bool CoreMbrsDateFilterOptions::operator!=(const CoreMbrsDateFilterOptions& rhs) const
{
   return m_date.time_since_epoch() != rhs.m_date.time_since_epoch() || m_period != rhs.m_period;
}

static const HChar* dateFolderKey = H_TEXT("LastDate");
static const HChar* periodFolderKey = H_TEXT("LastPeriod");

bool CoreMbrsDateFilterOptions::save() const
{
   string s = format(m_date);

   HostUserData date(CORE_PACKAGE_FRIENDLY_NAME);

   date.writeString(dateFolderKey, convertToWide(s));

   date.writeUint (periodFolderKey, static_cast<HUint>(m_period));

   return true;
}

bool CoreMbrsDateFilterOptions::load() 
{
   HostUserData date(CORE_PACKAGE_FRIENDLY_NAME);

   // If there is a prior key, and it points to data, then use it, else return current time
   if (date.isDataStoredAt(dateFolderKey) && date.isDataStoredAt(periodFolderKey))
   {
      HString ws = date.readString(dateFolderKey);
      if (ws.size() > 0)
      {
         string s = convertToNarrow(ws);
         m_date = parse(s); 
         m_period = static_cast<EPeriod> (date.readUint(periodFolderKey));
         return true;
      }
      else
      {
         m_date = std::chrono::system_clock::now();
         m_period = EPeriod::kNone;
         return false;
      }
   }
   else
   {
      m_date = std::chrono::system_clock::now();
      m_period = EPeriod::kNone;
      return false;
   }
}

std::string CoreMbrsDateFilterOptions::format(const std::chrono::system_clock::time_point& t) const
{
   // truncate a general timepoint by writing out only the date portion
   char dateString[COMMON_STRING_BUFFER_SIZE];

   std::time_t rawtime = std::chrono::system_clock::to_time_t(t);
   std::tm timeinfo;
   localtime_s(&timeinfo, &rawtime);
   strftime(dateString, COMMON_STRING_BUFFER_SIZE,
           ("%Y %m %d"), &timeinfo);

   return string(dateString); 
}

std::chrono::system_clock::time_point CoreMbrsDateFilterOptions::parse(const std::string& d) const
{
   std::tm timeinfo;
   memset(&timeinfo, sizeof(timeinfo), 0);

   // Set up the timeinfo structure by populating it via localtime_s
   time_t rawtime = 0;
   localtime_s(&timeinfo, &rawtime);

   std::stringstream ss(d);
   ss >> std::get_time(&timeinfo, "%Y %m %d");

   rawtime = std::mktime(&timeinfo);
   return std::chrono::system_clock::from_time_t(rawtime);
}
