/////////////////////////////////////////
// CoreKeywords.h
// Copyright (c) 2020 TXPCo Ltd
/////////////////////////////////////////

#ifndef COREKEYWORDS_INCLUDED
#define COREKEYWORDS_INCLUDED

#include "Common.h"
#include "Host.h"
#include "HostException.h"
#include "HostLocks.h"
#include "Core.h"

///////////////////////////////////////////////////////////////////////////////
// CoreCategoryKeywords
///////////////////////////////////////////////////////////////////////////////

class CORE_API CoreCategoryKeywords
{
public:
   // Constructors
   CoreCategoryKeywords(const HString& category);
   CoreCategoryKeywords(const CoreCategoryKeywords& copyMe);
   ~CoreCategoryKeywords(void);

   // Attributes
   HString category() const;
   std::vector<HString> keywords() const;
   bool hasKeyword(const HString& test) const;

   // Operations
   bool addKeyword(const HString& add);
   bool removeKeyword(const HString& remove);
   bool removeAllKeywords();

   CoreCategoryKeywords& operator=(const CoreCategoryKeywords& copyMe);

   // Comparison Operators
   bool operator==(const CoreCategoryKeywords& rhs) const;
   bool operator!=(const CoreCategoryKeywords& rhs) const;

   static const HString peopleKey ();
   static const HString placesKey ();
   static const HString timesKey ();

protected:

private:
   CoreCategoryKeywords() = delete; // Cannot create without a category 

   std::vector<HString> readKeywords(const HString& category);
   void writeKeywords(const HString& category, std::vector<HString>& keywords);

   HString m_category;
   std::vector<HString> m_keywords;
};

#endif // COREKEYWORDS_INCLUDED
