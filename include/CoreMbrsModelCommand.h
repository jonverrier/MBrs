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
   const std::list<CoreImageFile> images() const;
   const std::list<CoreImageFile> imagesFor (HInt year) const;
   const std::list<CoreImageFile> imagesFor (HInt year, HInt month) const;
   bool doesImageHaveTag(const HString& path, const HString& tag) const;

   // Operations
   CoreImageListModel& operator=(const CoreImageListModel& copyMe) = delete;

   // Commands
   void setPath(const HString& path);
   void addTag(const HString& path, const HString& tag);
   void removeTag(const HString& path, const HString& tag);

protected:
   void refreshImageList ();

private:
   HString m_path;
   std::list<CoreImageFile> m_images;
};

///////////////////////////////////////////////////////////////////////////////
// CoreChangeDirectoryCommand
///////////////////////////////////////////////////////////////////////////////

class CORE_API CoreChangeDirectoryCommand : public CoreCommand
{
public:
   // Constructors
   CoreChangeDirectoryCommand(const HString& newPath, const HString& oldPath, 
                              std::shared_ptr< CoreImageListModel> pModel, std::shared_ptr< CoreSelection> pSelection);
   virtual ~CoreChangeDirectoryCommand();

   // Attributes
   virtual bool canUndo();
   virtual CoreModel& model() const;
   virtual CoreSelection& selection() const;

   // Operations

   CoreChangeDirectoryCommand& operator=(const CoreChangeDirectoryCommand& copyMe);
   bool operator==(const CoreChangeDirectoryCommand& rhs) const;   
   bool operator!=(const CoreChangeDirectoryCommand& rhs) const;
   virtual void apply();
   virtual void undo();

protected:

private:
   std::shared_ptr< CoreImageListModel> m_pModel;
   std::shared_ptr< CoreSelection> m_pSelection;
   HString m_newPath;
   HString m_oldPath;
};


///////////////////////////////////////////////////////////////////////////////
// CoreImageListSelection
///////////////////////////////////////////////////////////////////////////////

class CORE_API CoreImageListSelection : public CoreSelection
{
public:
   // Constructors
   CoreImageListSelection(std::list<HString>& imagePaths);
   virtual ~CoreImageListSelection(void);

   // Attributes
   std::list<HString> imagePaths() const;

   // Operations
   CoreImageListSelection& operator=(const CoreImageListSelection& copyMe);
   bool operator==(const CoreImageListSelection& rhs) const;
   bool operator!=(const CoreImageListSelection& rhs) const;

protected:

private:
   std::list<HString> m_imagePaths;
};

///////////////////////////////////////////////////////////////////////////////
// CoreChangeDirectoryCommand
///////////////////////////////////////////////////////////////////////////////

class CORE_API CoreAddImageTagCommand : public CoreCommand
{
public:
   // Constructors
   CoreAddImageTagCommand(const HString& newTag, 
                          std::shared_ptr< CoreImageListModel> pModel, 
                          std::shared_ptr< CoreImageListSelection> pSelection);
   virtual ~CoreAddImageTagCommand();

   // Attributes
   virtual bool canUndo();
   virtual CoreModel& model() const;
   virtual CoreSelection& selection() const;

   // Operations

   CoreAddImageTagCommand& operator=(const CoreAddImageTagCommand& copyMe);
   bool operator==(const CoreAddImageTagCommand& rhs) const;
   bool operator!=(const CoreAddImageTagCommand& rhs) const;
   virtual void apply();
   virtual void undo();

protected:

private:
   void applyTo (const std::list< HString >& paths);
   void unApplyTo (const std::list< HString >& paths);

   std::shared_ptr<CoreImageListModel> m_pModel;
   std::shared_ptr<CoreImageListSelection> m_pSelection;
   std::list< HString > m_listForUndo;
   HString m_newTag;
};

#endif // COREMBRSMODELCOMMAND_INCLUDED
