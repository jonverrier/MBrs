
#include "pch.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#include "Common.h"
#include "Host.h"
#include "HostException.h"
#include "Core.h"
#include "CoreFile.h"
#include "CoreImageFile.h"
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

      TEST_METHOD(defaultDir)
      {
         HString pathIn = CoreImageFile::loadImageDirectory();

         // A default directory always exists
         Assert::IsTrue(pathIn.size() > 0);

         HString pathOut = H_TEXT("test");
         CoreImageFile::saveImageDirectory(pathOut);

         pathIn = CoreImageFile::loadImageDirectory();

         // A default directory always exists
         Assert::IsTrue(pathIn == pathOut);
      }
   };
}

