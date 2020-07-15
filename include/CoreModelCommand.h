/////////////////////////////////////////
// CoreModelCommand.h
// Copyright (c) 2020 TXPCo Ltd
/////////////////////////////////////////

#ifndef COREMODELCOMMAND_INCLUDED
#define COREMODELCOMMAND_INCLUDED

#include "Common.h"
#include "Host.h"
#include "HostException.h"
#include "HostLocks.h"
#include "Core.h"
#include "CoreQueue.h"

///////////////////////////////////////////////////////////////////////////////
// CoreModel
///////////////////////////////////////////////////////////////////////////////

class CORE_API CoreModel
{
public :
// Constructors
   CoreModel();
   virtual ~CoreModel (void);
   
   
// Operations
   CoreModel& operator=(const CoreModel& copyMe) = delete;

protected :

private :

};

///////////////////////////////////////////////////////////////////////////////
// CoreSelection
///////////////////////////////////////////////////////////////////////////////

class CORE_API CoreSelection
{
public:
   // Constructors
   CoreSelection();
   virtual ~CoreSelection(void);


   // Operations
   CoreSelection& operator=(const CoreSelection& copyMe) = delete;

protected:

private:

};

///////////////////////////////////////////////////////////////////////////////
// CoreCommand
///////////////////////////////////////////////////////////////////////////////

class CORE_API CoreCommand
{
public:
   // Constructors
   CoreCommand();
   virtual ~CoreCommand(void);

   // Attributes
   virtual bool canUndo() = 0;
   virtual CoreModel& model() const = 0;
   virtual CoreSelection& selection() const = 0;

   // Operations
   virtual bool apply() = 0;
   virtual bool undo() = 0;

protected:

private:
};

///////////////////////////////////////////////////////////////////////////////
// CoreCommandProcessor
///////////////////////////////////////////////////////////////////////////////

class CORE_API CoreCommandProcessor
{
public:
   // Constructors
   CoreCommandProcessor(std::shared_ptr<CoreModel> pModel);
   virtual ~CoreCommandProcessor(void);

   // Attributes
   std::shared_ptr<CoreModel> model() const;

   // Operations
   CoreCommandProcessor& operator=(const CoreCommandProcessor& copyMe);

   bool operator==(const CoreCommandProcessor& rhs) const;
   bool operator!=(const CoreCommandProcessor& rhs) const;
   bool adoptAndDo(std::shared_ptr<CoreCommand> pCommand);
   bool canUndo();
   bool undo();
   bool canRedo();
   bool redo();

protected:

private:
   std::shared_ptr<CoreModel>  m_pModel;
   std::list<std::shared_ptr<CoreCommand>> m_commands;
   std::list<std::shared_ptr<CoreCommand>>::iterator m_lastDone;
};

#endif // COREMODELCOMMAND_INCLUDED
