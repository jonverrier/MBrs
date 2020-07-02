
#include "pch.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#include "Common.h"
#include "Host.h"
#include "HostException.h"
#include "Core.h"
#include "CoreImageFile.h"

#include "Test.h"

using namespace std;

namespace TestCore
{
   TEST_CLASS(TestCoreTagCounter)
   {
      static HString tag1, tag2, tag3;
      static list<HString> list1, list2;


   public:
      TEST_CLASS_INITIALIZE(Lists)
      {
         tag1 = H_TEXT("TestTag1");
         tag2 = H_TEXT("TestTag2");
         tag3 = H_TEXT("TestTag3");

         list1.push_back(tag1);
         list1.push_back(tag2);
         list2.push_back(tag2);
         list2.push_back(tag3);
      }

      TEST_METHOD(ConstuctAndCopy)
      {
         CoreSubjectTagCounter counter1, counter2;

         counter1.addTags(list1);
         counter2.addTags(list2);

         testConstructionAndCopy(counter1, counter2);
      }

      TEST_METHOD(Attributes)
      {
         CoreSubjectTagCounter counter1, counter2;

         counter1.addTags(list1);
         counter2.addTags(list2);

         Assert::IsTrue(counter1.countOf(tag1) == CoreSubjectTagCounter::kAll);
         Assert::IsTrue(counter1.countOf(tag1) == CoreSubjectTagCounter::kAll);
         Assert::IsTrue(counter1.countOf(tag3) == CoreSubjectTagCounter::kNone);

         counter1.addTags(list2);
         Assert::IsTrue(counter1.countOf(tag3) == CoreSubjectTagCounter::kSome);
         Assert::IsTrue(counter1.countOf(tag2) == CoreSubjectTagCounter::kAll);
         Assert::IsTrue(counter1.countOf(tag3) == CoreSubjectTagCounter::kSome);
      }
   };

   HString TestCoreTagCounter::tag1, TestCoreTagCounter::tag2, TestCoreTagCounter::tag3;
   list<HString> TestCoreTagCounter::list1, TestCoreTagCounter::list2;
}

