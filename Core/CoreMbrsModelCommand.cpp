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

template <typename TP>

std::time_t to_time_t(TP tp)
{
   using namespace std::chrono;
   auto sctp = time_point_cast<system_clock::duration>(tp - TP::clock::now()
      + system_clock::now());
   return system_clock::to_time_t(sctp);
}

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

const vector<CoreFileSystemEntity> CoreImageListModel::images() const
{
   return m_images;
}

const vector<CoreFileSystemEntity> CoreImageListModel::imagesWrittenIn(HInt year) const
{
   vector<CoreFileSystemEntity> images;

   for (auto image : m_images)
   {
      struct tm tm;
      // time_t t = image.takenAt();
      time_t t = to_time_t (image.lastWriteTime());
      localtime_s(&tm, &t);
      if (tm.tm_year + 1900 == year) // localtime return years since 1900 
         images.push_back(image);
   }
   return images;
}

const vector<CoreFileSystemEntity> CoreImageListModel::imagesWrittenIn(HInt year, HInt month) const
{
   vector<CoreFileSystemEntity> images;

   for (auto image : m_images)
   {
      struct tm tm;
      // time_t t = image.takenAt();
      time_t t = to_time_t(image.lastWriteTime());
      localtime_s(&tm, &t);
      if (tm.tm_year + 1900 == year && tm.tm_mon + 1  == month) // localtime return years since 1900, months are zero based
         images.push_back(image);
   }
   return images;
}

bool CoreImageListModel::doesImageHaveTag(const HString& path, const HString& tag) const
{
   CoreImageListModel* _this = const_cast<CoreImageListModel*>(this);
   CoreImageFile enrichedImage = _this->lookupEnrichedImage(path);

   std::list<HString> tags = enrichedImage.subjectTags();

   if (find(tags.begin(), tags.end(), tag) != tags.end())
      return true;
   else
      return false;

   return false;
}

CoreImageFile CoreImageListModel::lookupEnrichedImage(const HString& path) 
{
   map<HString, CoreImageFile>::iterator iter = m_enrichedImages.find(path);
   if (iter != m_enrichedImages.end())
      return (*iter).second;

   CoreImageFile newEntry(path);
   m_enrichedImages.insert(std::pair<HString, CoreImageFile>(path, newEntry));
   return newEntry;
}

bool CoreImageListModel::refreshEnrichedImage(const HString& path, const CoreImageFile& file)
{
   map<HString, CoreImageFile>::iterator iter = m_enrichedImages.find(path);
   if (iter != m_enrichedImages.end())
   {
      (*iter).second = file;
      return true;
   }

   return false;
}

const std::list<HString> CoreImageListModel::tagsFor(const HString& path) const
{
   CoreImageListModel* _this = const_cast<CoreImageListModel*>(this);
   CoreImageFile enrichedImage = _this->lookupEnrichedImage(path);

   return enrichedImage.subjectTags();
}

void CoreImageListModel::setPath(const HString& path)
{
   m_path = path;
   CoreFileSystemEntity::saveImageDirectory(path);
   refreshImageList();
}

void CoreImageListModel::addTag(const HString& path, const HString& tag)
{
   CoreImageListModel* _this = const_cast<CoreImageListModel*>(this);
   CoreImageFile enrichedImage = _this->lookupEnrichedImage(path);
   list<HString> add = { tag };
   enrichedImage.addSubjectTags(add);
   enrichedImage.writeSubjectTags();

   // Save it back in the cache
   _this->refreshEnrichedImage(path, enrichedImage);
}

void CoreImageListModel::removeTag(const HString& path, const HString& tag)
{
   CoreImageListModel* _this = const_cast<CoreImageListModel*>(this);
   CoreImageFile enrichedImage = _this->lookupEnrichedImage(path);
   list<HString> remove = { tag };
   enrichedImage.removeSubjectTags(remove);
   enrichedImage.writeSubjectTags();

   // Save it back in the cache
   _this->refreshEnrichedImage(path, enrichedImage);
}

// order the list by date-time taken, newest first (highest takenAt() 
bool compare (const CoreFileSystemEntity& i, const CoreFileSystemEntity& j)
{ 
   time_t tj = to_time_t(j.lastWriteTime());
   time_t ti = to_time_t(i.lastWriteTime());

   return (tj < ti); 
}

void CoreImageListModel::refreshImageList()
{
   vector<HString> imagePaths;
   CoreDirectory dir(m_path);
   dir.listImages(imagePaths);

   m_images.clear();

   for (auto path : imagePaths)
   {
      CoreFileSystemEntity image(path);
      m_images.push_back (image);
   }

   sort (m_images.begin(), m_images.end(), compare);
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