/////////////////////////////////////////
// CoreModelCommand.cpp
// Copyright (c) 2020 TXPCo Ltd
/////////////////////////////////////////

#include "CorePrecompile.h"
#include "CoreModelCommand.h"
#include "CoreMbrsModelCommand.h"

using namespace std;

///////////////////////////////////////////////////////////////////////////////
// CoreImageListModel
///////////////////////////////////////////////////////////////////////////////

CoreImageListModel::CoreImageListModel()
{
}

CoreImageListModel::CoreImageListModel(const HString& path)
   : m_path (path)
{
   refreshImageList();
}

CoreImageListModel::~CoreImageListModel(void)
{
}


HString CoreImageListModel::path() const
{
   return m_path;
}

void CoreImageListModel::setPath(const HString& path)
{
   m_path = path;
   refreshImageList();
}

void CoreImageListModel::refreshImageList()
{
   // TODO
}

///////////////////////////////////////////////////////////////////////////////
// CoreChangeDirectoryCommand
///////////////////////////////////////////////////////////////////////////////

CoreChangeDirectoryCommand::CoreChangeDirectoryCommand(const HString& newPath, const HString& oldPath, std::shared_ptr< CoreImageListModel> pModel)
   : m_newPath (newPath), m_oldPath (oldPath), m_pModel (pModel)
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
