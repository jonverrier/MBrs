/////////////////////////////////////////
// HostLocks.cpp
// Copyright (c) 2020 TXPCo Ltd
/////////////////////////////////////////

#include "HostPrecompile.h"

#include "CommonStandardLibraryIncludes.h"
#include "CommonDefinitions.h"

#include "Host.h"
#include "HostLocks.h"
#include "HostException.h"

#include "HostInternal.h"

////////////////////////////////////////////////////////////////////////////
// HostInterlockedCount
////////////////////////////////////////////////////////////////////////////

HostInterlockedCount::HostInterlockedCount ()
{
   m_pCount = COMMON_NEW unsigned long;
   set (0);
}
 
HostInterlockedCount::HostInterlockedCount (HUint initialValue)
{
   m_pCount = COMMON_NEW unsigned long;
   set (initialValue);
}
   
HostInterlockedCount::~HostInterlockedCount (void)
{
   COMMON_DELETE(static_cast <unsigned long *> (m_pCount));
}

 HostInterlockedCount::HostInterlockedCount (const HostInterlockedCount& src)
{
   m_pCount = COMMON_NEW long;
   set (src.count());
}

HostInterlockedCount& 
HostInterlockedCount::operator=(const HostInterlockedCount& src)
{
   if (this == &src)
      return *this;

   set (src.count());
   return *this; 
}
   
bool   
HostInterlockedCount::operator==(const HostInterlockedCount& rhs) const
{
   if (this == &rhs)
      return TRUE;

   return (count () == rhs.count());
}

bool   
HostInterlockedCount::operator!=(const HostInterlockedCount& rhs) const
{
   if (this == &rhs)
      return FALSE;

   return (count () != rhs.count());
}

bool   
HostInterlockedCount::operator<(const HostInterlockedCount& rhs) const
{
   if (this == &rhs)
      return FALSE;

   return (count () < rhs.count());
}

bool   
HostInterlockedCount::operator<= (const HostInterlockedCount& rhs) const
{
   if (this == &rhs)
      return TRUE;
   return (count () <= rhs.count());
}

bool   
HostInterlockedCount::operator>(const HostInterlockedCount& rhs) const
{
   if (this == &rhs)
      return FALSE;
   return (count () > rhs.count());
}

bool   
HostInterlockedCount::operator>=(const HostInterlockedCount& rhs) const
{
   if (this == &rhs)
      return TRUE;
   return (count () >= rhs.count());
}

HUint  
HostInterlockedCount::increment ()
{
   return ::InterlockedIncrement (static_cast<long *>(m_pCount));
}

HUint 
HostInterlockedCount::decrement ()
{
   return ::InterlockedDecrement (static_cast<long *>(m_pCount));
}

HUint  
HostInterlockedCount::count () const
{
   return ::InterlockedExchangeAdd (static_cast<long *>(m_pCount), 0);

#if 0
   int  retval = 0;
   int *addr = (int *)m_pImpl;
   
   __asm push eax
   __asm push ecx
   __asm mov  eax, addr                 // target address in eax
   __asm mov  ecx, 0h                   // Set ecx to 0
   __asm lock xadd dword ptr[eax], ecx  // Move value in ecx into [eax]; 
                                        // and put sum into ecx
   __asm xchg ecx, eax                  // Get sum into eax
   __asm mov  retval, eax               // Get sum into retval
   __asm pop  ecx
   __asm pop  eax
 
   return retval;
#endif
}

void  
HostInterlockedCount::set (HUint newValue)
{
   ::InterlockedExchange (static_cast<unsigned long *>(m_pCount), newValue);
#if 0
   int  value = (int) uNewValue;
   int *addr = (int *)m_pImpl;
   
   __asm push eax
   __asm push ecx
   __asm mov  eax, addr                 // target address in eax
   __asm mov  ecx, value                // Set ecx to new value
   __asm lock xchg dword ptr[eax], ecx  // Move value in ecx into [eax]; 
   __asm pop  ecx
   __asm pop  eax
#endif
}