/////////////////////////////////////////
// HostUserData.cpp
// Copyright (c) 2020 TXPCo Ltd
/////////////////////////////////////////

#include "HostPrecompile.h"

#include "CommonStandardLibraryIncludes.h"
#include "CommonDefinitions.h"

#include "Host.h"
#include "HostUserData.h"
#include "HostException.h"

#include "HostInternal.h"

#include <memory>           // std::unique_ptr
#include <stdexcept>        // std::runtime_error
#include <string>           // std::wstring
#include <utility>          // std::swap, std::pair
#include <vector>           // std::vector

#include <shlobj.h>


class HostWrapper {
   public:

      //
      // Construction/Destruction
      // 

      // Initialize as an empty key handle
      HostWrapper() noexcept = default;
      
      // Open a subkey
      HostWrapper(const HKEY hKeyParent, const HString& subKey);


      // Take ownership of the input key handle
      explicit HostWrapper(HKEY hKey) noexcept;

      // Take ownership of the input key handle.
      // The input key handle wrapper is reset to an empty state.
      HostWrapper(HostWrapper&& other) noexcept;

      // Move-assign from the input key handle.
      // Properly check against self-move-assign (which is safe and does nothing).
      HostWrapper& operator=(HostWrapper&& other) noexcept;

      // Ban copy
      HostWrapper(const HostWrapper&) = delete;
      HostWrapper& operator=(const HostWrapper&) = delete;

      // Safely close the wrapped key handle (if any)
      ~HostWrapper() noexcept;

      //
      // Properties
      // 


      // get the current value 
      HKEY value () const noexcept;

      // Is the wrapped HKEY handle valid?
      bool isValid() const noexcept;

      // Same as isValid(), but allow a short "if (regKey)" syntax
      explicit operator bool() const noexcept;



      // Is the wrapped handle a predefined handle (e.g.HKEY_CURRENT_USER) ?
      bool isPredefined() const noexcept;

      //
      // Operations
      // 



      // Close current HKEY handle.
      // If there's no valid handle, do nothing.
      // This method doesn't close predefined HKEY handles (e.g. HKEY_CURRENT_USER).
      void close() noexcept;



      // Transfer ownership of current HKEY to the caller.
      // Note that the caller is responsible for closing the key handle!
      HKEY detach() noexcept;


      // Take ownership of the input HKEY handle.
      // Safely close any previously open handle.
      // Input key handle can be nullptr.
      void attach(HKEY hKey) noexcept;



      // Non-throwing swap;
      // Note: There's also a non-member swap overload
      void swapWith(HostWrapper& other) noexcept;


      //
      // Wrappers around Windows Registry APIs.
      // See the official MSDN documentation for these APIs for detailed explanations
      // of the wrapper method parameters.
      // 



      // Wrapper around RegCreateKeyEx, that allows you to specify desired access
      void create(
         HKEY hKeyParent,
         const std::wstring& subKey,
         REGSAM desiredAccess = KEY_READ | KEY_WRITE
      );

      void create(
         const HKEY                  hKeyParent,
         const std::wstring & subKey,
         const REGSAM                desiredAccess,
         const DWORD                 options,
         SECURITY_ATTRIBUTES* const  securityAttributes,
         DWORD* const                disposition
      );


      // Wrapper around RegOpenKeyEx
      void open(
         HKEY hKeyParent,
         const std::wstring& subKey,
         REGSAM desiredAccess = KEY_READ | KEY_WRITE
      );

      //
      // Registry Value Setters
      // 
      void setUintValue(const std::wstring& valueName, DWORD data);
      void setStringValue(const std::wstring& valueName, const std::wstring& data);
      void setMultiStringValue(const std::wstring& valueName, const std::vector<std::wstring>& data);

      //
      // Registry Value Getters
      // 
      DWORD uintValue(const std::wstring& valueName) const;
      std::wstring stringValue(const std::wstring& valueName) const;
      std::vector<std::wstring> multiStringValue(const std::wstring& valueName) const;

      //
      // Query Operations
      // 
      void keyInfo(DWORD& subKeys, DWORD& values, FILETIME& lastWriteTime) const;

