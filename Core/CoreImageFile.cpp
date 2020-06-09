/////////////////////////////////////////
// CoreImageFile.cpp
// Copyright (c) 2020 TXPCo Ltd
/////////////////////////////////////////

#include "Common.h"
#include "CoreImageFile.h"
#include "HostUserData.h"

using namespace std;

////////////////////////////////////////////////////////////////////////////
// CoreImageFile
////////////////////////////////////////////////////////////////////////////

CoreImageFile::CoreImageFile(const HString& path) :
   CoreFileSystemEntity (path),
   m_tagCache ()
{
}

CoreImageFile::~CoreImageFile(void)
{
}

CoreImageFile::CoreImageFile(const CoreImageFile& src) :
   CoreFileSystemEntity(src),
   m_tagCache (src.m_tagCache)
{

}

CoreImageFile&
CoreImageFile::operator=(const CoreImageFile& src)
{
   CoreFileSystemEntity::operator=(src);
   m_tagCache = src.m_tagCache;

   return *this;
}

bool
CoreImageFile::operator==(const CoreImageFile& rhs) const
{
   if (this == &rhs)
      return TRUE;

   return (CoreFileSystemEntity::operator==(rhs));
}

bool
CoreImageFile::operator!=(const CoreImageFile& rhs) const
{
   if (this == &rhs)
      return FALSE;

   return (CoreFileSystemEntity::operator!=(rhs));
}

list<HString>
CoreImageFile::addSubjectTags(const list<HString>& add)
{
   deduplicateTags(m_tagCache, add);
   return m_tagCache;
}

list<HString>
CoreImageFile::removeSubjectTags(const list<HString>& remove)
{
   for (auto tag : remove)
   {
      list<HString>::iterator iter = find(m_tagCache.begin(), m_tagCache.end(), tag);
      if (iter != m_tagCache.end())
         m_tagCache.erase(iter);
   }
   return m_tagCache;
}

list<HString>
CoreImageFile::readSubjectTags(HUint& fileError)
{
   fileError = 0;
   list<HString> exifSubjects;
   list<HString> iptcSubjects;
   list<HString> xmpSubjects;

   Exiv2::Image::AutoPtr image = openImage(path(), fileError);
   if (fileError == 0)
   {
      exifSubjects = readExifSubjectTags(image, fileError);

      if (fileError == 0)
      {
         m_tagCache = exifSubjects;

         iptcSubjects = readIptcSubjectTags(image, fileError);
         if (fileError == 0)
            deduplicateTags(m_tagCache, iptcSubjects);
         if (fileError == 0)
            xmpSubjects = readXmpSubjectTags(image, fileError);
         if (fileError == 0)
            deduplicateTags(m_tagCache, xmpSubjects);
         m_tagCache.sort();
      } 

      image.release();
   }

   return m_tagCache;
}

list<HString>
CoreImageFile::writeSubjectTags(HUint& fileError) 
{
   fileError = 0;

   Exiv2::Image::AutoPtr image = openImage(path(), fileError);
   if (fileError == 0)
   {
      writeExifSubjectTags(image, m_tagCache, fileError);
      if (fileError == 0)
         writeIptcSubjectTags(image, m_tagCache, fileError);
      if (fileError == 0)
        writeXmpSubjectTags(image, m_tagCache, fileError);

      image->writeMetadata();
      image.release();
   }
   return m_tagCache;
}

Exiv2::Image::AutoPtr
CoreImageFile::openImage(const HString& path, HUint& fileError) const
{
   fileError = 0;

   Exiv2::Image::AutoPtr image = Exiv2::ImageFactory::open(path);

   if (image.get())
   {
      image->readMetadata();
      fileError = 0;
   }
   else
   {
      fileError = EACCES;
   }

   return image;
}

