/////////////////////////////////////////
// CoreImageFile.cpp
// Copyright (c) 2020 TXPCo Ltd
/////////////////////////////////////////

#include "Common.h"
#include "CoreImageFile.h"
#include "HostUserData.h"

#include "exiv2/exiv2.hpp"
#include "CoreTimeUtil.h"

using namespace std;

Exiv2::Image::AutoPtr openImage(const HString& path, HUint& fileError);

std::filesystem::file_time_type readExifDateTime(Exiv2::Image::AutoPtr& pImage, HUint& fileError);

std::list<HString> readSubjectTags(std::list<HString>& tagCache, Exiv2::Image::AutoPtr& pImage, HUint& fileError);

std::list<HString> readExifSubjectTags(Exiv2::Image::AutoPtr& pImage, HUint& fileError);

void writeExifSubjectTags(Exiv2::Image::AutoPtr& pImage, const std::list<HString>& tags, HUint& fileError);

std::list<HString> readIptcSubjectTags(Exiv2::Image::AutoPtr& pImage, HUint& fileError);

void writeIptcSubjectTags(Exiv2::Image::AutoPtr& pImage, const std::list<HString>& tags, HUint& fileError);

std::list<HString> readXmpSubjectTags(Exiv2::Image::AutoPtr& pImage, HUint& fileError);

void writeXmpSubjectTags(Exiv2::Image::AutoPtr& pImage, const std::list<HString>& tags, HUint& fileError);

static std::list<HString> deduplicateTags(std::list<HString>& consolidatedList, const std::list<HString>& newList);

static std::list<HString> parseDelimiters(const HString& input, const HString& delimiter);

static std::list<HString> storeSubjectTag(std::list<HString>& tags, const HString& input);

static HString makeDelimited(const  std::list<HString>& input, const HString& delimiter);

static HString convertToWide(const std::string& orig);

static std::string convertToNarrow(const HString& orig);

////////////////////////////////////////////////////////////////////////////
// CoreImageFile
////////////////////////////////////////////////////////////////////////////

/* 
 * Links on how & which tags to read/write
 * https://www.exiv2.org/doc/xmpsample_8cpp-example.html
 * https://exiftool.org/forum/index.php?PHPSESSID=549060ab6264eb1b85c258c2135f19b0&topic=6591.msg32875#msg32875
 * https://docs.microsoft.com/en-gb/windows/win32/wic/-wic-photoprop-system-keywords
 *
 * Subject tags read:
 * XMP:Subject
 * IPTC : Keywords
 * EXIF : XPKeywords
 *
 * Subject tags written :
 * XMP:Subject
 * IPTC : Keywords
 * EXIF : XPKeywords
 * TODO: Consider writing XMP : LastKeywordIPTC, LastKeywordXMP
 */ 

CoreImageFile::CoreImageFile(const HString& path) :
   CoreFileSystemEntity (path),
   m_tagCache (),
   m_takenAt ()
{
   readMetaData();
}

CoreImageFile::~CoreImageFile(void)
{
}

CoreImageFile::CoreImageFile(const CoreImageFile& src) :
   CoreFileSystemEntity(src),
   m_tagCache (src.m_tagCache),
   m_takenAt (src.m_takenAt)
{

}

CoreImageFile& CoreImageFile::operator=(const CoreImageFile& src)
{
   CoreFileSystemEntity::operator=(src);
   m_tagCache = src.m_tagCache;
   m_takenAt = src.m_takenAt;

   return *this;
}

bool CoreImageFile::operator==(const CoreImageFile& rhs) const
{
   if (this == &rhs)
      return TRUE;

   return (m_tagCache == rhs.m_tagCache && m_takenAt == rhs.m_takenAt && CoreFileSystemEntity::operator==(rhs));
}

bool CoreImageFile::operator!=(const CoreImageFile& rhs) const
{
   if (this == &rhs)
      return FALSE;

   return (m_tagCache != rhs.m_tagCache || m_takenAt != rhs.m_takenAt || CoreFileSystemEntity::operator!=(rhs));
}

std::filesystem::file_time_type CoreImageFile::takenAt() const
{
   return m_takenAt;
}

list<HString> CoreImageFile::subjectTags() const
{
   return m_tagCache;
}

bool CoreImageFile::hasSubjectTag(const HString& tagToTest) const
{
   for (auto tag : m_tagCache)
   {
      if (tag == tagToTest)
         return true;
   }

   return false;
}

