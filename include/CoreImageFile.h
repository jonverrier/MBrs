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
   time_t takenAt() const;

   // Operations
   std::list<HString> addSubjectTags(const std::list<HString>& add);
   std::list<HString> removeSubjectTags(const std::list<HString>& add);
   std::list<HString> writeSubjectTags();

   CoreImageFile& operator=(const CoreImageFile& copyMe);

   // Comparison Operators
   bool operator==(const CoreImageFile& rhs) const;
   bool operator!=(const CoreImageFile& rhs) const;

protected:

private:
   CoreImageFile(); // Cannot create without a path 

   void readMetaData();

   std::list<HString> m_tagCache;
   time_t             m_takenAt;
};

#endif // COREIMAGEFILE_INCLUDED