      // Return the DWORD type ID for the input registry value
      DWORD valueType(const std::wstring& valueName) const; // this one throws exception if nothing stored
      DWORD testValueType(const std::wstring& valueName) const; // this one returns 0 nothing stored

      // Enumerate the subkeys of the registry key, using RegEnumKeyEx
      std::vector<std::wstring> enumSubKeys() const;

      // Enumerate the values under the registry key, using RegEnumValue.
      // Returns a vector of pairs: In each pair, the wstring is the value name, 
      // the DWORD is the value type.
      std::vector<std::pair<std::wstring, DWORD>> EnumValues() const;


      //
      // Misc Registry API Wrappers
      // 
      void deleteValue(const std::wstring& valueName);
      void deleteKey(const std::wstring& subKey, REGSAM desiredAccess);
      void deleteTree(const std::wstring& subKey);
      void copyTree(const std::wstring& sourceSubKey, const HostWrapper& destKey);
      void flushKey();
      void loadKey(const std::wstring& subKey, const std::wstring& filename);
      void saveKey(const std::wstring& filename, SECURITY_ATTRIBUTES* securityAttributes);

      //
      // Relational comparison operators are overloaded as non-members
      // ==, !=
      // 

   private:

      // The wrapped registry key handle
      HKEY m_hKey{ nullptr };
};


//------------------------------------------------------------------------------
//          Overloads of relational comparison operators for KeyWrapper
//------------------------------------------------------------------------------
inline bool operator==(const HostWrapper& a, const HostWrapper& b) noexcept
{
      return a.value() == b.value();
}

inline bool operator!=(const HostWrapper& a, const HostWrapper& b) noexcept
{
      return a.value() != b.value();
 }

//------------------------------------------------------------------------------
//                          HostWrapper Inline Methods
//------------------------------------------------------------------------------
inline HostWrapper::HostWrapper(const HKEY hKey) noexcept
      : m_hKey{ hKey }
{}


inline HostWrapper::HostWrapper(const HKEY hKeyParent, const std::wstring& subKey)
{
   create(hKeyParent, subKey);
}

inline HostWrapper::HostWrapper(HostWrapper&& other) noexcept
   : m_hKey{ other.m_hKey }
{
   // Other doesn't own the handle anymore
   other.m_hKey = nullptr;
}

inline HostWrapper& HostWrapper::operator=(HostWrapper&& other) noexcept
{
   // Prevent self-move-assign
   if ((this != &other) && (m_hKey != other.m_hKey))
   {
      // Close current
      close();

      // Move from other (i.e. take ownership of other's raw handle)
      m_hKey = other.m_hKey;
      other.m_hKey = nullptr;

   }

   return *this;
}

inline HostWrapper::~HostWrapper() noexcept
{
   // Release the owned handle (if any)
   close();
}

inline HKEY HostWrapper::value() const noexcept
{
   return m_hKey;
}

inline void HostWrapper::close() noexcept
{
   if (isValid())
   {
      // Do not call RegCloseKey on predefined keys
      if (!isPredefined())
      {
         ::RegCloseKey(m_hKey);
      }
      // Avoid dangling references
      m_hKey = nullptr;
   }
}

inline bool HostWrapper::isValid() const noexcept
{
   return m_hKey != nullptr;
}

inline HostWrapper::operator bool() const noexcept
{
   return isValid();
}

inline bool HostWrapper::isPredefined() const noexcept
{
   // Predefined keys
   // https://msdn.microsoft.com/en-us/library/windows/desktop/ms724836(v=vs.85).aspx

   if ((m_hKey == HKEY_CURRENT_USER)
       || (m_hKey == HKEY_LOCAL_MACHINE)
       || (m_hKey == HKEY_CLASSES_ROOT)
       || (m_hKey == HKEY_CURRENT_CONFIG)
       || (m_hKey == HKEY_CURRENT_USER_LOCAL_SETTINGS)
       || (m_hKey == HKEY_PERFORMANCE_DATA)
       || (m_hKey == HKEY_PERFORMANCE_NLSTEXT)
       || (m_hKey == HKEY_PERFORMANCE_TEXT)
       || (m_hKey == HKEY_USERS))
   {
      return true;
   }
   return false;
}

