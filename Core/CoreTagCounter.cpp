/////////////////////////////////////////
// CoreTagCounter.cpp
// Copyright (c) 2020 TXPCo Ltd
/////////////////////////////////////////

#include "Common.h"
#include "CoreImageFile.h"

using namespace std;

// Constructors
CoreSubjectTagCounter::CoreSubjectTagCounter()
   : m_count(0)
{
}

CoreSubjectTagCounter::~CoreSubjectTagCounter(void)
{
}

void CoreSubjectTagCounter::addTags(const std::list<HString>& tags)
{
   m_count++;

   for (auto tag : tags)
   {
      auto iter = m_tags.find(tag);
      if (iter != m_tags.end())
      {
         (*iter).second++;
      }
      else
      {
         pair<HString, HUint> p(tag, 1);
         m_tags.insert(p);
      }
   }
}

CoreSubjectTagCounter::EUsed CoreSubjectTagCounter::countOf(const HString& tag)
{
   auto iter = m_tags.find(tag);
   if (iter != m_tags.end())
   {
      if ((*iter).second == m_count)
         return kAll;
      else
         return kSome;
   }
   else
   {
      return kNone;
   }
}

CoreSubjectTagCounter& CoreSubjectTagCounter::operator=(const CoreSubjectTagCounter& copyMe)
{
   m_count = copyMe.m_count;
   m_tags = copyMe.m_tags;

   return *this;
}

bool CoreSubjectTagCounter::operator==(const CoreSubjectTagCounter& rhs) const
{
   if (m_count == rhs.m_count && m_tags == rhs.m_tags)
      return true;
   else
      return false;
}

bool CoreSubjectTagCounter::operator!=(const CoreSubjectTagCounter& rhs) const
{
   if (m_count != rhs.m_count || m_tags != rhs.m_tags)
      return true;
   else
      return false;
}
