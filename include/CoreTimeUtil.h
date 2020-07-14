/////////////////////////////////////////
// CoreTimeUtil.h
// Copyright (c) 2020 TXPCo Ltd
/////////////////////////////////////////

#ifndef CORETIMEUTIL_INCLUDED
#define CORETIMEUTIL_INCLUDED

// Helper function to convert a chrono time to a time_t
template <typename TP> std::time_t to_time_t(TP tp)
{
   using namespace std::chrono;
   auto sctp = time_point_cast<system_clock::duration>(tp - TP::clock::now()
      + system_clock::now());
   return system_clock::to_time_t(sctp);
}

#endif // CORETIMEUTIL_INCLUDED