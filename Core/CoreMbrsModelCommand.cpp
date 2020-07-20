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
   trimFilterDate();
}

CoreImageListModel::CoreImageListModel(const HString& path)
   : m_path (path)
{
   m_filter.load();
   refreshImageList();
   trimFilterDate();
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
      time_s (&tm, &t);
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
      time_s(&tm, &t);
      if (tm.tm_year + 1900 == year && tm.tm_mon + 1  == month) // time_s return years since 1900, months are zero based
         images.push_back(image);
   }
   return images;
}

const std::vector<CoreFileSystemEntity> CoreImageListModel::filteredImages() const
{
   struct tm tm;
   time_t t = to_time_t(m_filter.date());
   time_s(&tm, &t);

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

void CoreImageListModel::removeImage(const HString& path)
{
   map<HString, CoreImageFile>::iterator iter = m_enrichedImages.find(path);
   if (iter != m_enrichedImages.end())
      m_enrichedImages.erase(iter);

   bool found = false;
   for (vector<CoreFileSystemEntity>::iterator iter2 = m_images.begin(); 
      iter2 != m_images.end() && !found; iter2++)
   {
      if ((*iter2).path() == path)
      {
         m_images.erase(iter2);
         found = true;
         break;
      }
   }
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

void CoreImageListModel::path(const HString& path)
{
   m_path = path;
   CoreFileSystemEntity::saveImageDirectory(path);
   refreshImageList();
   trimFilterDate();
}

void CoreImageListModel::filterPeriod(CoreDateFilter::EPeriod period)
{
   m_filter = CoreDateFilter(m_filter.date(), period);
   m_filter.save();
}

void CoreImageListModel::filterDate(const std::chrono::system_clock::time_point& date)
{
   m_filter = CoreDateFilter(date, m_filter.period());
   m_filter.save();
}

bool CoreImageListModel::addTag(const HString& path, const HString& tag)
{
   bool found = false;
   CoreImageFile enrichedImage = lookupEnrichedImage(path);
   list<HString> add = { tag };
   enrichedImage.addSubjectTags(add);
   enrichedImage.writeSubjectTags();

   if (enrichedImage.writeSubjectTags())
   {
      // Save it back in the cache
      refreshEnrichedImage(path, enrichedImage);
      found = true;
   }
   else
   {
      // If there was an error, remove from memory
      removeImage(path);
   }
   return found;
}

bool CoreImageListModel::removeTag(const HString& path, const HString& tag)
{
   bool found = false;
   CoreImageFile enrichedImage = lookupEnrichedImage(path);
   list<HString> remove = { tag };
   enrichedImage.removeSubjectTags(remove);
   enrichedImage.writeSubjectTags();

   if (enrichedImage.writeSubjectTags())
   {
      // Save it back in the cache
      refreshEnrichedImage(path, enrichedImage);
      found = true;
   }
   else
   {
      // If there was an error, remove from memory
      removeImage(path);
   }
   return found;
}

bool CoreImageListModel::addRemoveTags(const HString& path, const std::list<HString>& tagsToAdd, const std::list<HString>& tagsToRemove)
{
   bool found = false;
   CoreImageFile enrichedImage = lookupEnrichedImage(path);
   enrichedImage.addSubjectTags(tagsToAdd);
   enrichedImage.removeSubjectTags(tagsToRemove);
   
   if (enrichedImage.writeSubjectTags())
   {
      // Save it back in the cache
      refreshEnrichedImage(path, enrichedImage);
      found = true;
   }
   else
   {
      // If there was an error, remove from memory
      removeImage(path);
   }
   return found;
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

void CoreImageListModel::trimFilterDate()
{
   std::chrono::system_clock::time_point tpFilter = m_filter.date();

   if (m_images.size() > 0)
   {
      // Check that filter is within bounds of images we have 
      std::filesystem::file_time_type tpFile0 = m_images[0].lastWriteTime();
      std::filesystem::file_time_type tpFileN = m_images[m_images.size() - 1].lastWriteTime();
      std::time_t timeFile0 = to_time_t(tpFile0);
      std::time_t timeFileN = to_time_t(tpFileN);
      std::time_t timeFilter = to_time_t(tpFilter);

      if (timeFilter > timeFile0)
      {
         chrono::system_clock::time_point tpNew = chrono::system_clock::from_time_t(timeFile0);
         m_filter = CoreDateFilter(tpNew, m_filter.period());
      }
      else
      if (timeFilter < timeFileN)
      {
         chrono::system_clock::time_point tpNew = chrono::system_clock::from_time_t(timeFileN);
         m_filter = CoreDateFilter(tpNew, m_filter.period());
      }
   }
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
   return m_addTags == rhs.m_addTags && m_removeTags == rhs.m_removeTags 
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

bool CoreCompoundImageTagChangeCommand::apply()
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
   return applyTo(m_paths);
}

bool CoreCompoundImageTagChangeCommand::undo()
{
   return unApplyTo(m_paths);
}

bool CoreCompoundImageTagChangeCommand::applyTo(const std::vector< HString >& paths)
{
   bool ok = true;

   for (auto i = 0; i < m_paths.size(); i++)
   {
      auto path = m_paths[i];
      if (!m_pModel->addRemoveTags(path, m_actualAddTagLists[i], m_actualRemoveTagLists[i]))
         ok = false;
   }
   return ok;
}

bool CoreCompoundImageTagChangeCommand::unApplyTo(const std::vector< HString >& paths)
{
   bool ok = true;

   // Undo the command by adding the remove list, and vice-versa
   for (auto i = 0; i < m_paths.size(); i++)
   {
      auto path = m_paths[i];
      if (!m_pModel->addRemoveTags(path, m_actualRemoveTagLists[i], m_actualAddTagLists[i]))
         ok = false;
   }
   return ok;
}