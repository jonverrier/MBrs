/////////////////////////////////////////
// CoreFile.cpp
// Copyright (c) 2020 TXPCo Ltd
/////////////////////////////////////////

#include "CorePrecompile.h"

#include "CommonStandardLibraryIncludes.h"
#include "CommonDefinitions.h"

#include "Host.h"
#include "HostException.h"

#include "Core.h"
#include "CoreFile.h"

//#include "CoreInternal.h"

using namespace std;

static const HChar* imageExtensions[] = { H_TEXT(".jpg"), H_TEXT(".png") };

////////////////////////////////////////////////////////////////////////////
// CoreFileSystemEntity
////////////////////////////////////////////////////////////////////////////

CoreFileSystemEntity::CoreFileSystemEntity(const HString& path) :
   m_path(path)
{
}
   
CoreFileSystemEntity::~CoreFileSystemEntity(void)
{
}

CoreFileSystemEntity::CoreFileSystemEntity(const CoreFileSystemEntity& src) :
   m_path(src.m_path)
{

}

CoreFileSystemEntity&
CoreFileSystemEntity::operator=(const CoreFileSystemEntity& src)
{
   if (this == &src)
      return *this;

   m_path = src.m_path;

   return *this; 
}
   
HBool   
CoreFileSystemEntity::operator==(const CoreFileSystemEntity& rhs) const
{
   if (this == &rhs)
      return TRUE;

   return (m_path == rhs.m_path);
}

HBool   
CoreFileSystemEntity::operator!=(const CoreFileSystemEntity& rhs) const
{
   if (this == &rhs)
      return FALSE;

   return (m_path != rhs.m_path);
}

HString
CoreFileSystemEntity::path() const
{
   return m_path;
}

HBool
CoreFileSystemEntity::existsOnFileSystem() const
{
   error_code ec;

   // Use the version that does not throw an exception if file does not exist
   return filesystem::exists (m_path, ec);
}

HBool 
CoreFileSystemEntity::lastWriteTime(filesystem::file_time_type& writeTime) const
{
   error_code ec;

   // Use the version that does not throw an exception if file does not exist
   writeTime = filesystem::last_write_time(m_path, ec);

   return (ec.value() == 0);
}

HBool
CoreFileSystemEntity::isDirectory() const
{
   error_code ec;

   // Use the version that does not throw an exception if file does not exist
   return  filesystem::is_directory(m_path, ec);
}

HBool
CoreFileSystemEntity::isFile() const
{
   error_code ec;

   // Use the version that does not throw an exception if file does not exist
   return filesystem::is_regular_file(m_path, ec);
}

HBool
CoreFileSystemEntity::isImageFile() const
{
   error_code ec;

   // Use the version that does not throw an exception if file does not exist
   HBool isFile = filesystem::is_regular_file(m_path, ec);
   if (isFile)
   {
      HString extension((filesystem::path(m_path)).extension());
      
      // OK to convert using 'tolower' as we know file extensions are ascii, and if the file name passed in is not, it won't match 
      transform(extension.begin(), extension.end(), extension.begin(), ::tolower);

      // Loop throug Image fle extensions looking for match, if found then break
      for (auto iter =0; iter < (sizeof(imageExtensions) / sizeof(imageExtensions[0])); iter++)
      {
         if (extension == HString(imageExtensions[iter]))
            return true;
      }
   }
   return false;
}

