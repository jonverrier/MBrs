/////////////////////////////////////////
// CoreFile.cpp
// Copyright (c) 2020 TXPCo Ltd
/////////////////////////////////////////

#include "CorePrecompile.h"
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

CoreDirectory&
CoreDirectory::operator=(const CoreDirectory& src)
{
   CoreFileSystemEntity::operator=(src);

   return *this;
}

bool
CoreDirectory::operator==(const CoreDirectory& rhs) const
{
   if (this == &rhs)
      return TRUE;

   return (CoreFileSystemEntity::operator==(rhs));
}

bool
CoreDirectory::operator!=(const CoreDirectory& rhs) const
{
   if (this == &rhs)
      return FALSE;

   return (CoreFileSystemEntity::operator!=(rhs));
}

void
CoreDirectory::imageDirContents(std::list<HString>& images, std::list<HString>& dirs) const
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
   m_rootDir (path), m_activeThreadCount(0)
{
}

CoreDirectorySearch::~CoreDirectorySearch(void)
{
}

CoreDirectorySearch::CoreDirectorySearch(const CoreDirectorySearch& src) : 
   m_rootDir (src.m_rootDir), m_activeThreadCount(0)
{

}

CoreDirectorySearch&
CoreDirectorySearch::operator=(const CoreDirectorySearch& src)
{
   m_rootDir = src.m_rootDir;

   return *this;
}

bool
CoreDirectorySearch::operator==(const CoreDirectorySearch& rhs) const
{
   if (this == &rhs)
      return TRUE;

   return (m_rootDir == rhs.m_rootDir);
}

bool
CoreDirectorySearch::operator!=(const CoreDirectorySearch& rhs) const
{
   if (this == &rhs)
      return FALSE;

   return (m_rootDir != rhs.m_rootDir);
}

void
CoreDirectorySearch::listImagesSynch(CoreDirectorySearch::SearchQueue& queueIn,
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

// Only start one asnc thread until race condition for threads that might wait for ever is resolve
// Or don't resolve it - might go all the way up to overall UI, only one thread ever searching
const int nbConsumers = 1;

void consume (CoreDirectorySearch::SearchQueue& queueIn, 
              CoreDirectorySearch::SearchQueue& queueOut,
              HostInterlockedCount& activeCount)
{      
    while (queueIn.size() > 0)
    {
        auto item = queueIn.pop();
        CoreDirectorySearch nextSearch(item);

        nextSearch.listImagesSynch(queueIn,
                                      queueOut);
    } 
    activeCount.decrement();
}

void
CoreDirectorySearch::listImagesAsynch(CoreDirectorySearch::SearchQueue& queueIn, 
                                      CoreDirectorySearch::SearchQueue& images)
{
   // Push the first search - new threads will then search new directories as they are found
   // Since they start immediately, first item needs to be on queue before they start
   queueIn.push(m_rootDir.path());

   // Start consumer threads
   std::vector<std::thread> consumers;
   for (int i = 0; i < nbConsumers; ++i) 
   {
      m_activeThreadCount.increment();
      std::thread consumer(std::bind(&consume,
                                     std::ref(queueIn), 
                                     std::ref(images),
                                     std::ref(m_activeThreadCount)));
      consumers.push_back(std::move(consumer));
   }

   // detach all async threads
   for (auto& consumer : consumers) 
   {
      consumer.detach();
   }
}

bool
CoreDirectorySearch::isActive () const
{
   return m_activeThreadCount.count() > 0;
}