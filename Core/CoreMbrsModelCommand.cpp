/////////////////////////////////////////
// CoreModelCommand.cpp
// Copyright (c) 2020 TXPCo Ltd
/////////////////////////////////////////

#include "Common.h"

#include "CoreFile.h"
#include "CoreModelCommand.h"
#include "CoreMbrsModelCommand.h"
#include "CoreCompoundName.h"
#include "CoreTimeUtil.h"

using namespace std;


///////////////////////////////////////////////////////////////////////////////
// CoreImageListModel
///////////////////////////////////////////////////////////////////////////////


CoreImageListModel::CoreImageListModel()
   : m_path(CoreFileSystemEntity::loadImageDirectory())
{
   m_filter.load();
   refreshImageList();
}

CoreImageListModel::CoreImageListModel(const HString& path)
   : m_path (path)
{
   m_filter.load();
   refreshImageList();
}

CoreImageListModel::~CoreImageListModel(void)
{
}

const HString CoreImageListModel::path() const
{
   return m_path;
}

const HString CoreImageListModel::imageSpecAsUIString() const
{
   HString formatted;
   CoreWindowsPathParser parser;
   CoreUIPathFormatter formatter;

   CoreCompoundName compound (parser.parsePath(m_path));
   return formatter.formatPath(compound) + H_TEXT(", ") + m_filter.asUIString() + H_TEXT(".");
}

CoreDateFilter CoreImageListModel::filter() const
{
   return m_filter;
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
      gmtime_s(&tm, &t);
      if (tm.tm_year + 1900 == year && tm.tm_mon + 1  == month) // gmtime_s return years since 1900, months are zero based
         images.push_back(image);
   }
   return images;
}

