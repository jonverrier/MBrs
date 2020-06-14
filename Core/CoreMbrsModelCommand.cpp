/////////////////////////////////////////
// CoreModelCommand.cpp
// Copyright (c) 2020 TXPCo Ltd
/////////////////////////////////////////

#include "Common.h"

#include "CoreFile.h"
#include "CoreModelCommand.h"
#include "CoreMbrsModelCommand.h"

using namespace std;

///////////////////////////////////////////////////////////////////////////////
// CoreImageListModel
///////////////////////////////////////////////////////////////////////////////


CoreImageListModel::CoreImageListModel()
   : m_path(CoreFileSystemEntity::loadImageDirectory())
{
   refreshImageList();
}

CoreImageListModel::CoreImageListModel(const HString& path)
   : m_path (path)
{
   refreshImageList();
}

CoreImageListModel::~CoreImageListModel(void)
{
}


const HString CoreImageListModel::path() const
{
   return m_path;
}

const list<CoreImageFile> CoreImageListModel::images() const
{
   return m_images;
}

const list<CoreImageFile> CoreImageListModel::imagesFor(HInt year) const
{
   list<CoreImageFile> images;

   for (auto image : m_images)
   {
      struct tm tm;
      time_t t = image.takenAt();
      localtime_s(&tm, &t);
      if (tm.tm_year + 1900 == year) // localtime return years since 1900 
         images.push_back(image);
   }
   return images;
}

const list<CoreImageFile> CoreImageListModel::imagesFor(HInt year, HInt month) const
{
   list<CoreImageFile> images;

   for (auto image : m_images)
   {
      struct tm tm;
      time_t t = image.takenAt();
      localtime_s(&tm, &t);
      if (tm.tm_year + 1900 == year && tm.tm_mon + 1  == month) // localtime return years since 1900, months are zero based
         images.push_back(image);
   }
   return images;
}

void CoreImageListModel::setPath(const HString& path)
{
   m_path = path;
   CoreFileSystemEntity::saveImageDirectory(path);
   refreshImageList();
}

// order the list by date-time taken, newest first (highest takenAt() 
bool compare (const CoreImageFile& i, const CoreImageFile& j) 
{ 
   return (j.takenAt() < i.takenAt()); 
}

void CoreImageListModel::refreshImageList()
{
   list<HString> imagePaths;
   CoreDirectory dir(m_path);
   dir.listImages(imagePaths);

   m_images.clear();

   for (auto path : imagePaths)
   {
      CoreImageFile image(path);
      m_images.push_back (image);
   }

   m_images.sort (compare);
}

///////////////////////////////////////////////////////////////////////////////
// CoreChangeDirectoryCommand
///////////////////////////////////////////////////////////////////////////////

CoreChangeDirectoryCommand::CoreChangeDirectoryCommand(const HString& newPath, const HString& oldPath, 
    std::shared_ptr< CoreImageListModel> pModel, std::shared_ptr< CoreSelection> pSelection)
   : CoreCommand (), 
     m_newPath (newPath), m_oldPath (oldPath), m_pModel (pModel), m_pSelection (pSelection)
{
}

CoreChangeDirectoryCommand::~CoreChangeDirectoryCommand(void)
{
}

CoreChangeDirectoryCommand& CoreChangeDirectoryCommand::operator=(const CoreChangeDirectoryCommand& copyMe)
{
   m_newPath = copyMe.m_newPath;
   m_oldPath = copyMe.m_oldPath;
   m_pModel = copyMe.m_pModel;

   return *this;
}

bool CoreChangeDirectoryCommand::operator==(const CoreChangeDirectoryCommand& rhs) const
{
   return m_newPath == rhs.m_newPath && m_oldPath == rhs.m_oldPath && m_pModel == rhs.m_pModel;
}

bool CoreChangeDirectoryCommand::operator!=(const CoreChangeDirectoryCommand& rhs) const
{
   return m_newPath != rhs.m_newPath || m_oldPath != rhs.m_oldPath || m_pModel != rhs.m_pModel;
}

CoreModel& CoreChangeDirectoryCommand::model() const
{
   return *m_pModel;
}

CoreSelection& CoreChangeDirectoryCommand::selection() const
{
   return *m_pSelection;
}

bool CoreChangeDirectoryCommand::canUndo()
{
   return m_oldPath.size() > 0;
}

void CoreChangeDirectoryCommand::apply()
{
   m_pModel->setPath(m_newPath);
}

void CoreChangeDirectoryCommand::undo()
{
   m_pModel->setPath(m_oldPath);
}

///////////////////////////////////////////////////////////////////////////////
// CoreImageListSelection
///////////////////////////////////////////////////////////////////////////////

CoreImageListSelection::CoreImageListSelection(std::list<HString>& imagePaths)
   : m_imagePaths (imagePaths)
{
}

CoreImageListSelection::~CoreImageListSelection(void)
{
}

std::list<HString> CoreImageListSelection::imagePaths() const
{
   return m_imagePaths;
}

CoreImageListSelection& CoreImageListSelection::operator=(const CoreImageListSelection& copyMe)
{
   m_imagePaths = copyMe.m_imagePaths;

   return *this;
}

bool CoreImageListSelection::operator==(const CoreImageListSelection& rhs) const
{
   return m_imagePaths == rhs.m_imagePaths;
}

bool CoreImageListSelection::operator!=(const CoreImageListSelection& rhs) const
{
   return m_imagePaths != rhs.m_imagePaths;
}