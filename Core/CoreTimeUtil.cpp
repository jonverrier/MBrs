/////////////////////////////////////////
// CoreFile.cpp
// Copyright (c) 2020 TXPCo Ltd
/////////////////////////////////////////

#include "Common.h"
#include "CoreTimeUtil.h"

using namespace std;

// To store number of days in all months from January to Dec. 
const int g_monthDays[12] = { 31, 28, 31, 30, 31, 30,
                              31, 31, 30, 31, 30, 31 };

// given date 
HInt countLeapYears(HInt year, HInt month)
{
   // Check if the current year needs to be considered 
   // for the count of leap years or not 
   if (month < 2)
      year--;

   // An year is a leap year if it is a multiple of 4, 
   // multiple of 400 and not a multiple of 100. 
   return year / 4 - year / 100 + year / 400;
}

HInt yearMonthDayToHours(HInt year, HInt month, HInt day)
{
   // initialize count using years and day 
   HInt n = year * 365;

   // Add days for months in given date 
   for (int i = 0; i < month - 1; i++)
      n += g_monthDays[i];

   // Since every leap year is of 366 days, 
   // Add a day for every leap year 
   n += countLeapYears(year, month);

   // Add day in month
   n += (day - 1);

   return n * 24;
}

std::chrono::seconds yearMonthDayHoursMinSecondsToSeconds(HInt year, HInt month, HInt day, HInt hour, HInt minute, HInt second)
{
   std::chrono::hours hours = std::chrono::hours(yearMonthDayToHours(year, month, day) + hour);
   std::chrono::seconds seconds = std::chrono::duration_cast<std::chrono::seconds> (hours) + std::chrono::seconds(minute * 60 + second);

   return seconds;
}

std::chrono::time_point<std::chrono::system_clock> secondsToSystemTime(const std::chrono::seconds& seconds)
{
   std::chrono::seconds epoch = yearMonthDayHoursMinSecondsToSeconds(1970, 1, 1, 0, 0, 0);

   std::chrono::seconds delta = seconds - epoch;
   const time_t t = delta.count();

   return std::chrono::system_clock::from_time_t (t);
}