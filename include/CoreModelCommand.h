/////////////////////////////////////////
// CoreModelCommand.h
// Copyright (c) 2020 TXPCo Ltd
/////////////////////////////////////////

#ifndef COREMODELCOMMAND_INCLUDED
#define COREMODELCOMMAND_INCLUDED

#include <exiv2/exiv2.hpp>
#include "CommonDefinitions.h"
#include "CommonStandardLibraryIncludes.h"
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


   // Operations

   CoreCommand&
   operator=(const CoreCommand& copyMe) = delete;

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

   CoreCommandProcessor();

   virtual ~CoreCommandProcessor(void);

   bool
      operator==(const CoreCommandProcessor& rhs) const;

   bool
      operator!=(const CoreCommandProcessor& rhs) const;

   // Operations

   CoreCommandProcessor&
   operator=(const CoreCommandProcessor& copyMe);

  void
  adoptAndProcess(std::shared_ptr<CoreCommand> pCommand);

protected:

private:
   std::list<std::shared_ptr<CoreCommand>> m_commands;
};

#endif // COREMODELCOMMAND_INCLUDED
