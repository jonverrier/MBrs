/////////////////////////////////////////
// CoreTimeUtil.h
// Copyright (c) 2020 TXPCo Ltd
/////////////////////////////////////////

#ifndef CORETIMEUTIL_INCLUDED
#define CORETIMEUTIL_INCLUDED

#include "Common.h"
#include <chrono>

// Helper function to convert a chrono time to a time_t
template <typename TP> std::time_t to_time_t(TP tp)
{
   using namespace std::chrono;
   auto sctp = time_point_cast<system_clock::duration>(tp - TP::clock::now()
      + system_clock::now());
   return system_clock::to_time_t(sctp);
}

std::chrono::seconds yearMonthDayHoursMinSecondsToSeconds(HInt year, HInt month, HInt day, HInt hour, HInt minute, HInt second);
time_t secondsToTime (const std::chrono::seconds& seconds);

inline errno_t time_s(struct tm* const tm, time_t const* const t)
{
   return localtime_s(tm, t);
}

#endif // CORETIMEUTIL_INCLUDED