void CoreImageFile::readMetaData()
{
   HUint fileError = 0;


   if (!existsOnFileSystem() || (filesystem::file_size(path()) <= minFileSize))
      return;

   Exiv2::Image::AutoPtr pImage = openImage(path(), fileError);
   if (fileError == 0)
   {
      m_takenAt = readExifDateTime(pImage, fileError);
      if (fileError == 0)
         m_tagCache = readSubjectTags(m_tagCache, pImage, fileError);

      pImage.release();
   }
}

list<HString> CoreImageFile::addSubjectTags(const list<HString>& add)
{
   deduplicateTags(m_tagCache, add);
   return m_tagCache;
}

list<HString> CoreImageFile::removeSubjectTags(const list<HString>& remove)
{
   for (auto tag : remove)
   {
      list<HString>::iterator iter = find(m_tagCache.begin(), m_tagCache.end(), tag);
      if (iter != m_tagCache.end())
         m_tagCache.erase(iter);
   }
   return m_tagCache;
}

bool CoreImageFile::writeSubjectTags()
{
   HUint fileError = 0;

   if (!existsOnFileSystem() || (filesystem::file_size(path()) <= minFileSize))
      return false;

   // keep the timestamp on the filesystem constant
   filesystem::file_time_type last = filesystem::last_write_time(path());
   error_code ec;

   Exiv2::Image::AutoPtr pImage = openImage(path(), fileError);
   if (fileError == 0)
   {
      writeExifSubjectTags(pImage, m_tagCache, fileError);
      if (fileError == 0)
         writeIptcSubjectTags(pImage, m_tagCache, fileError);
      if (fileError == 0)
         writeXmpSubjectTags(pImage, m_tagCache, fileError);

      pImage->writeMetadata();
      pImage.release();

      filesystem::last_write_time(path(), last, ec);

      return true;
   }

   return false;
}

std::list<HString> CoreImageFile::actualAddSubjectTags(const std::list<HString>& add)
{
   std::list<HString> diff;

   for (auto tag : add)
   {
      if (!hasSubjectTag(tag))
         diff.push_back(tag);
   }
   return diff;
}

std::list<HString> CoreImageFile::actualRemoveSubjectTags(const std::list<HString>& remove)
{
   std::list<HString> diff;

   for (auto tag : remove)
   {
      if (hasSubjectTag(tag))
         diff.push_back(tag);
   }
   return diff;
}

Exiv2::Image::AutoPtr openImage(const HString& path, HUint& fileError) 
{
   fileError = 0;

   // test the file before trying to read it properly
   // If this passes we know it has valid extension and also valid initial contents
   int type = Exiv2::ImageFactory::getType(path);
   if (type != Exiv2::ImageType::none)
   {
      Exiv2::Image::AutoPtr image = Exiv2::ImageFactory::open(path);

      if (image.get())
      {
         image->readMetadata();
         fileError = 0;
      }
      else
         fileError = EACCES;

      return image;
   }
   fileError = EACCES;
   return Exiv2::Image::AutoPtr(nullptr);
}

std::list<HString> readSubjectTags (std::list<HString>& tagCache, Exiv2::Image::AutoPtr& pImage, HUint& fileError)
{
   list<HString> exifSubjects;
   list<HString> iptcSubjects;
   list<HString> xmpSubjects;
   list<HString> allSubjects;

   if (fileError == 0)
      exifSubjects = readExifSubjectTags(pImage, fileError);

   if (fileError == 0)
   {
      allSubjects = exifSubjects;

      iptcSubjects = readIptcSubjectTags(pImage, fileError);
      if (fileError == 0)
         deduplicateTags(tagCache, iptcSubjects);
      if (fileError == 0)
         xmpSubjects = readXmpSubjectTags(pImage, fileError);
      if (fileError == 0)
         deduplicateTags(tagCache, xmpSubjects);
      allSubjects.sort();
   } 

   return allSubjects;
}

