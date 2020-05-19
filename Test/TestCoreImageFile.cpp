
#include "pch.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#include "CommonDefinitions.h"
#include "CommonStandardLibraryIncludes.h"

#include "Host.h"
#include "HostException.h"
#include "Core.h"
#include "CoreFile.h"

#include "Test.h"

using namespace std;

void readFile();

namespace TestCore
{
   TEST_CLASS(TestCoreImageFile)
   {
   public:

      TEST_METHOD(ConstuctAndCopy)
      {
         const HString path1 = H_TEXT("TestFile1");
         const HString path2 = H_TEXT("TestFile2");

         CoreImageFile file1(path1), file2(path2);

         testConstructionAndCopy(file1, file2);
      }

      TEST_METHOD(addSubjects)
      {
         const HString path = H_TEXT("Test.jpg");
         HUint error = 0;

         CoreImageFile image (path);

         Assert::IsTrue(image.isImageFile());
         Assert::IsTrue(image.existsOnFileSystem());

         // Read existing tags (before), add some (added), then merge the lists for a 'before and added' 
         // against which can test the end result
         list<HString> tagsBefore = image.readSubjectTags(error);
         list<HString> tagsIn = { H_TEXT("TestTag1"), H_TEXT("TestTag2") };
         list<HString> tagsBeforeAndAdded = tagsBefore;
         for (auto item : tagsIn)
            if (find (tagsBeforeAndAdded.begin(), tagsBeforeAndAdded.end(), item) == tagsBeforeAndAdded.end())
               tagsBeforeAndAdded.push_back(item);

         image.addSubjectTags(tagsIn);
         image.writeSubjectTags(error);
         list<HString> tagsAfter = image.readSubjectTags(error);

         tagsBeforeAndAdded.sort();
         tagsAfter.sort();

         Assert::IsTrue(tagsAfter == tagsBeforeAndAdded);
      }
      TEST_METHOD(removeSubjects)
      {
         const HString path = H_TEXT("Test2.jpg");
         HUint error = 0;

         CoreImageFile image(path);

         Assert::IsTrue(image.isImageFile());
         Assert::IsTrue(image.existsOnFileSystem());


         list<HString> tagsBefore = image.readSubjectTags(error);
         list<HString> tagsIn = { H_TEXT("TestTag1"), H_TEXT("TestTag2") };

         image.addSubjectTags(tagsIn);
         image.writeSubjectTags(error);

         // Remove the tags we just added
         image.removeSubjectTags(tagsIn);
         image.writeSubjectTags(error);

         list<HString> tagsAfter = image.readSubjectTags(error);

         tagsBefore.sort();
         tagsAfter.sort();

         Assert::IsTrue(tagsAfter == tagsBefore);
      }
   };
}

void readFile()
{
   char* file = "test.jpg";

   Exiv2::Image::AutoPtr image = Exiv2::ImageFactory::open(file);
   assert(image.get() != 0);
   image->readMetadata();

   Exiv2::ExifData& exifData = image->exifData();

   ostringstream os1, os2;

   os1 << exifData["Exif.Image.XPKeywords"];

   HString tag(H_TEXT("Tag1fromtest;Tag2fromtest;"));
   const Exiv2::byte* p = reinterpret_cast<const Exiv2::byte*> (tag.c_str());

   Exiv2::DataValue value(p, (long)tag.size() * 2l + 2l, Exiv2::ByteOrder::invalidByteOrder, Exiv2::TypeId::unsignedByte);

   exifData["Exif.Image.XPKeywords"] = value;
     
   exifData["Exif.Photo.UserComment"]
      = "charset=\"Unicode\" Unicode Exif comment added with Exiv2";

   image->writeMetadata();
   image.release();

   image = Exiv2::ImageFactory::open(file);
   assert(image.get() != 0);
   image->readMetadata();

   exifData = image->exifData();

   os2 << exifData["Exif.Image.XPKeywords"];

   return; 
}
