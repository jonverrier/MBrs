/////////////////////////////////////////
// HostStringLoader.cpp
// Copyright (c) 2020 TXPCo Ltd
/////////////////////////////////////////

#include "HostPrecompile.h"
#include "HostStringLoader.h"
#include "HostException.h"
#include "HostInternal.h"

////////////////////////////////////////////////////////////
// HostStringLoader
////////////////////////////////////////////////////////////

void
HostStringLoader::loadSystemResourceString (const HUint id, HString& out)
{
   TCHAR sz[COMMON_STRING_BUFFER_SIZE];
   ULONG lRet = FormatMessage (FORMAT_MESSAGE_FROM_SYSTEM,
                              NULL, id, 0,
                              sz, 
      COMMON_STRING_BUFFER_SIZE - 1,
                              NULL);

   TESTEQUALTHROWLASTERROR (lRet, 0);

   out = sz;
}

void
HostStringLoader::loadResourceString (const HString& packageName, 
                                      const HUint id, HString& out) 
{
   HANDLE hModule = ::GetModuleHandle (packageName.c_str());
   TCHAR  sz[COMMON_STRING_BUFFER_SIZE];

   ULONG lRet = LoadString ((HMODULE)hModule, id, 
                            sz, 
                            COMMON_STRING_BUFFER_SIZE - 1);

   TESTEQUALTHROWLASTERROR (lRet, 0);

   out = sz;
}

void
HostStringLoader::loadResourceStringWithParms (const HString& packageName,
                                               HUint id, HString& out, 
                                                const HString& parm1, const HString& parm2,
                                                const HString& parm3, const HString& parm4) 
{
   HANDLE hModule = ::GetModuleHandle (packageName.c_str());
   TCHAR  workingBuffer[COMMON_STRING_BUFFER_SIZE];

   ULONG lRet = LoadString ((HMODULE)hModule, id,
                            workingBuffer, 
                            COMMON_STRING_BUFFER_SIZE - 1);
   TESTEQUALTHROWLASTERROR (lRet, 0);

   HString inputs[4];
   TCHAR *pInputBuffers[5];
  
   inputs[0] = parm1;
   pInputBuffers[0]  = const_cast<TCHAR *> (inputs[0].c_str());
   inputs[1] = parm2;
   pInputBuffers[1]  = const_cast<TCHAR *> (inputs[1].c_str());
   inputs[2] = parm3;
   pInputBuffers[2]  = const_cast<TCHAR *> (inputs[2].c_str());
   inputs[3] = parm4;
   pInputBuffers[3]  = const_cast<TCHAR *> (inputs[3].c_str());
   pInputBuffers[4]  = NULL;

   TCHAR sz[COMMON_STRING_BUFFER_SIZE];
   lRet = FormatMessage (FORMAT_MESSAGE_FROM_STRING | FORMAT_MESSAGE_ARGUMENT_ARRAY, 
                         workingBuffer, id, 0,
                         sz, 
                         COMMON_STRING_BUFFER_SIZE - 1, 
                         reinterpret_cast<va_list *> (& pInputBuffers[0]));

   TESTEQUALTHROWLASTERROR (lRet, 0);

   out = sz;
}
