/////////////////////////////////////////
// HostLocks.h
// Copyright (c) 2020 TXPCo Ltd
/////////////////////////////////////////

#ifndef HOSTLOCKS_INCLUDED
#define HOSTLOCKS_INCLUDED

#include "Common.h"
#include "Host.h"

///////////////////////////////////////////////////////////////////////////////
// HostInterlockedCount
///////////////////////////////////////////////////////////////////////////////

class HOST_API HostInterlockedCount
{
public :
// Constructors
   HostInterlockedCount ();

   HostInterlockedCount (HUint initialValue);

   HostInterlockedCount (const HostInterlockedCount& copyMe);

   ~HostInterlockedCount (void);
   
// Attributes
   HUint count () const;

   void set (HUint uValue);
   
// Operations
   HUint increment ();

   HUint decrement ();

   HostInterlockedCount& operator=(const HostInterlockedCount& copyMe);

// Comparison Operators
   
   bool operator==(const HostInterlockedCount& rhs) const;

   bool operator!=(const HostInterlockedCount& rhs) const;

   bool operator<(const HostInterlockedCount& rhs) const;

   bool operator<=(const HostInterlockedCount& rhs) const;

   bool operator>(const HostInterlockedCount& rhs) const;

   bool operator>=(const HostInterlockedCount& rhs) const;

protected :

private :
   void *m_pCount;
};

#endif // HOSTLOCKS_INCLUDED