list<HString>
CoreImageFile::readExifSubjectTags(Exiv2::Image::AutoPtr& image, HUint& fileError) 
{
   list<HString> exifSubjects;

   if (fileError == 0)
   {
      Exiv2::ExifData& exifData = image->exifData();

      if (!exifData.empty()) 
      {
         // check the image tags are set before trying to read them
         char key[] = "Exif.Image.XPKeywords";
         Exiv2::ExifKey exifKey(key);
         Exiv2::ExifData::iterator iter;
         iter = exifData.findKey(exifKey);
         
         if (iter != exifData.end())
         {
            ostringstream os;
            os << exifData[key];

            // read as byte stream then convert to unicode
            string streamString = os.str();
            HString converted = convertToWide(streamString);
            HString delimiter = H_TEXT(";");
            exifSubjects = parseDelimiters(converted, delimiter);
         }
      }
   }

   return exifSubjects;
}

void
CoreImageFile::writeExifSubjectTags(Exiv2::Image::AutoPtr& image, const list<HString>& tags, HUint& fileError) const
{
   HString delimited = makeDelimited(tags, H_TEXT(";")); 
   
   if (fileError == 0)
   {
      Exiv2::ExifData& exifData = image->exifData();

      // Remove existing Exif subject data if it exists
      Exiv2::ExifKey key("Exif.Image.XPKeywords");
      Exiv2::ExifData::iterator iter;
      for (iter = exifData.findKey(key); iter != exifData.end(); iter = exifData.findKey(key))
         exifData.erase(iter);

      // Write as unicode byte stream
      const Exiv2::byte* p = reinterpret_cast<const Exiv2::byte*> (delimited.c_str());
      Exiv2::DataValue value(p, (long)delimited.size() * 2l + 2l, 
                             Exiv2::ByteOrder::invalidByteOrder, Exiv2::TypeId::unsignedByte);

      exifData["Exif.Image.XPKeywords"] = value;
   }
}

list<HString>
CoreImageFile::readIptcSubjectTags(Exiv2::Image::AutoPtr& image, HUint& fileError) 
{
   list<HString> iptcSubjects;

   if (fileError == 0) 
   {
      Exiv2::IptcData& iptcData = image->iptcData();

      if (!iptcData.empty())
      {
         Exiv2::IptcKey key("Iptc.Application2.Keywords");
         Exiv2::IptcData::iterator iter;

         // Read as an array
         for (iter = iptcData.findKey(key); iter != iptcData.end(); iter++)
         {
            Exiv2::Iptcdatum datum = *iter;
            if (datum.key() == key.key())
            {
               Exiv2::Value::AutoPtr pValue = datum.getValue();
               string narrow = pValue.get()->toString();
               iptcSubjects.push_back(convertToWide(narrow));
            }
         }
      }
   }

   return iptcSubjects;
}

void
CoreImageFile::writeIptcSubjectTags(Exiv2::Image::AutoPtr& image, const list<HString>& tags, HUint& fileError) const
{
   if (fileError == 0)
   {
      Exiv2::IptcData& iptcData = image->iptcData();

      // Remove existing IPTC subject data if it exists
      Exiv2::IptcKey key("Iptc.Application2.Keywords");
      Exiv2::IptcData::iterator iter;
      for (iter = iptcData.findKey(key); iter != iptcData.end(); iter = iptcData.findKey(key))
         iptcData.erase(iter);

      // write as a series of entries
      for (auto item : tags)
      {
         Exiv2::Iptcdatum datum(key);
         datum.setValue (convertToNarrow(item));
         iptcData.add(datum);
      }
   }
}

