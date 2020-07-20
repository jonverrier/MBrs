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

// Helper function to convert a chrono time to a time_t
template <typename TP> void from_time_t(time_t t, TP& tp)
{
   auto sctp = std::chrono::system_clock::from_time_t(t);
   tp =std::chrono::time_point_cast<TP::clock::duration>(sctp - std::chrono::system_clock::now() + TP::clock::now());
}

// Helper function to convert a chrono time to a time_t
template <typename TP1, typename TP2> void convert_tp (const TP1& tp1, TP2& tp2)
{
   tp2 = std::chrono::time_point_cast<TP2::clock::duration>(tp1 -TP1::clock::now() + TP2::clock::now());
}

// Convert time breakdown to seconds
std::chrono::seconds yearMonthDayHoursMinSecondsToSeconds(HInt year, HInt month, HInt day, HInt hour, HInt minute, HInt second);

// Convert seconds to time_point
std::chrono::time_point<std::chrono::system_clock> secondsToSystemTime (const std::chrono::seconds& seconds);

// Single point to call either localtime() or gmtime(). Currenty use localtime() lse you get odd artefacts around dates where clocks move. 
inline errno_t time_s(struct tm* const tm, time_t const* const t)
{
   return localtime_s(tm, t);
}

#endif // CORETIMEUTIL_INCLUDED