std::filesystem::file_time_type readExifDateTime(Exiv2::Image::AutoPtr& pImage, HUint& fileError)
{
   // Initialise the time to start of epoch in case there is an error reading 
   std::chrono::time_point<std::chrono::system_clock> sctp;
   std::filesystem::file_time_type ft;

   Exiv2::ExifData& exifData = pImage->exifData();

   if (!exifData.empty())
   {
      // check the image tags are set before trying to read them
      char key[] = "Exif.Photo.DateTimeOriginal";
      Exiv2::ExifKey exifKey(key);
      Exiv2::ExifData::iterator iter;
      iter = exifData.findKey(exifKey);
      if (iter != exifData.end())
      {
         ostringstream os;
         os << exifData[key];

         // read as byte stream then convert to time
         string streamString = os.str();

         // read individual date-time components
         HInt year = 0, month = 0, day = 0, hour = 0, minute = 0, second = 0;
         chrono::time_point<chrono::system_clock> takenAt;
         int r = 0;
         r = sscanf(streamString.c_str(), "%d:%d:%d %d:%d:%d", &year, &month, &day,
                                          &hour, &minute, &second);
         if (r == 6)
         {
            // Convert to time_t
            std::chrono::seconds seconds = yearMonthDayHoursMinSecondsToSeconds(year, month, day, hour, minute, second);
            takenAt = chrono::time_point<chrono::system_clock> (seconds);
            sctp = secondsToSystemTime(std::chrono::duration_cast<std::chrono::seconds> (takenAt.time_since_epoch()));
         }
      }
   }
   
   // convert from system time to file time
   convert_tp(sctp, ft);

   return ft;
}

list<HString> readExifSubjectTags(Exiv2::Image::AutoPtr& pImage, HUint& fileError) 
{
   list<HString> exifSubjects;

   Exiv2::ExifData& exifData = pImage->exifData();

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

   return exifSubjects;
}

void writeExifSubjectTags(Exiv2::Image::AutoPtr& pImage, const list<HString>& tags, HUint& fileError) 
{
   HString delimited = makeDelimited(tags, H_TEXT(";")); 
   
   Exiv2::ExifData& exifData = pImage->exifData();

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

std::list<HString> readIptcSubjectTags(Exiv2::Image::AutoPtr& pImage, HUint& fileError) 
{
   list<HString> iptcSubjects;

   Exiv2::IptcData& iptcData = pImage->iptcData();

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
   return iptcSubjects;
}

void writeIptcSubjectTags(Exiv2::Image::AutoPtr& pImage, const list<HString>& tags, HUint& fileError) 
{
   Exiv2::IptcData& iptcData = pImage->iptcData();

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

list<HString> readXmpSubjectTags(Exiv2::Image::AutoPtr& pImage, HUint& fileError)
{
   list<HString> xmpSubjects;

   Exiv2::XmpData& xmpData = pImage->xmpData();
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
   return xmpSubjects;
}

void writeXmpSubjectTags(Exiv2::Image::AutoPtr& pImage, const list<HString>& tags, HUint& fileError) 
{
   Exiv2::XmpData& xmpData = pImage->xmpData();

   // Remove existing XMP subject data if it exists
   Exiv2::XmpKey key("Xmp.dc.subject");
   Exiv2::XmpData::iterator iter;
   for (iter = xmpData.findKey(key); iter != xmpData.end(); iter = xmpData.findKey(key))
      xmpData.erase(iter);

   // write as a series of entries
   for (auto item : tags)
      xmpData["Xmp.dc.subject"] = convertToNarrow(item);
}

static HString convertToWide(const string& orig) 
{
   size_t newsize = orig.size() + 1;

   unique_ptr<wchar_t> pNew (COMMON_NEW wchar_t[newsize]);

   // Convert char* string to a wchar_t* string.
   size_t convertedChars = 0;
   mbstowcs_s(&convertedChars, pNew.get(), newsize, orig.c_str(), _TRUNCATE);

   HString returnString(pNew.get());

   return returnString;
}

static string convertToNarrow(const HString& orig) 
{
   size_t newsize = orig.size() + 1;

   unique_ptr<char> pNew(COMMON_NEW char[newsize]);

   // Convert w_char_t* string to a char* string.
   size_t convertedChars = 0;
   wcstombs_s(&convertedChars, pNew.get(), newsize, orig.c_str(), _TRUNCATE);

   string returnString(pNew.get());

   return returnString;
}

static list<HString> parseDelimiters (const HString& input, const HString& delimiter) 
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

static list<HString> storeSubjectTag(list<HString>& tags, const HString& input) 
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

static HString makeDelimited(const list<HString>& input, const HString& delimiter) 
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

static list<HString> deduplicateTags(list<HString>& consolidatedList, const list<HString>& newList)
{
   for (auto item : newList)
   {
      list<HString>::iterator it2 = find(consolidatedList.begin(), consolidatedList.end(), item);

      if (it2 == consolidatedList.end())
         consolidatedList.push_back(item);
   }

   return consolidatedList;
}