inline HKEY HostWrapper::detach() noexcept
{
   HKEY hKey{ m_hKey };

   // We don't own the HKEY handle anymore
   m_hKey = nullptr;
   // Transfer ownership to the caller
   return hKey;
}

inline void HostWrapper::attach(const HKEY hKey) noexcept
{
   // Prevent self-attach
   if (m_hKey != hKey)
   {
      // Close any open registry handle
      close();

      // Take ownership of the input hKey
      m_hKey = hKey;

   }
}

inline void HostWrapper::swapWith(HostWrapper& other) noexcept
{
   // Enable ADL (not necessary in this case, but good practice)
   using std::swap;

   // Swap the raw handle members
   swap(m_hKey, other.m_hKey);
}

inline void swap(HostWrapper& a, HostWrapper& b) noexcept
{
   a.swapWith(b);
}

inline void HostWrapper::create(
      const HKEY hKeyParent,
      const std::wstring& subKey,
      const REGSAM desiredAccess
   )
 {
   constexpr DWORD kDefaultOptions = REG_OPTION_NON_VOLATILE;

   create(hKeyParent, subKey, desiredAccess, kDefaultOptions,
         nullptr, // no security attributes,
         nullptr  // no disposition 
      );
}

inline void HostWrapper::create(
   const HKEY                  hKeyParent,
   const std::wstring& subKey,
   const REGSAM                desiredAccess,
   const DWORD                 options,
   SECURITY_ATTRIBUTES* const  securityAttributes,
   DWORD* const                disposition
   )
{
   HKEY hKey{ nullptr };
   LONG retCode = ::RegCreateKeyEx(
         hKeyParent,
         subKey.c_str(),
         0,          // reserved
         REG_NONE,   // user-defined class type parameter not supported
         options,
         desiredAccess,
         securityAttributes,
         &hKey,
         disposition
      );

      if (retCode != ERROR_SUCCESS)
      {
         THROW(HostException, retCode);
      }

   // Safely close any previously opened key
   close();

   // Take ownership of the newly created key
   m_hKey = hKey;
}

inline void HostWrapper::open(
   const HKEY              hKeyParent,
   const std::wstring& subKey,
   const REGSAM            desiredAccess)
{
   HKEY hKey{ nullptr };
   LONG retCode = ::RegOpenKeyEx(
         hKeyParent,
         subKey.c_str(),
         REG_NONE,           // default options
         desiredAccess,
         &hKey);

   if (retCode != ERROR_SUCCESS)
   {
      THROW (HostException, retCode);
   }

   // Safely close any previously opened key
   close();

   // Take ownership of the newly created key
   m_hKey = hKey;
}

inline void HostWrapper::setUintValue(const std::wstring& valueName, const DWORD data)
{
   _ASSERTE(isValid());

   LONG retCode = ::RegSetValueEx(
         m_hKey,
         valueName.c_str(),
         0, // reserved
         REG_DWORD,
         reinterpret_cast<const BYTE*>(&data),
         sizeof(data));

   if (retCode != ERROR_SUCCESS)
   {
      THROW (HostException, retCode);
   }
}

inline void HostWrapper::setStringValue(const std::wstring& valueName, const std::wstring& data)
{
   _ASSERTE(isValid());

   // String size including the terminating NUL, in bytes

   const DWORD dataSize = static_cast<DWORD>((data.length() + 1) * sizeof(wchar_t));
   LONG retCode = ::RegSetValueEx(
               m_hKey,
               valueName.c_str(),
               0, // reserved
               REG_SZ,
               reinterpret_cast<const BYTE*>(data.c_str()),
               dataSize);

   if (retCode != ERROR_SUCCESS)
   {
      THROW(HostException, retCode);
   }
}

namespace details
{
   // Helper function to build a multi-string from a vector<wstring>

