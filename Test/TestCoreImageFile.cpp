
#include "pch.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#include "Common.h"
#include "Host.h"
#include "HostException.h"
#include "Core.h"
#include "CoreFile.h"
#include "CoreImageFile.h"
#include "CoreTimeUtil.h"
#include "Test.h"

using namespace std;

namespace TestCore
{
   TEST_CLASS(TestCoreImageFile)
   {
   public:

      TEST_METHOD(ConstuctAndCopy)
      {
         // These two files exist in home bin directory
         const HString path1 = H_TEXT("Test.jpg");
         const HString path2 = H_TEXT("test7.jpg");

         CoreImageFile file1(path1), file2(path2);

         testConstructionAndCopy(file1, file2);
      }

      TEST_METHOD(Taken)
      {
         // These two files exist in home bin directory
         const HString path1 = H_TEXT("Test.jpg");

         CoreImageFile file1(path1);

         std::filesystem::file_time_type ft = file1.takenAt();
         std::chrono::time_point<std::chrono::system_clock> sctp;
         convert_tp(ft, sctp);

         time_t t = std::chrono::system_clock::to_time_t(sctp);
         struct tm tm;
         time_s(&tm, &t);
         Assert::IsTrue(tm.tm_year == 115); // 2015
      }

      TEST_METHOD(addSubjects)
      {
         const HString path = H_TEXT("Test.jpg");

         CoreImageFile image (path);

         Assert::IsTrue(image.isImageFile());
         Assert::IsTrue(image.existsOnFileSystem());

         // Read existing tags (before), add some (added), then merge the lists for a 'before and added' 
         // against which can test the end result
         list<HString> tagsBefore = image.subjectTags();
         list<HString> tagsIn = { H_TEXT("TestTag1"), H_TEXT("TestTag2") };
         list<HString> tagsBeforeAndAdded = tagsBefore;
         for (auto item : tagsIn)
            if (find (tagsBeforeAndAdded.begin(), tagsBeforeAndAdded.end(), item) == tagsBeforeAndAdded.end())
               tagsBeforeAndAdded.push_back(item);

         image.addSubjectTags(tagsIn);
         image.writeSubjectTags();

         CoreImageFile image2(path);
         list<HString> tagsAfter = image2.subjectTags();

         tagsBeforeAndAdded.sort();
         tagsAfter.sort();

         Assert::IsTrue(tagsAfter == tagsBeforeAndAdded);
      }
      TEST_METHOD(removeSubjects)
      {
         const HString path = H_TEXT("Test2.jpg");

         CoreImageFile image(path);

         Assert::IsTrue(image.isImageFile());
         Assert::IsTrue(image.existsOnFileSystem());


         list<HString> tagsBefore = image.subjectTags();
         list<HString> tagsIn = { H_TEXT("TestTag1"), H_TEXT("TestTag2") };

         image.addSubjectTags(tagsIn);
         image.writeSubjectTags();

         // Remove the tags we just added
         image.removeSubjectTags(tagsIn);
         image.writeSubjectTags();

         CoreImageFile image2(path);
         list<HString> tagsAfter = image2.subjectTags();

         tagsBefore.sort();
         tagsAfter.sort();

         Assert::IsTrue(tagsAfter == tagsBefore);
      }

      TEST_METHOD(actualAddRemove)
      {
         const HString path = H_TEXT("Test2.jpg");

         CoreImageFile image(path);

         Assert::IsTrue(image.isImageFile());
         Assert::IsTrue(image.existsOnFileSystem());

         list<HString> tagsBefore = image.subjectTags();
         list<HString> tagsIn = { H_TEXT("TestTag1"), H_TEXT("TestTag2") };
         list<HString> tagsIn2 = { H_TEXT("TestTag3"), H_TEXT("TestTag4") };

         image.addSubjectTags(tagsIn);

         // If we try to re-add the same, the actual left is 0. The actual we could remove is 2
         Assert::IsTrue(image.actualAddSubjectTags(tagsIn).size() == 0);
         Assert::IsTrue(image.actualRemoveSubjectTags(tagsIn).size() == 2);

         // If we try to re-add two different, the actual left is 2. The actual we could remove is 0
         Assert::IsTrue(image.actualAddSubjectTags(tagsIn2).size() == 2);
         Assert::IsTrue(image.actualRemoveSubjectTags(tagsIn2).size() == 0);

      }
   };
}

