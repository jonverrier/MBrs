/////////////////////////////////////////
// CoreImageFile.h
// Copyright (c) 2020 TXPCo Ltd
/////////////////////////////////////////

#ifndef COREIMAGEFILE_INCLUDED
#define COREIMAGEFILE_INCLUDED

#include "Common.h"
#include "exiv2/exiv2.hpp"
#include "Host.h"
#include "HostException.h"
#include "HostLocks.h"
#include "Core.h"
#include "CoreFile.h"

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

   Exiv2::Image::AutoPtr openImage(const HString& path, HUint& fileError) const;

   time_t readExifDateTime (Exiv2::Image::AutoPtr& pImage, HUint& fileError);
   
   std::list<HString> readSubjectTags(Exiv2::Image::AutoPtr& pImage, HUint& fileError);

   std::list<HString> readExifSubjectTags(Exiv2::Image::AutoPtr& pImage, HUint& fileError);
   
   void writeExifSubjectTags(Exiv2::Image::AutoPtr& pImage, const std::list<HString>& tags, HUint& fileError) const;
   
   std::list<HString> readIptcSubjectTags(Exiv2::Image::AutoPtr& pImage, HUint& fileError);

   void writeIptcSubjectTags(Exiv2::Image::AutoPtr& pImage, const std::list<HString>& tags, HUint& fileError) const;

   std::list<HString> readXmpSubjectTags(Exiv2::Image::AutoPtr& pImage, HUint& fileError);

   void writeXmpSubjectTags(Exiv2::Image::AutoPtr& pImage, const std::list<HString>& tags, HUint& fileError) const;

   std::list<HString> deduplicateTags (std::list<HString>& consolidatedList, const std::list<HString>& newList);

   std::list<HString> parseDelimiters(const HString& input, const HString& delimiter) const;
   
   std::list<HString> storeSubjectTag(std::list<HString>& tags, const HString& input) const;
   
   HString makeDelimited(const  std::list<HString>& input, const HString& delimiter) const;

   HString convertToWide(const std::string& orig) const;
   
   std::string convertToNarrow(const HString& orig) const;
 
   std::list<HString> m_tagCache;
   time_t        m_takenAt;
};

#endif // COREIMAGEFILE_INCLUDED