   inline std::vector<wchar_t> BuildMultiString(const std::vector<std::wstring>& data)
   {

       // Special case of the empty multi-string
       if (data.empty())
       {
          // Build a vector containing just two NULs
          return std::vector<wchar_t>(2, L'\0');
       }

       // Get the total length in wchar_ts of the multi-string
       size_t totalLen = 0;

       for (const auto& s : data)
       {
          // Add one to current string's length for the terminating NUL
          totalLen += (s.length() + 1);
       }

       // Add one for the last NUL terminator (making the whole structure double-NUL terminated)
       totalLen++;

       // Allocate a buffer to store the multi-string
       std::vector<wchar_t> multiString;

       multiString.reserve(totalLen);

       // Copy the single strings into the multi-string
       for (const auto& s : data)
       {
           multiString.insert(multiString.end(), s.begin(), s.end());
           // Don't forkeyValue to NUL-terminate the current string
           multiString.push_back(L'\0');
       }
     
       // Add the last NUL-terminator
       multiString.push_back(L'\0');

       return multiString;
   }
} // namespace details


inline void HostWrapper::setMultiStringValue(
           const std::wstring& valueName,
           const std::vector<std::wstring>& data)
{
   _ASSERTE(isValid());

   // First, we have to build a double-NUL-terminated multi-string from the input data
   const std::vector<wchar_t> multiString = details::BuildMultiString(data);

   // Total size, in bytes, of the whole multi-string structure
   const DWORD dataSize = static_cast<DWORD>(multiString.size() * sizeof(wchar_t));

   LONG retCode = ::RegSetValueEx(
         m_hKey,
         valueName.c_str(),
         0, // reserved
         REG_MULTI_SZ,
         reinterpret_cast<const BYTE*>(&multiString[0]),
         dataSize);

   if (retCode != ERROR_SUCCESS)
   {
      THROW(HostException, retCode);
   }
}

inline DWORD HostWrapper::uintValue(const std::wstring& valueName) const
{
   _ASSERTE(isValid());

   DWORD data{};                   // to be read from the registry
   DWORD dataSize = sizeof(data);  // size of data, in bytes

   const DWORD flags = RRF_RT_REG_DWORD;
   LONG retCode = ::RegGetValue(
         m_hKey,
         nullptr, // no subkey
         valueName.c_str(),
         flags,
         nullptr, // type not required
         &data,
         &dataSize);

   if (retCode != ERROR_SUCCESS)
   {
      THROW(HostException, retCode);
   }
   return data;
}

inline std::wstring HostWrapper::stringValue(const std::wstring& valueName) const
{
   _ASSERTE(isValid());

   // Get the size of the result string
   DWORD dataSize = 0; // size of data, in bytes
   const DWORD flags = RRF_RT_REG_SZ;
   LONG retCode = ::RegGetValue(
      m_hKey,
      nullptr, // no subkey
      valueName.c_str(),
      flags,
      nullptr, // type not required
      nullptr, // output buffer not needed now
      &dataSize);

   if (retCode != ERROR_SUCCESS)
   {
      THROW(HostException, retCode);
   }


   // Allocate a string of proper size.
   // Note that dataSize is in bytes and includes the terminating NUL;
   // we have to convert the size from bytes to wchar_ts for wstring::resize.
   std::wstring result;
   result.resize(dataSize / sizeof(wchar_t));

   // Call RegGetValue for the second time to read the string's content
   retCode = ::RegGetValue(
      m_hKey,
      nullptr,    // no subkey
      valueName.c_str(),
      flags,
      nullptr,    // type not required
      &result[0], // output buffer
      &dataSize);

   if (retCode != ERROR_SUCCESS)
   {
      THROW(HostException, retCode);
   }

   // Remove the NUL terminator scribbled by RegGetValue from the wstring
   result.resize((dataSize / sizeof(wchar_t)) - 1);

   return result;
}

