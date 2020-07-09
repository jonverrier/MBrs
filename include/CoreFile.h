/////////////////////////////////////////
// CoreImageFile.h
// Copyright (c) 2020 TXPCo Ltd
/////////////////////////////////////////

#ifndef COREFILE_INCLUDED
#define COREFILE_INCLUDED

#include "Common.h"
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
   HString path() const;
   HString filename() const;
   std::filesystem::file_time_type lastWriteTime() const;
   bool setLastWriteTime(const std::filesystem::file_time_type& writeTime) const;

   bool existsOnFileSystem () const;
   bool isDirectory () const;
   bool isFile() const;
   bool isImageFile() const;
   
// Operations
   CoreFileSystemEntity& operator=(const CoreFileSystemEntity& copyMe);

   // Operations
   static bool saveImageDirectory(const HString& folder);
   static HString loadImageDirectory();

// Comparison Operators   
   bool operator==(const CoreFileSystemEntity& rhs) const;
   bool operator!=(const CoreFileSystemEntity& rhs) const;

protected :

private :
   CoreFileSystemEntity() = delete; // Cannot create without a path 

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

   // Operations
   CoreDirectory& operator=(const CoreDirectory& copyMe);
   void listImages (std::vector<HString>& images) const;

   // Comparison Operators
   bool operator==(const CoreDirectory& rhs) const;
   bool operator!=(const CoreDirectory& rhs) const;

protected:

private:
   HUint m_maxSearchSize = 250;

   CoreDirectory(); // Cannot create without a path 
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
   CoreDirectorySearch& operator=(const CoreDirectorySearch& copyMe);
   void listImages (SearchQueue& queueImg);


   // Comparison Operators
   bool operator==(const CoreDirectorySearch& rhs) const;

   bool operator!=(const CoreDirectorySearch& rhs) const;

protected:

private:
   CoreDirectorySearch(); // Cannot create without a path 

   CoreDirectory m_rootDir;
};

#endif // COREFILE_INCLUDED