list<HString>
CoreImageFile::readXmpSubjectTags(Exiv2::Image::AutoPtr& image, HUint& fileError)
{
   list<HString> xmpSubjects;

   if (fileError == 0) 
   {
      Exiv2::XmpData& xmpData = image->xmpData();
      if (!xmpData.empty())
      {
         // check the image tags are set before trying to read them
         char key[] = "Xmp.dc.subject";
         Exiv2::XmpKey xmpKey(key);
         Exiv2::XmpData::iterator iter;
         iter = xmpData.findKey(xmpKey);

         if (iter != xmpData.end())
         {
            ostringstream os;
            os << xmpData["Xmp.dc.subject"];

            string streamString = os.str();
            HString converted = convertToWide(streamString);
            HString delimiter = H_TEXT(",");
            xmpSubjects = parseDelimiters(converted, delimiter);
         }
      }
   }

   return xmpSubjects;
}

void
CoreImageFile::writeXmpSubjectTags(Exiv2::Image::AutoPtr& image, const list<HString>& tags, HUint& fileError) const
{
   if (fileError == 0) 
   {
      Exiv2::XmpData& xmpData = image->xmpData();
      
      // Remove existing XMP subject data if it exists
      Exiv2::XmpKey key ("Xmp.dc.subject");
      Exiv2::XmpData::iterator iter;
      for (iter = xmpData.findKey(key); iter != xmpData.end(); iter = xmpData.findKey(key))
         xmpData.erase(iter);

      // write as a series of entries
      for (auto item : tags)
         xmpData["Xmp.dc.subject"] = convertToNarrow(item); 
   }
}

HString
CoreImageFile::convertToWide(const string& orig) const
{
   size_t newsize = orig.size() + 1;

   unique_ptr<wchar_t> pNew (COMMON_NEW wchar_t[newsize]);

   // Convert char* string to a wchar_t* string.
   size_t convertedChars = 0;
   mbstowcs_s(&convertedChars, pNew.get(), newsize, orig.c_str(), _TRUNCATE);

   HString returnString(pNew.get());

   return returnString;
}

string
CoreImageFile::convertToNarrow(const HString& orig) const
{
   size_t newsize = orig.size() + 1;

   unique_ptr<char> pNew(COMMON_NEW char[newsize]);

   // Convert w_char_t* string to a char* string.
   size_t convertedChars = 0;
   wcstombs_s(&convertedChars, pNew.get(), newsize, orig.c_str(), _TRUNCATE);

   string returnString(pNew.get());

   return returnString;
}

list<HString>
CoreImageFile::CoreImageFile::parseDelimiters (const HString& input, const HString& delimiter) const
{
   list<HString> tags;
   HString temp = input;

   // loop over string separating by delimeter
   size_t pos = 0;
   HString token;
   while ((pos = temp.find(delimiter)) != HString::npos) 
   {
      token = temp.substr(0, pos);
      storeSubjectTag(tags, token);
      temp.erase(0, pos + delimiter.length());
   }

   // if the end of the string was not delimited, add it in
   storeSubjectTag(tags, temp);

   return tags;
}

list<HString>
CoreImageFile::CoreImageFile::storeSubjectTag(list<HString>& tags, const HString& input) const
{
   HString temp = input;

   // trim leading spaces
   while (temp.size() > 0 && temp[0] == H_TEXT(' '))
      temp.erase(0, 1);

   // add if length > 0 and not already stored
   if (temp.size() > 0 && (find (tags.begin(), tags.end(), temp) == tags.end()))
      tags.push_back(temp);

   return tags;
}

HString
CoreImageFile::makeDelimited(const list<HString>& input, const HString& delimiter) const
{
   HString delimited;

   for (auto item : input)
   {
      delimited.append(item); 
      delimited.append(delimiter);
   }

   // right trim the last delimeter
   delimited.erase(delimited.find_last_not_of(delimiter) + 1);

   return delimited; 
}

list<HString>
CoreImageFile::deduplicateTags(list<HString>& consolidatedList, const list<HString>& newList)
{
   for (auto item : newList)
   {
      list<HString>::iterator it2 = find(consolidatedList.begin(), consolidatedList.end(), item);

      if (it2 == consolidatedList.end())
         consolidatedList.push_back(item);
   }

   return consolidatedList;
}