inline std::vector<std::wstring> HostWrapper::multiStringValue (const std::wstring& valueName) const
{
   _ASSERTE(isValid());

   // Request the size of the multi-string, in bytes
   DWORD dataSize = 0;

   const DWORD flags = RRF_RT_REG_MULTI_SZ;
   LONG retCode = ::RegGetValue(
         m_hKey,
         nullptr,    // no subkey
         valueName.c_str(),
         flags,
         nullptr,    // type not required
         nullptr,    // output buffer not needed now
         &dataSize);

   if (retCode != ERROR_SUCCESS)
   {
      THROW(HostException, retCode);
   }

   // Allocate room for the result multi-string.
   // Note that dataSize is in bytes, but our vector<wchar_t>::resize method requires size 
   // to be expressed in wchar_ts.

   std::vector<wchar_t> data;
   data.resize(dataSize / sizeof(wchar_t));

   // Read the multi-string from the registry into the vector object
   retCode = ::RegGetValue(
         m_hKey,
         nullptr,    // no subkey
         valueName.c_str(),
         flags,
         nullptr,    // no type required
         &data[0],   // output buffer
         &dataSize
      );

   if (retCode != ERROR_SUCCESS)
   {
      THROW(HostException, retCode);
   }

   // Resize vector to the actual size returned by GetRegValue.
   // Note that the vector is a vector of wchar_ts, instead the size returned by GetRegValue
   // is in bytes, so we have to scale from bytes to wchar_t count.
   data.resize(dataSize / sizeof(wchar_t));

   // Parse the double-NUL-terminated string into a vector<wstring>, 
   // which will be returned to the caller
   std::vector<std::wstring> result;

   const wchar_t* currStringPtr = &data[0];

   while (*currStringPtr != L'\0')
   {

      // Current string is NUL-terminated, so get its length calling wcslen
      const size_t currStringLength = wcslen(currStringPtr);

      // Add current string to the result vector
      result.push_back(std::wstring{ currStringPtr, currStringLength });

      // Move to the next string
      currStringPtr += currStringLength + 1;
   }

   return result;
}

inline DWORD HostWrapper::valueType(const std::wstring& valueName) const
{
   _ASSERTE(isValid());

   DWORD typeId{};     // will be returned by RegQueryValueEx

   LONG retCode = ::RegQueryValueEx(
         m_hKey,
         valueName.c_str(),
         nullptr,    // reserved
         &typeId,
         nullptr,    // not interested
         nullptr     // not interested
      );

   if (retCode != ERROR_SUCCESS)
   {
      THROW(HostException, retCode);
   }
   return typeId;
}

inline DWORD HostWrapper::testValueType(const std::wstring& valueName) const
{
   _ASSERTE(isValid());

   DWORD typeId{};     // will be returned by RegQueryValueEx

   LONG retCode = ::RegQueryValueEx(
      m_hKey,
      valueName.c_str(),
      nullptr,    // reserved
      &typeId,
      nullptr,    // not interested
      nullptr     // not interested
   );

   if (retCode != ERROR_SUCCESS)
   {
      typeId = 0;
   }
   return typeId;
}

inline void HostWrapper::keyInfo(DWORD& subKeys, DWORD& values, FILETIME& lastWriteTime) const
{
   _ASSERTE(isValid());

   LONG retCode = ::RegQueryInfoKey(
         m_hKey,
         nullptr,
         nullptr,
         nullptr,
         &subKeys,
         nullptr,
         nullptr,
         &values,
         nullptr,
         nullptr,
         nullptr,
         &lastWriteTime);

   if (retCode != ERROR_SUCCESS)
   {
      THROW(HostException, retCode);
   }
}

inline std::vector<std::wstring> HostWrapper::enumSubKeys() const
{
   _ASSERTE(isValid());

   // Get some useful enumeration info, like the total number of subkeys
   // and the maximum length of the subkey names
   DWORD subKeyCount{};
   DWORD maxSubKeyNameLen{};
   LONG retCode = ::RegQueryInfoKey(
         m_hKey,
         nullptr,    // no user-defined class
         nullptr,    // no user-defined class size
         nullptr,    // reserved
         &subKeyCount,
         &maxSubKeyNameLen,
         nullptr,    // no subkey class length
         nullptr,    // no value count
         nullptr,    // no value name max length
         nullptr,    // no max value length
         nullptr,    // no security descriptor
         nullptr);   // no last write time

   if (retCode != ERROR_SUCCESS)
   {
      THROW(HostException, retCode);
   }
   
   // NOTE: According to the MSDN documentation, the size returned for subkey name max length
   // does *not* include the terminating NUL, so let's add +1 to take it into account
   // when I allocate the buffer for reading subkey names.
   maxSubKeyNameLen++;

   // Preallocate a buffer for the subkey names
   auto nameBuffer = std::make_unique<wchar_t[]>(maxSubKeyNameLen);

   // The result subkey names will be stored here
   std::vector<std::wstring> subkeyNames;

   // Enumerate all the subkeys
   for (DWORD index = 0; index < subKeyCount; index++)
   {
      // Get the name of the current subkey
      DWORD subKeyNameLen = maxSubKeyNameLen;
      retCode = ::RegEnumKeyEx(
            m_hKey,
            index,
            nameBuffer.get(),
            &subKeyNameLen,
            nullptr, // reserved
            nullptr, // no class
            nullptr, // no class
            nullptr);  // no last write time
         

      if (retCode != ERROR_SUCCESS)
      {
         THROW(HostException, retCode);
      }

      // On success, the ::RegEnumKeyEx API writes the length of the
      // subkey name in the subKeyNameLen output parameter 
      // (not including the terminating NUL).
      // So I can build a wstring based on that length.

      subkeyNames.push_back(std::wstring{ nameBuffer.get(), subKeyNameLen });
   }

   return subkeyNames;
}

