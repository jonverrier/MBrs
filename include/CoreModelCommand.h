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
   CoreModel&
   operator=(const CoreModel& copyMe) = delete;

protected :

private :

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

   // Operations
   CoreCommand& operator=(const CoreCommand& copyMe) = delete;
   virtual void apply() = 0;
   virtual void undo() = 0;

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

   bool operator==(const CoreCommandProcessor& rhs) const;
   bool operator!=(const CoreCommandProcessor& rhs) const;

   // Operations
   CoreCommandProcessor& operator=(const CoreCommandProcessor& copyMe);
   void adoptAndDo(std::shared_ptr<CoreCommand> pCommand);
   bool canUndo();
   void undo();
   bool canRedo();
   void redo();

protected:

private:
   std::shared_ptr<CoreModel>  m_pModel;
   std::list<std::shared_ptr<CoreCommand>> m_commands;
   std::list<std::shared_ptr<CoreCommand>>::iterator m_lastDone;
};

#endif // COREMODELCOMMAND_INCLUDED
