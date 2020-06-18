/////////////////////////////////////////
// CoreModelCommand.cpp
// Copyright (c) 2020 TXPCo Ltd
/////////////////////////////////////////

#include "Common.h"

#include "CoreFile.h"
#include "CoreModelCommand.h"
#include "CoreMbrsModelCommand.h"
#include "CoreCompoundName.h"

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

const HString CoreImageListModel::pathAsUserString() const
{
   HString formatted;
   CoreWindowsPathParser parser;
   CoreUIPathFormatter formatter;

   CoreCompoundName compound (parser.parsePath(m_path));
   return formatter.formatPath(compound);
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

bool CoreImageListModel::doesImageHaveTag(const HString& path, const HString& tag) const
{
   for (auto image : m_images)
   {
      if (image.path() == path)
      {
         std::list<HString> tags = image.subjectTags();

         if (find(tags.begin(), tags.end(), tag) != tags.end())
            return true;
         else
            return false;
      }
   }

   return false;
}

void CoreImageListModel::setPath(const HString& path)
{
   m_path = path;
   CoreFileSystemEntity::saveImageDirectory(path);
   refreshImageList();
}

void CoreImageListModel::addTag(const HString& path, const HString& tag)
{
   list<CoreImageFile>::iterator iter;

   for (iter = m_images.begin(); iter != m_images.end(); iter++)
   {
      if ((*iter).path() == path)
      {
         list<HString> add = { tag };
         (*iter).addSubjectTags(add);
         (*iter).writeSubjectTags();
         return;
      }
   }
}

void CoreImageListModel::removeTag(const HString& path, const HString& tag)
{
   list<CoreImageFile>::iterator iter;

   for (iter = m_images.begin(); iter != m_images.end(); iter++)
   {
      if ((*iter).path() == path)
      {
         list<HString> remove = { tag };
         (*iter).removeSubjectTags(remove);
         (*iter).writeSubjectTags();
         return;
      }
   }
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
   m_pSelection = copyMe.m_pSelection;

   return *this;
}

bool CoreChangeDirectoryCommand::operator==(const CoreChangeDirectoryCommand& rhs) const
{
   return m_newPath == rhs.m_newPath && m_oldPath == rhs.m_oldPath && m_pModel == rhs.m_pModel && m_pSelection == rhs.m_pSelection;
}

bool CoreChangeDirectoryCommand::operator!=(const CoreChangeDirectoryCommand& rhs) const
{
   return m_newPath != rhs.m_newPath || m_oldPath != rhs.m_oldPath || m_pModel != rhs.m_pModel || m_pSelection != rhs.m_pSelection;
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

///////////////////////////////////////////////////////////////////////////////
// CoreAddImageTagCommand
///////////////////////////////////////////////////////////////////////////////

CoreAddImageTagCommand::CoreAddImageTagCommand (const HString& newTag, 
   std::shared_ptr< CoreImageListModel> pModel, std::shared_ptr< CoreImageListSelection> pSelection)
   : CoreCommand(),
   m_newTag(newTag), m_pModel(pModel), m_pSelection(pSelection)
{
}

CoreAddImageTagCommand::~CoreAddImageTagCommand(void)
{
}

CoreAddImageTagCommand& CoreAddImageTagCommand::operator=(const CoreAddImageTagCommand& copyMe)
{
   m_newTag = copyMe.m_newTag;
   m_pModel = copyMe.m_pModel;
   m_pSelection = copyMe.m_pSelection;
   m_listForUndo = copyMe.m_listForUndo;

   return *this;
}

bool CoreAddImageTagCommand::operator==(const CoreAddImageTagCommand& rhs) const
{
   return m_newTag == rhs.m_newTag && m_pModel == rhs.m_pModel && m_pSelection == rhs.m_pSelection && m_listForUndo == rhs.m_listForUndo;
}

bool CoreAddImageTagCommand::operator!=(const CoreAddImageTagCommand& rhs) const
{
   return m_newTag != rhs.m_newTag || m_pModel != rhs.m_pModel || m_pSelection != rhs.m_pSelection || m_listForUndo != rhs.m_listForUndo;
}

CoreModel& CoreAddImageTagCommand::model() const
{
   return *m_pModel;
}

CoreSelection& CoreAddImageTagCommand::selection() const
{
   return *m_pSelection;
}

bool CoreAddImageTagCommand::canUndo()
{
   return m_listForUndo.size() > 0;
}

void CoreAddImageTagCommand::apply()
{
   std::list<HString> pathsWithoutTag;

   for (auto path : m_pSelection->imagePaths())
   {
      if (! m_pModel->doesImageHaveTag (path, m_newTag))
         pathsWithoutTag.push_back (path);
   }

   m_listForUndo = pathsWithoutTag;

   applyTo(pathsWithoutTag);
}

void CoreAddImageTagCommand::undo()
{
   unApplyTo(m_listForUndo);
}

void CoreAddImageTagCommand::applyTo(const std::list< HString >& paths)
{
   for (auto path : paths)
      m_pModel->addTag(path, m_newTag);
}

void CoreAddImageTagCommand::unApplyTo(const std::list< HString >& paths)
{
   for (auto path : paths)
      m_pModel->removeTag(path, m_newTag);
}

///////////////////////////////////////////////////////////////////////////////
// CoreRemoveImageTagCommand
///////////////////////////////////////////////////////////////////////////////

CoreRemoveImageTagCommand::CoreRemoveImageTagCommand(const HString& oldTag,
   std::shared_ptr< CoreImageListModel> pModel, std::shared_ptr< CoreImageListSelection> pSelection)
   : CoreCommand(),
   m_oldTag(oldTag), m_pModel(pModel), m_pSelection(pSelection)
{
}

CoreRemoveImageTagCommand::~CoreRemoveImageTagCommand(void)
{
}

CoreRemoveImageTagCommand& CoreRemoveImageTagCommand::operator=(const CoreRemoveImageTagCommand& copyMe)
{
   m_oldTag = copyMe.m_oldTag;
   m_pModel = copyMe.m_pModel;
   m_pSelection = copyMe.m_pSelection;
   m_listForUndo = copyMe.m_listForUndo;

   return *this;
}

bool CoreRemoveImageTagCommand::operator==(const CoreRemoveImageTagCommand& rhs) const
{
   return m_oldTag == rhs.m_oldTag && m_pModel == rhs.m_pModel && m_pSelection == rhs.m_pSelection && m_listForUndo == rhs.m_listForUndo;
}

bool CoreRemoveImageTagCommand::operator!=(const CoreRemoveImageTagCommand& rhs) const
{
   return m_oldTag != rhs.m_oldTag || m_pModel != rhs.m_pModel || m_pSelection != rhs.m_pSelection || m_listForUndo != rhs.m_listForUndo;
}

CoreModel& CoreRemoveImageTagCommand::model() const
{
   return *m_pModel;
}

CoreSelection& CoreRemoveImageTagCommand::selection() const
{
   return *m_pSelection;
}

bool CoreRemoveImageTagCommand::canUndo()
{
   return m_listForUndo.size() > 0;
}

void CoreRemoveImageTagCommand::apply()
{
   std::list<HString> pathsWithTag;

   for (auto path : m_pSelection->imagePaths())
   {
      if (m_pModel->doesImageHaveTag(path, m_oldTag))
         pathsWithTag.push_back(path);
   }

   m_listForUndo = pathsWithTag;

   applyTo(pathsWithTag);
}

void CoreRemoveImageTagCommand::undo()
{
   unApplyTo(m_listForUndo);
}

void CoreRemoveImageTagCommand::applyTo(const std::list< HString >& paths)
{
   for (auto path : paths)
      m_pModel->removeTag(path, m_oldTag);
}

void CoreRemoveImageTagCommand::unApplyTo(const std::list< HString >& paths)
{
   for (auto path : paths)
      m_pModel->addTag(path, m_oldTag);
}