/////////////////////////////////////////
// CoreFile.h
// Copyright (c) 2020 TXPCo Ltd
/////////////////////////////////////////

#ifndef COREFILE_INCLUDED
#define COREFILE_INCLUDED

#include <exiv2/exiv2.hpp>
#include "CommonDefinitions.h"
#include "CommonStandardLibraryIncludes.h"
#include "Host.h"
#include "HostException.h"
#include "HostLocks.h"
#include "Core.h"
#include "CoreQueue.h"

///////////////////////////////////////////////////////////////////////////////
// CoreFileSystemEntity
///////////////////////////////////////////////////////////////////////////////

class CORE_API CoreFileSystemEntity
{
public :
// Constructors

   CoreFileSystemEntity(const HString& path);

   CoreFileSystemEntity(const CoreFileSystemEntity& copyMe);

   ~CoreFileSystemEntity(void);
   
// Attributes
   HString
   path() const;

   bool
   existsOnFileSystem () const;

   bool 
   lastWriteTime(std::filesystem::file_time_type& writeTime) const;

   bool
   isDirectory () const;

   bool
   isFile() const;

   bool
   isImageFile() const;
   
// Operations

   CoreFileSystemEntity&
   operator=(const CoreFileSystemEntity& copyMe);

// Comparison Operators
   
   bool   
   operator==(const CoreFileSystemEntity& rhs) const;

   bool
   operator!=(const CoreFileSystemEntity& rhs) const;

protected :

private :
   CoreFileSystemEntity(); // Cannot create without a path 

   HString m_path;
};

///////////////////////////////////////////////////////////////////////////////
// CoreDirectory
///////////////////////////////////////////////////////////////////////////////

class CORE_API CoreDirectory : public CoreFileSystemEntity 
{
public:
   // Constructors

   CoreDirectory(const HString& path);

   CoreDirectory(const CoreDirectory& copyMe);

   ~CoreDirectory(void);

   // Attributes

   // Operations

   CoreDirectory&
   operator=(const CoreDirectory& copyMe);

   void
   imageDirContents(std::list<HString>& images, std::list<HString>& dirs) const;

   // Comparison Operators

   bool
      operator==(const CoreDirectory& rhs) const;

   bool
      operator!=(const CoreDirectory& rhs) const;

protected:

private:
   CoreDirectory(); // Cannot create without a path 

   HString m_path;
};

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
   CoreImageTagList
   readSubjectTags(HUint &fileError);

   CoreImageTagList
   addSubjectTags(const CoreImageTagList& add);

   CoreImageTagList
   removeSubjectTags(const CoreImageTagList& add);

   CoreImageTagList
   writeSubjectTags(HUint& fileError);

   CoreImageFile&
      operator=(const CoreImageFile& copyMe);

   // Comparison Operators

   bool
   operator==(const CoreImageFile& rhs) const;

   bool
   operator!=(const CoreImageFile& rhs) const;


   // Operations
   static void
   saveImageDirectory(const HString& folder);

   static HString
   loadImageDirectory();

protected:

private:
   CoreImageFile(); // Cannot create without a path 

   Exiv2::Image::AutoPtr 
   openImage(const HString& path, HUint& fileError) const;

   CoreImageTagList
   readExifSubjectTags(Exiv2::Image::AutoPtr& image, HUint& fileError);

   void
   writeExifSubjectTags(Exiv2::Image::AutoPtr& image, const CoreImageTagList& tags, HUint& fileError) const;

   CoreImageTagList
   readIptcSubjectTags(Exiv2::Image::AutoPtr& image, HUint& fileError);

   void
   writeIptcSubjectTags(Exiv2::Image::AutoPtr& image, const CoreImageTagList& tags, HUint& fileError) const;

   CoreImageTagList
   readXmpSubjectTags(Exiv2::Image::AutoPtr& image, HUint& fileError);

   void
   writeXmpSubjectTags(Exiv2::Image::AutoPtr& image, const CoreImageTagList& tags, HUint& fileError) const;

   CoreImageTagList
   deduplicateTags (CoreImageTagList& consolidatedList, const CoreImageTagList& newList);

   CoreImageTagList
   parseDelimiters(const HString& input, const HString& delimiter) const;
   
   CoreImageTagList
   storeSubjectTag(CoreImageTagList& tags, const HString& input) const;
   
   HString
   makeDelimited(const  CoreImageTagList& input, const HString& delimiter) const;

   HString
   convertToWide(const std::string& orig) const;
   
   std::string
   convertToNarrow(const HString& orig) const;

   HString m_path;
   CoreImageTagList m_tagCache;
};

///////////////////////////////////////////////////////////////////////////////
// CoreDirectorySearch
///////////////////////////////////////////////////////////////////////////////

class CORE_API CoreDirectorySearch 
{
public:
   typedef CoreConcurrentQueue<HString> SearchQueue;

   // Constructors

   CoreDirectorySearch (const HString& path);

   CoreDirectorySearch (const CoreDirectorySearch& copyMe);

   ~CoreDirectorySearch(void);

   // Attributes

   // Operations

   CoreDirectorySearch&
   operator=(const CoreDirectorySearch& copyMe);

   void
   listImagesSynch (SearchQueue& queueIn, SearchQueue& queueOut);

   void
   listImagesAsynch (SearchQueue& queueIn, SearchQueue& queueOut);

   bool
   isActive() const;

   // Comparison Operators
   bool
      operator==(const CoreDirectorySearch& rhs) const;

   bool
      operator!=(const CoreDirectorySearch& rhs) const;

protected:

private:
   CoreDirectorySearch(); // Cannot create without a path 

   HostInterlockedCount m_activeThreadCount; 

   CoreDirectory m_rootDir;
};

///////////////////////////////////////////////////////////////////////////////
// CoreUserImageFolder
///////////////////////////////////////////////////////////////////////////////

class CORE_API CoreUserImageFolder
{
public:



protected:

private:

   CoreUserImageFolder(); // Cannot create without a path 
   ~CoreUserImageFolder(void);
};

#endif // COREFILE_INCLUDED
