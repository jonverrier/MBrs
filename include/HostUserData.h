/////////////////////////////////////////
// HostUserData.h
// Copyright (c) 2020 TXPCo Ltd
/////////////////////////////////////////

#ifndef HOSTUSERDATA_INCLUDED
#define HOSTUSERDATA_INCLUDED

#include "Common.h"
#include "Host.h"

///////////////////////////////////////////////////////////////////////////////
// HostUserData
///////////////////////////////////////////////////////////////////////////////

class HostRegWrapper;

class HOST_API HostUserData
{
public :
// Constructors

   HostUserData (const HString& applicationKey);

   HostUserData (const HostUserData& copyMe);

   ~HostUserData (void);
   
// Attributes
   bool isDataStoredAt (const HString& key);
   
// Operations
   void writeUint (const HString& key, const HUint value);

   HUint readUint(const HString& key) const;

   void writeString(const HString& key, const HString& value);

   HString readString(const HString& key) const;

   std::vector<HString> readMultiString(const HString& key) const;

   void writeMultiString (const HString& key, const std::vector<HString>& data);

   void removeData(const HString& key);

   void removeAllData();

   HostUserData& operator=(const HostUserData& copyMe);

   static HString defaultImageDirectory();

// Comparison Operators
   
   bool operator==(const HostUserData& rhs) const;

   bool operator!=(const HostUserData& rhs) const;

protected :

private :
   HostUserData(); // Cannot create without an application key. 

   HString m_subPath, m_fullPath;
   std::unique_ptr<HostRegWrapper> m_pKey;
};

#endif // HOSTUSERDATA_INCLUDED