inline std::vector<std::pair<std::wstring, DWORD>> HostWrapper::EnumValues() const
{
   _ASSERTE(isValid());

   // Get useful enumeration info, like the total number of values
   // and the maximum length of the value names

   DWORD valueCount{};
   DWORD maxValueNameLen{};
   LONG retCode = ::RegQueryInfoKey(
         m_hKey,
         nullptr,    // no user-defined class
         nullptr,    // no user-defined class size
         nullptr,    // reserved
         nullptr,    // no subkey count
         nullptr,    // no subkey max length
         nullptr,    // no subkey class length
         &valueCount,
         &maxValueNameLen,
         nullptr,    // no max value length
         nullptr,    // no security descriptor
         nullptr     // no last write time
      );

   if (retCode != ERROR_SUCCESS)
   {
      THROW(HostException, retCode);
   }

   // NOTE: According to the MSDN documentation, the size returned for value name max length
   // does *not* include the terminating NUL, so let's add +1 to take it into account
   // when I allocate the buffer for reading value names.

   maxValueNameLen++;

   // Preallocate a buffer for the value names
   auto nameBuffer = std::make_unique<wchar_t[]>(maxValueNameLen);

   // The value names and types will be stored here
   std::vector<std::pair<std::wstring, DWORD>> valueInfo;

   // Enumerate all the values
   for (DWORD index = 0; index < valueCount; index++)
   {
      // Get the name and the type of the current value
      DWORD valueNameLen = maxValueNameLen;
      DWORD valueType{};

      retCode = ::RegEnumValue(
            m_hKey,
            index,
            nameBuffer.get(),
            &valueNameLen,
            nullptr,    // reserved
            &valueType,
            nullptr,    // no data
            nullptr     // no data size
         );
         if (retCode != ERROR_SUCCESS)
         {
            THROW(HostException, retCode);
         }

         // On success, the RegEnumValue API writes the length of the
         // value name in the valueNameLen output parameter 
         // (not including the terminating NUL).
         // So we can build a wstring based on that.

         valueInfo.push_back(
            std::make_pair(std::wstring{ nameBuffer.get(), valueNameLen }, valueType)
         );
      }

   return valueInfo;
}

inline void HostWrapper::deleteValue(const std::wstring& valueName)
{
   _ASSERTE(isValid());

   LONG retCode = ::RegDeleteValue(m_hKey, valueName.c_str());

   if (retCode != ERROR_SUCCESS)
   {
      THROW(HostException, retCode);
   }
}

inline void HostWrapper::deleteKey(const std::wstring& subKey, const REGSAM desiredAccess)
{
   _ASSERTE(isValid());

   LONG retCode = ::RegDeleteKeyEx(m_hKey, subKey.c_str(), desiredAccess, 0);

   if (retCode != ERROR_SUCCESS)
   {
      THROW(HostException, retCode);
   }
}

inline void HostWrapper::deleteTree(const std::wstring& subKey)
{
   _ASSERTE(isValid());

   LONG retCode = ::RegDeleteTree(m_hKey, subKey.c_str());

   if (retCode != ERROR_SUCCESS)
   {
      THROW(HostException, retCode);
   }
}

