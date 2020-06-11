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

typedef  std::list<HString> CoreImageTagList;

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

   // Operations
   CoreImageTagList readSubjectTags(HUint &fileError);
   CoreImageTagList addSubjectTags(const CoreImageTagList& add);
   CoreImageTagList removeSubjectTags(const CoreImageTagList& add);
   CoreImageTagList writeSubjectTags(HUint& fileError);
   CoreImageFile& operator=(const CoreImageFile& copyMe);

   // Comparison Operators
   bool operator==(const CoreImageFile& rhs) const;
   bool operator!=(const CoreImageFile& rhs) const;

protected:

private:
   CoreImageFile(); // Cannot create without a path 

   Exiv2::Image::AutoPtr openImage(const HString& path, HUint& fileError) const;

   void readExifDateTime (Exiv2::Image::AutoPtr& pImage, HUint& fileError);

   CoreImageTagList readExifSubjectTags(Exiv2::Image::AutoPtr& pImage, HUint& fileError);
   
   void writeExifSubjectTags(Exiv2::Image::AutoPtr& pImage, const CoreImageTagList& tags, HUint& fileError) const;
   
   CoreImageTagList readIptcSubjectTags(Exiv2::Image::AutoPtr& pImage, HUint& fileError);

   void writeIptcSubjectTags(Exiv2::Image::AutoPtr& pImage, const CoreImageTagList& tags, HUint& fileError) const;

   CoreImageTagList readXmpSubjectTags(Exiv2::Image::AutoPtr& pImage, HUint& fileError);

   void writeXmpSubjectTags(Exiv2::Image::AutoPtr& pImage, const CoreImageTagList& tags, HUint& fileError) const;

   CoreImageTagList deduplicateTags (CoreImageTagList& consolidatedList, const CoreImageTagList& newList);

   CoreImageTagList parseDelimiters(const HString& input, const HString& delimiter) const;
   
   CoreImageTagList storeSubjectTag(CoreImageTagList& tags, const HString& input) const;
   
   HString makeDelimited(const  CoreImageTagList& input, const HString& delimiter) const;

   HString convertToWide(const std::string& orig) const;
   
   std::string convertToNarrow(const HString& orig) const;
 
   CoreImageTagList m_tagCache;
};

#endif // COREIMAGEFILE_INCLUDED
