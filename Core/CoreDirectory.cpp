/////////////////////////////////////////
// CoreFile.cpp
// Copyright (c) 2020 TXPCo Ltd
/////////////////////////////////////////

#include "Common.h"
#include "CoreFile.h"

using namespace std;

////////////////////////////////////////////////////////////////////////////
// CoreDirectory
////////////////////////////////////////////////////////////////////////////

CoreDirectory::CoreDirectory(const HString& path) :
   CoreFileSystemEntity (path)
{
}

CoreDirectory::~CoreDirectory(void)
{
}

CoreDirectory::CoreDirectory(const CoreDirectory& src) :
   CoreFileSystemEntity(src)
{

}

CoreDirectory& CoreDirectory::operator=(const CoreDirectory& src)
{
   CoreFileSystemEntity::operator=(src);

   return *this;
}

bool CoreDirectory::operator==(const CoreDirectory& rhs) const
{
   if (this == &rhs)
      return TRUE;

   return (CoreFileSystemEntity::operator==(rhs));
}

bool CoreDirectory::operator!=(const CoreDirectory& rhs) const
{
   if (this == &rhs)
      return FALSE;

   return (CoreFileSystemEntity::operator!=(rhs));
}

void CoreDirectory::imageDirContents(std::list<HString>& images, std::list<HString>& dirs) const
{
   filesystem::directory_iterator iter(path());
   images.clear();
   dirs.clear();

   for (auto& p : iter)
   {
      HString contentPath((*iter).path());

      CoreFileSystemEntity directoryEntry (contentPath);

      if (directoryEntry.isImageFile())
      {
         images.push_back(contentPath);
      }
      if (directoryEntry.isDirectory())
      {
         dirs.push_back(contentPath);
      }
   }
}

////////////////////////////////////////////////////////////////////////////
// CoreDirectorySearch
////////////////////////////////////////////////////////////////////////////

CoreDirectorySearch::CoreDirectorySearch(const HString& path) :
   m_rootDir (path)
{
}

CoreDirectorySearch::~CoreDirectorySearch(void)
{
}

CoreDirectorySearch::CoreDirectorySearch(const CoreDirectorySearch& src) : 
   m_rootDir (src.m_rootDir)
{

}

CoreDirectorySearch& CoreDirectorySearch::operator=(const CoreDirectorySearch& src)
{
   m_rootDir = src.m_rootDir;

   return *this;
}

bool CoreDirectorySearch::operator==(const CoreDirectorySearch& rhs) const
{
   if (this == &rhs)
      return TRUE;

   return (m_rootDir == rhs.m_rootDir);
}

bool CoreDirectorySearch::operator!=(const CoreDirectorySearch& rhs) const
{
   if (this == &rhs)
      return FALSE;

   return (m_rootDir != rhs.m_rootDir);
}

void CoreDirectorySearch::listImages(CoreDirectorySearch::SearchQueue& queueIn,
                                     CoreDirectorySearch::SearchQueue& queueOut)
{
   CoreDirectory dir(m_rootDir);
   list<HString> images, subDirectories;

   dir.imageDirContents(images, subDirectories);

   for (auto subDir : subDirectories)
   {
      queueIn.push(subDir);
   }

   for (auto image : images)
   {
      queueOut.push(image);
   }
}
