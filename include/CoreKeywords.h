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
   std::list<HString> keywords() const;

   // Operations
  bool addKeyword(const HString& add);
  bool removeKeyword(const HString& remove);
  bool removeAllKeywords();

   CoreCategoryKeywords& operator=(const CoreCategoryKeywords& copyMe);

   // Comparison Operators
   bool operator==(const CoreCategoryKeywords& rhs) const;
   bool operator!=(const CoreCategoryKeywords& rhs) const;

protected:

private:
   CoreCategoryKeywords() = delete; // Cannot create without a category 

   std::list<HString> readKeywords(const HString& category);
   void writeKeywords(const HString& category, std::list<HString>& keywords);

   HString m_category;
   std::list<HString> m_keywords;
};

#endif // COREKEYWORDS_INCLUDED
