/////////////////////////////////////////
// CoreModelCommand.cpp
// Copyright (c) 2020 TXPCo Ltd
/////////////////////////////////////////

#include "Common.h"
#include "CoreModelCommand.h"

using namespace std;

///////////////////////////////////////////////////////////////////////////////
// CoreModel
///////////////////////////////////////////////////////////////////////////////

CoreModel::CoreModel()
{
}

CoreModel::~CoreModel(void)
{
}

///////////////////////////////////////////////////////////////////////////////
// CoreSelection
///////////////////////////////////////////////////////////////////////////////

CoreSelection::CoreSelection()
{
}

CoreSelection::~CoreSelection(void)
{
}

///////////////////////////////////////////////////////////////////////////////
// CoreCommand
///////////////////////////////////////////////////////////////////////////////

CoreCommand::CoreCommand()
{
}

CoreCommand::~CoreCommand(void)
{
}


///////////////////////////////////////////////////////////////////////////////
// CoreCommandProcessor
///////////////////////////////////////////////////////////////////////////////

CoreCommandProcessor::CoreCommandProcessor(std::shared_ptr<CoreModel> pModel)
   : m_pModel (pModel), m_commands (), m_lastDone (m_commands.end())
{
}

CoreCommandProcessor::~CoreCommandProcessor(void)
{
}

bool CoreCommandProcessor::operator==(const CoreCommandProcessor& rhs) const
{
   return m_commands == rhs.m_commands && m_lastDone == rhs.m_lastDone;
}

bool CoreCommandProcessor::operator!=(const CoreCommandProcessor& rhs) const
{
   return m_commands != rhs.m_commands || m_lastDone != rhs.m_lastDone;
}

CoreCommandProcessor& CoreCommandProcessor::operator=(const CoreCommandProcessor& copyMe)
{
   m_commands = copyMe.m_commands;
   m_lastDone = copyMe.m_lastDone;
   return *this;
}

std::shared_ptr<CoreModel> CoreCommandProcessor::model() const
{
   return m_pModel;
}

bool CoreCommandProcessor::adoptAndDo(std::shared_ptr<CoreCommand> pCommand)
{
   bool ok = false;

   ok = pCommand->apply();

   if (ok)
   {
      m_commands.push_front(pCommand);
      m_lastDone = m_commands.begin();
   }
   else
   {
      // There was a non-fatal error -> clear the command stack
      m_commands.erase(m_commands.begin(), m_commands.end());
      m_lastDone = m_commands.end();
   }

   return ok;
}

bool CoreCommandProcessor::canUndo()
{
   // can undo if we have anything in the list and we are not at the end
   if (m_commands.size() > 0 && m_lastDone != m_commands.end())
   {
      return (*m_lastDone)->canUndo();
   }
   else
      return false;
}

void CoreCommandProcessor::undo()
{
   if (canUndo())
   {
      // move fwd one step after undoing - opposite of 'redo' 
      (*m_lastDone)->undo();

      m_lastDone++;
   }
}

bool CoreCommandProcessor::canRedo()
{
   // can redo if we have anything in the list and we are not at the start
   if (m_commands.size() > 0 && m_lastDone != m_commands.begin())
   {
      return true;
   }
   else
      return false;
}

void CoreCommandProcessor::redo()
{
   if (canRedo())
   {
      // move back one step before applying - opposite of 'undo' 
      m_lastDone--;
      (*m_lastDone)->apply();
   }
}