const std::vector<CoreFileSystemEntity> CoreImageListModel::filteredImages() const
{
   struct tm tm;
   time_t t = to_time_t(m_filter.date());
   gmtime_s(&tm, &t);

   switch (m_filter.period())
   {
   case CoreDateFilter::kMonth:
      return imagesWrittenIn(tm.tm_year + 1900, tm.tm_mon + 1);

   case CoreDateFilter::kYear:
      return imagesWrittenIn(tm.tm_year + 1900);

   case CoreDateFilter::kDay:
   case CoreDateFilter::kNone:
   default:
      return images();
   }
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

const std::list<HString> CoreImageListModel::actualAddTagsFor(const HString& path, std::list<HString>& add) const
{
   CoreImageListModel* _this = const_cast<CoreImageListModel*>(this);
   CoreImageFile enrichedImage = _this->lookupEnrichedImage(path);

   return enrichedImage.actualAddSubjectTags(add);
}

const std::list<HString> CoreImageListModel::actualRemoveTagsFor(const HString& path, std::list<HString>& remove) const
{
   CoreImageListModel* _this = const_cast<CoreImageListModel*>(this);
   CoreImageFile enrichedImage = _this->lookupEnrichedImage(path);

   return enrichedImage.actualRemoveSubjectTags(remove);
}

void CoreImageListModel::setPath(const HString& path)
{
   m_path = path;
   CoreFileSystemEntity::saveImageDirectory(path);
   refreshImageList();
}

void CoreImageListModel::setFilterPeriod(CoreDateFilter::EPeriod period)
{
   m_filter = CoreDateFilter(m_filter.date(), period);
   m_filter.save();
}

void CoreImageListModel::setFilterDate(const std::chrono::system_clock::time_point& date)
{
   m_filter = CoreDateFilter(date, m_filter.period());
   m_filter.save();
}

void CoreImageListModel::addTag(const HString& path, const HString& tag)
{
   CoreImageFile enrichedImage = lookupEnrichedImage(path);
   list<HString> add = { tag };
   enrichedImage.addSubjectTags(add);
   enrichedImage.writeSubjectTags();

   // Save it back in the cache
   refreshEnrichedImage(path, enrichedImage);
}

void CoreImageListModel::removeTag(const HString& path, const HString& tag)
{
   CoreImageFile enrichedImage = lookupEnrichedImage(path);
   list<HString> remove = { tag };
   enrichedImage.removeSubjectTags(remove);
   enrichedImage.writeSubjectTags();

   // Save it back in the cache
   refreshEnrichedImage(path, enrichedImage);
}

void CoreImageListModel::addRemoveTags(const HString& path, const std::list<HString>& tagsToAdd, const std::list<HString>& tagsToRemove)
{
   CoreImageFile enrichedImage = lookupEnrichedImage(path);
   enrichedImage.addSubjectTags(tagsToAdd);
   enrichedImage.removeSubjectTags(tagsToRemove);
   enrichedImage.writeSubjectTags();

   // Save it back in the cache
   refreshEnrichedImage(path, enrichedImage);
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

CoreImageListSelection::CoreImageListSelection(std::vector<HString>& imagePaths)
   : m_imagePaths (imagePaths)
{
}

CoreImageListSelection::~CoreImageListSelection(void)
{
}

std::vector<HString> CoreImageListSelection::imagePaths() const
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
   m_changeTag(newTag), m_pModel(pModel), m_pSelection(pSelection)
{
}

CoreAddImageTagCommand::~CoreAddImageTagCommand(void)
{
}

CoreAddImageTagCommand& CoreAddImageTagCommand::operator=(const CoreAddImageTagCommand& copyMe)
{
   m_changeTag = copyMe.m_changeTag;
   m_pModel = copyMe.m_pModel;
   m_pSelection = copyMe.m_pSelection;
   m_paths = copyMe.m_paths;

   return *this;
}

bool CoreAddImageTagCommand::operator==(const CoreAddImageTagCommand& rhs) const
{
   return m_changeTag == rhs.m_changeTag && m_pModel == rhs.m_pModel && m_pSelection == rhs.m_pSelection && m_paths == rhs.m_paths;
}

bool CoreAddImageTagCommand::operator!=(const CoreAddImageTagCommand& rhs) const
{
   return m_changeTag != rhs.m_changeTag || m_pModel != rhs.m_pModel || m_pSelection != rhs.m_pSelection || m_paths != rhs.m_paths;
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
   return m_paths.size() > 0;
}

void CoreAddImageTagCommand::apply()
{
   std::vector<HString> pathsWithoutTag;

   for (auto path : m_pSelection->imagePaths())
   {
      if (! m_pModel->doesImageHaveTag (path, m_changeTag))
         pathsWithoutTag.push_back (path);
   }

   m_paths = pathsWithoutTag;

   applyTo(pathsWithoutTag);
}

void CoreAddImageTagCommand::undo()
{
   unApplyTo(m_paths);
}

void CoreAddImageTagCommand::applyTo(const std::vector< HString >& paths)
{
   for (auto path : paths)
      m_pModel->addTag(path, m_changeTag);
}

void CoreAddImageTagCommand::unApplyTo(const std::vector< HString >& paths)
{
   for (auto path : paths)
      m_pModel->removeTag(path, m_changeTag);
}

///////////////////////////////////////////////////////////////////////////////
// CoreRemoveImageTagCommand
///////////////////////////////////////////////////////////////////////////////

CoreRemoveImageTagCommand::CoreRemoveImageTagCommand(const HString& oldTag,
   std::shared_ptr< CoreImageListModel> pModel, std::shared_ptr< CoreImageListSelection> pSelection)
   : CoreCommand(),
   m_changeTag(oldTag), m_pModel(pModel), m_pSelection(pSelection)
{
}

CoreRemoveImageTagCommand::~CoreRemoveImageTagCommand(void)
{
}

CoreRemoveImageTagCommand& CoreRemoveImageTagCommand::operator=(const CoreRemoveImageTagCommand& copyMe)
{
   m_changeTag = copyMe.m_changeTag;
   m_pModel = copyMe.m_pModel;
   m_pSelection = copyMe.m_pSelection;
   m_paths = copyMe.m_paths;

   return *this;
}

bool CoreRemoveImageTagCommand::operator==(const CoreRemoveImageTagCommand& rhs) const
{
   return m_changeTag == rhs.m_changeTag && m_pModel == rhs.m_pModel && m_pSelection == rhs.m_pSelection && m_paths == rhs.m_paths;
}

bool CoreRemoveImageTagCommand::operator!=(const CoreRemoveImageTagCommand& rhs) const
{
   return m_changeTag != rhs.m_changeTag || m_pModel != rhs.m_pModel || m_pSelection != rhs.m_pSelection || m_paths != rhs.m_paths;
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
   return m_paths.size() > 0;
}

void CoreRemoveImageTagCommand::apply()
{
   std::vector<HString> pathsWithTag;

   for (auto path : m_pSelection->imagePaths())
   {
      if (m_pModel->doesImageHaveTag(path, m_changeTag))
         pathsWithTag.push_back(path);
   }

   m_paths = pathsWithTag;

   applyTo(pathsWithTag);
}

void CoreRemoveImageTagCommand::undo()
{
   unApplyTo(m_paths);
}

void CoreRemoveImageTagCommand::applyTo(const std::vector< HString >& paths)
{
   for (auto path : paths)
      m_pModel->removeTag(path, m_changeTag);
}

void CoreRemoveImageTagCommand::unApplyTo(const std::vector< HString >& paths)
{
   for (auto path : paths)
      m_pModel->addTag(path, m_changeTag);
}

///////////////////////////////////////////////////////////////////////////////
// CoreCompoundImageTagChangeCommand
///////////////////////////////////////////////////////////////////////////////

CoreCompoundImageTagChangeCommand::CoreCompoundImageTagChangeCommand(const list<HString>& add,
                                                                     const list<HString>& remove,
                                                                     std::shared_ptr< CoreImageListModel> pModel,
                                                                     std::shared_ptr< CoreImageListSelection> pSelection)
   : CoreCommand(),
   m_addTags(add), m_removeTags (remove), m_pModel(pModel), m_pSelection(pSelection)
{
}

CoreCompoundImageTagChangeCommand::~CoreCompoundImageTagChangeCommand(void)
{
}

CoreCompoundImageTagChangeCommand& CoreCompoundImageTagChangeCommand::operator=(const CoreCompoundImageTagChangeCommand& copyMe)
{
   m_addTags = copyMe.m_addTags;
   m_removeTags = copyMe.m_removeTags;
   m_pModel = copyMe.m_pModel;
   m_pSelection = copyMe.m_pSelection;
   m_paths = copyMe.m_paths;

   return *this;
}

bool CoreCompoundImageTagChangeCommand::operator==(const CoreCompoundImageTagChangeCommand& rhs) const
{
   return m_addTags == rhs.m_addTags && rhs.m_removeTags == rhs.m_removeTags 
      && m_pModel == rhs.m_pModel && m_pSelection == rhs.m_pSelection && m_paths == rhs.m_paths;
}

bool CoreCompoundImageTagChangeCommand::operator!=(const CoreCompoundImageTagChangeCommand& rhs) const
{
   return m_addTags != rhs.m_addTags || m_removeTags != rhs.m_removeTags ||
      m_pModel != rhs.m_pModel || m_pSelection != rhs.m_pSelection || m_paths != rhs.m_paths;
}

CoreModel& CoreCompoundImageTagChangeCommand::model() const
{
   return *m_pModel;
}

CoreSelection& CoreCompoundImageTagChangeCommand::selection() const
{
   return *m_pSelection;
}

bool CoreCompoundImageTagChangeCommand::canUndo()
{
   return m_paths.size() > 0;
}

void CoreCompoundImageTagChangeCommand::apply()
{
   // when the command is applied, we calculate the actual changes to make to each image & save for the undo list
   m_paths = m_pSelection->imagePaths();
   m_actualAddTagLists.resize(m_paths.size());
   m_actualRemoveTagLists.resize(m_paths.size());

   for (auto i = 0; i < m_paths.size(); i++)
   {
      auto path = m_paths[i];
      m_actualAddTagLists[i] = m_pModel->actualAddTagsFor (path, m_addTags);
      m_actualRemoveTagLists[i] = m_pModel->actualRemoveTagsFor (path, m_removeTags);
   }
   applyTo(m_paths);
}

void CoreCompoundImageTagChangeCommand::undo()
{
   unApplyTo(m_paths);
}

void CoreCompoundImageTagChangeCommand::applyTo(const std::vector< HString >& paths)
{
   for (auto i = 0; i < m_paths.size(); i++)
   {
      auto path = m_paths[i];
      m_pModel->addRemoveTags(path, m_actualAddTagLists[i], m_actualRemoveTagLists[i]);
   }
}

void CoreCompoundImageTagChangeCommand::unApplyTo(const std::vector< HString >& paths)
{
   // Undo the command by adding the remove list, and vice-versa
   for (auto i = 0; i < m_paths.size(); i++)
   {
      auto path = m_paths[i];
      m_pModel->addRemoveTags(path, m_actualRemoveTagLists[i], m_actualAddTagLists[i]);
   }
}