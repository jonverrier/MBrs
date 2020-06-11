/////////////////////////////////////////
// CoreFile.cpp
// Copyright (c) 2020 TXPCo Ltd
/////////////////////////////////////////

#include "Common.h"
#include "CoreKeywords.h"
#include "HostUserData.h"

using namespace std;

CoreCategoryKeywords::CoreCategoryKeywords(const HString& category)
   : m_category (category), m_keywords(readKeywords(category))
{
}

CoreCategoryKeywords::CoreCategoryKeywords(const CoreCategoryKeywords& copyMe)
   : m_category(copyMe.m_category), m_keywords (copyMe.m_keywords)
{
}

CoreCategoryKeywords::~CoreCategoryKeywords(void)
{
}

HString CoreCategoryKeywords::category() const
{
   return m_category;
}

std::list<HString> CoreCategoryKeywords::keywords() const
{
   return m_keywords;
}

bool CoreCategoryKeywords::addKeyword(const HString& add)
{
   list<HString>::iterator iter = find(m_keywords.begin(), m_keywords.end(), add);

   if (iter == m_keywords.end())
   {
      m_keywords.push_back(add);
      writeKeywords(m_category, m_keywords);
      return true;
   }
   return false;
}

bool CoreCategoryKeywords::removeKeyword(const HString& remove)
{
   list<HString>::iterator iter = find(m_keywords.begin(), m_keywords.end(), remove);

   if (iter != m_keywords.end())
   {
      m_keywords.erase (iter); 
      writeKeywords(m_category, m_keywords);
      return true;
   }
   return false;
}

bool CoreCategoryKeywords::removeAllKeywords()
{
   if (m_keywords.size() > 0)
      m_keywords.clear();
   writeKeywords(m_category, m_keywords);
   return true;
}

CoreCategoryKeywords& CoreCategoryKeywords::operator=(const CoreCategoryKeywords& copyMe)
{
   m_keywords = copyMe.m_keywords;
   m_category = copyMe.m_category;

   return *this;
}

bool CoreCategoryKeywords::operator==(const CoreCategoryKeywords& rhs) const
{
   return m_category == rhs.m_category && m_keywords == rhs.m_keywords;
}

bool CoreCategoryKeywords::operator!=(const CoreCategoryKeywords& rhs) const
{
   return m_category != rhs.m_category || m_keywords != rhs.m_keywords;
}

list<HString> CoreCategoryKeywords::readKeywords(const HString& category)
{
   list<HString> list;
   std::vector<HString> stored;

   HostUserData data(CORE_PACKAGE_FRIENDLY_NAME);

   if (data.isDataStoredAt(category))
   {
      stored = data.readMultiString(category);
      list.resize(stored.size());
      list.assign(stored.begin(), stored.end());
   }

   return list;
}

void CoreCategoryKeywords::writeKeywords(const HString& category, std::list<HString>& keywords)
{
   HostUserData data(CORE_PACKAGE_FRIENDLY_NAME);

   if (keywords.size() == 0)
   {
      if (data.isDataStoredAt(category))
         data.removeData(category);
   }
   else
   {
      list<HString> list;
      std::vector<HString> stored;
      stored.resize(keywords.size());
      stored.assign(keywords.begin(), keywords.end());
      data.writeMultiString(category, stored);
   }
}
