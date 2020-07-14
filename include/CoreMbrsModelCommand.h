/////////////////////////////////////////
// CoreModelCommand.h
// Copyright (c) 2020 TXPCo Ltd
/////////////////////////////////////////

#ifndef COREMBRSMODELCOMMAND_INCLUDED
#define COREMBRSMODELCOMMAND_INCLUDED

#include "Common.h"
#include "Host.h"
#include "HostException.h"
#include "HostLocks.h"
#include "Core.h"
#include "CoreQueue.h"
#include "CoreModelCommand.h"
#include "CoreImageFile.h"
#include "CoreDateFilter.h"

///////////////////////////////////////////////////////////////////////////////
// CoreImageListModel
///////////////////////////////////////////////////////////////////////////////

class CORE_API CoreImageListModel : public CoreModel
{
public:
   // Constructors
   CoreImageListModel();
   CoreImageListModel(const HString& path);
   virtual ~CoreImageListModel(void);

   // Attributes
   const HString path () const;
   const HString imageSpecAsUIString () const;
   CoreDateFilter filter () const;
   const std::vector<CoreFileSystemEntity> images() const;
   const std::vector<CoreFileSystemEntity> imagesWrittenIn (HInt year) const;
   const std::vector<CoreFileSystemEntity> imagesWrittenIn (HInt year, HInt month) const;
   const std::vector<CoreFileSystemEntity> filteredImages() const;
   bool doesImageHaveTag(const HString& path, const HString& tag) const;
   const std::list<HString> tagsFor (const HString& path) const;
   const std::list<HString> actualAddTagsFor(const HString& path, std::list<HString>& add) const;
   const std::list<HString> actualRemoveTagsFor(const HString& path, std::list<HString>& remove) const;

   // Operations
   CoreImageListModel& operator=(const CoreImageListModel& copyMe) = delete;

   // Commands
   void setPath(const HString& path);
   void setFilterPeriod (CoreDateFilter::EPeriod period);
   void setFilterDate (const std::chrono::system_clock::time_point& date);
   bool addTag(const HString& path, const HString& tag);
   bool removeTag(const HString& path, const HString& tag);
   bool addRemoveTags(const HString& path, const std::list<HString>& tagsToAdd, const std::list<HString>& tagsToRemove);

protected:
   void refreshImageList ();

private:
   CoreImageFile lookupEnrichedImage (const HString& path);
   bool refreshEnrichedImage(const HString& path, const CoreImageFile& file);
   void removeImage(const HString& path);
   
   HString m_path;
   CoreDateFilter         m_filter;
   std::vector<CoreFileSystemEntity> m_images;
   std::map<HString, CoreImageFile>  m_enrichedImages;
};


///////////////////////////////////////////////////////////////////////////////
// CoreImageListSelection
///////////////////////////////////////////////////////////////////////////////

class CORE_API CoreImageListSelection : public CoreSelection
{
public:
   // Constructors
   CoreImageListSelection(std::vector<HString>& imagePaths);
   virtual ~CoreImageListSelection(void);

   // Attributes
   std::vector<HString> imagePaths() const;

   // Operations
   CoreImageListSelection& operator=(const CoreImageListSelection& copyMe);
   bool operator==(const CoreImageListSelection& rhs) const;
   bool operator!=(const CoreImageListSelection& rhs) const;

protected:

private:
   std::vector<HString> m_imagePaths;
};

///////////////////////////////////////////////////////////////////////////////
// CoreCompoundImageTagChangeCommand
///////////////////////////////////////////////////////////////////////////////

class CORE_API CoreCompoundImageTagChangeCommand : public CoreCommand
{
public:
   // Constructors
   CoreCompoundImageTagChangeCommand(const std::list<HString>& add,
                                     const std::list<HString>& remove,
                                     std::shared_ptr< CoreImageListModel> pModel,
                                     std::shared_ptr< CoreImageListSelection> pSelection);
   virtual ~CoreCompoundImageTagChangeCommand();

   // Attributes
   virtual bool canUndo();
   virtual CoreModel& model() const;
   virtual CoreSelection& selection() const;

   // Operations

   CoreCompoundImageTagChangeCommand& operator=(const CoreCompoundImageTagChangeCommand& copyMe);
   bool operator==(const CoreCompoundImageTagChangeCommand& rhs) const;
   bool operator!=(const CoreCompoundImageTagChangeCommand& rhs) const;
   virtual bool apply();
   virtual bool undo();

protected:

private:
   bool applyTo(const std::vector< HString >& paths);
   bool unApplyTo(const std::vector< HString >& paths);

   std::shared_ptr<CoreImageListModel> m_pModel;
   std::shared_ptr<CoreImageListSelection> m_pSelection;
   std::list< HString > m_addTags, m_removeTags;
   std::vector<HString> m_paths;
   std::vector< std::list<HString> > m_actualAddTagLists, m_actualRemoveTagLists;
};

#endif // COREMBRSMODELCOMMAND_INCLUDED
