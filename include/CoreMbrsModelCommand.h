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

   // Operations
   CoreImageListModel& operator=(const CoreImageListModel& copyMe) = delete;

   // Commands
   void setPath(const HString& path);

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

#endif // COREMBRSMODELCOMMAND_INCLUDED
