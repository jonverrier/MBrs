/////////////////////////////////////////
// HostStringLoader.h
// Copyright (c) 2020 TXPCo Ltd
/////////////////////////////////////////

#ifndef HOSTSTRINGLOADER_INCLUDED
#define HOSTSTRINGLOADER_INCLUDED

#include "CommonDefinitions.h"
#include "CommonStandardLibraryIncludes.h"
#include "Host.h"

///////////////////////////////////////////////////////////////////////////////
// HostStringLoader - loads resource strings
///////////////////////////////////////////////////////////////////////////////

class HOST_API HostStringLoader 
{

public:
   static void loadSystemResourceString (const HUint uID, HString& out);

   static void loadResourceString (const HString& libraryName,
                                   const HUint id, HString& out);

   static void loadResourceStringWithParms (const HString& libraryName,
                                            HUint id, 
                                            HString& out,
                                            const HString& parm1, const HString& parm2,
                                            const HString& parm3, const HString& parm4);

private:
   HostStringLoader ();
   HostStringLoader (const HostStringLoader& rhs);
   HostStringLoader& operator=(const HostStringLoader& rhs);
};


#endif // HOSTSTRINGLOADER_INCLUDED
