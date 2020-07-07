/////////////////////////////////////////
// CoreImageFile.h
// Copyright (c) 2020 TXPCo Ltd
/////////////////////////////////////////

#ifndef COREIMAGEFILE_INCLUDED
#define COREIMAGEFILE_INCLUDED

#include "Common.h"
#include "Host.h"
#include "HostException.h"
#include "HostLocks.h"
#include "Core.h"
#include "CoreFile.h"
// #include "exiv2/exiv2.hpp"

///////////////////////////////////////////////////////////////////////////////
// CoreImageFile
///////////////////////////////////////////////////////////////////////////////

class CORE_API CoreImageFile : public CoreFileSystemEntity
{
public:
   // Constructors
   CoreImageFile(const HString& path);
   CoreImageFile(const CoreImageFile& copyMe);
   ~CoreImageFile(void);

   // Attributes
   std::list<HString> subjectTags() const;
   bool hasSubjectTag (const HString& tagToTest) const;
   time_t takenAt() const;

   // Operations
   std::list<HString> addSubjectTags(const std::list<HString>& add);
   std::list<HString> removeSubjectTags(const std::list<HString>& add);
   bool writeSubjectTags();
   std::list<HString> actualAddSubjectTags(const std::list<HString>& add);
   std::list<HString> actualRemoveSubjectTags(const std::list<HString>& remove);

   CoreImageFile& operator=(const CoreImageFile& copyMe);

   // Comparison Operators
   bool operator==(const CoreImageFile& rhs) const;
   bool operator!=(const CoreImageFile& rhs) const;

protected:

private:
   const HInt minFileSize = 1024; // Only try to open files >=  this size. 

   CoreImageFile(); // Cannot create without a path 

   void readMetaData();

   std::list<HString> m_tagCache;
   time_t             m_takenAt;
};

///////////////////////////////////////////////////////////////////////////////
// CoreSubjectTagCounter
///////////////////////////////////////////////////////////////////////////////

class CORE_API CoreSubjectTagCounter 
{
public:

   enum class CORE_API EUsed { kAll, kNone, kSome };

   // Constructors
   CoreSubjectTagCounter();
   ~CoreSubjectTagCounter(void);

   // Attributes
   const std::map<HString, HUint> tags() const;

   // Operations
   void addTags(const std::list<HString>& tags);
   EUsed countOf (const HString& tag) const;

   CoreSubjectTagCounter& operator=(const CoreSubjectTagCounter& copyMe);

   // Comparison Operators
   bool operator==(const CoreSubjectTagCounter& rhs) const;
   bool operator!=(const CoreSubjectTagCounter& rhs) const;

protected:

private:
   HUint m_count;
   std::map<HString, HUint> m_tags;
};
#endif // COREIMAGEFILE_INCLUDED
