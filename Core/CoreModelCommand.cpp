/////////////////////////////////////////
// CoreModelCommand.cpp
// Copyright (c) 2020 TXPCo Ltd
/////////////////////////////////////////

#include "CorePrecompile.h"
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

CoreCommandProcessor::CoreCommandProcessor()
{
}

CoreCommandProcessor::~CoreCommandProcessor(void)
{
}

bool
CoreCommandProcessor::operator==(const CoreCommandProcessor& rhs) const
{
   return m_commands == rhs.m_commands;
}

bool
CoreCommandProcessor::operator!=(const CoreCommandProcessor& rhs) const
{
   return m_commands != rhs.m_commands;
}

CoreCommandProcessor&
CoreCommandProcessor::operator=(const CoreCommandProcessor& copyMe)
{
   m_commands = copyMe.m_commands;
   return *this;
}

void
CoreCommandProcessor::adoptAndProcess(std::shared_ptr<CoreCommand> pCommand)
{
   m_commands.push_back(pCommand);
}