inline void HostWrapper::flushKey()
{
   _ASSERTE(isValid());

   LONG retCode = ::RegFlushKey(m_hKey);

   if (retCode != ERROR_SUCCESS)
   {
      THROW(HostException, retCode);
   }
}

inline void HostWrapper::loadKey(const std::wstring& subKey, const std::wstring& filename)
{
   close();

   LONG retCode = ::RegLoadKey(m_hKey, subKey.c_str(), filename.c_str());

   if (retCode != ERROR_SUCCESS)
   {
      THROW(HostException, retCode);
   }
}

inline void HostWrapper::saveKey(
      const std::wstring& filename,
      SECURITY_ATTRIBUTES* const securityAttributes
)
{
   _ASSERTE(isValid());

   LONG retCode = ::RegSaveKey(m_hKey, filename.c_str(), securityAttributes);

   if (retCode != ERROR_SUCCESS)
   {
      THROW(HostException, retCode);
   }

}


////////////////////////////////////////////////////////////////////////////
// HostUserData
////////////////////////////////////////////////////////////////////////////

static const HString g_stem(H_TEXT("Software\\TXPCO\\Media Bowser\\"));

HostUserData::HostUserData (const HString& applicationKey) :
   m_subPath(applicationKey),
   m_fullPath (g_stem + applicationKey + HString(H_TEXT("\\"))),
   m_pKey (COMMON_NEW HostWrapper(HKEY_CURRENT_USER, m_fullPath))
{
}
   
HostUserData::~HostUserData (void)
{
}

 HostUserData::HostUserData (const HostUserData& src) :
    m_subPath(src.m_subPath),
    m_fullPath(src.m_fullPath),
    m_pKey (COMMON_NEW HostWrapper(HKEY_CURRENT_USER, src.m_fullPath))
{

}

HostUserData& 
HostUserData::operator=(const HostUserData& src)
{
   if (this == &src)
      return *this;

   m_subPath = src.m_subPath;
   m_fullPath = src.m_fullPath;
   m_pKey.reset (COMMON_NEW HostWrapper(HKEY_CURRENT_USER, src.m_fullPath));

   return *this; 
}
   
bool   
HostUserData::operator==(const HostUserData& rhs) const
{
   if (this == &rhs)
      return TRUE;

   return (m_fullPath == rhs.m_fullPath);
}

bool   
HostUserData::operator!=(const HostUserData& rhs) const
{
   if (this == &rhs)
      return FALSE;

   return (m_fullPath != rhs.m_fullPath);
}

void 
HostUserData::writeUint(const HString& key, const HUint value) 
{
   m_pKey->setUintValue(key, value);
}

HUint
HostUserData::readUint(const HString& key) const
{
   return m_pKey->uintValue(key);
}

void
HostUserData::writeString(const HString& key, const HString& value)
{
   m_pKey->setStringValue(key, value);
}

HString
HostUserData::readString(const HString& key) const
{
   return m_pKey->stringValue(key);
}

std::vector<HString>
HostUserData::readMultiString(const HString& key) const
{
   return m_pKey->multiStringValue(key);
}

void
HostUserData::writeMultiString(const HString& key, const std::vector<HString>& data)
{
   m_pKey->setMultiStringValue(key, data);
}

bool
HostUserData::isDataStored (const HString& key)
{
   // Return the DWORD type ID for the input registry key
   DWORD ret = m_pKey->testValueType(key);

   return (ret != 0);
}

void
HostUserData::removeData (const HString& key)
{
   m_pKey->deleteValue (key);
}

void
HostUserData::removeAllData ()
{
   HostWrapper wrapper(HKEY_CURRENT_USER, g_stem);

   wrapper.deleteTree(m_subPath);
}


HString
HostUserData::defaultImageDirectory()
{
   WCHAR myPictures[MAX_PATH] = H_TEXT("");
   
   BOOL result = SHGetSpecialFolderPathW(NULL, myPictures, CSIDL_MYPICTURES, TRUE);

   if (!result)
      result = SHGetSpecialFolderPathW(NULL, myPictures, CSIDL_COMMON_PICTURES, TRUE);
   
   if (!result)
      THROW(HostException, __HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND));

   return myPictures;
}