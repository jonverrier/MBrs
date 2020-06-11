
#include "pch.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#include "Common.h"
#include "Host.h"
#include "HostException.h"
#include "Core.h"
#include "CoreKeywords.h"

#include "Test.h"

using namespace std;

namespace TestCore
{
   TEST_CLASS(TestCoreKeywords)
   {
   public:

      TEST_METHOD(ConstuctAndCopy)
      {
         const HString catName1 = H_TEXT("CategoryCons1");
         const HString catName2 = H_TEXT("CategoryCons2");

         CoreCategoryKeywords cat1(catName1), cat2(catName2);

         testConstructionAndCopy(cat1, cat2);
      }

      TEST_METHOD(Attributes)
      {
         const HString catName1 = H_TEXT("CategoryAttr1");

         CoreCategoryKeywords cat1(catName1);

         Assert::IsTrue(cat1.category() == catName1);
         Assert::IsTrue(cat1.keywords().size() == 0);
      }

      TEST_METHOD(Operations)
      {
         const HString catName1 = H_TEXT("CategoryOp1");
         const HString keyName1 = H_TEXT("Key1");
         const HString keyName2 = H_TEXT("Key2");

         // add a keywords
         CoreCategoryKeywords cat1(catName1);
         cat1.removeAllKeywords();
         cat1.addKeyword(keyName1);

         // read it back and check
         CoreCategoryKeywords cat2(catName1);
         Assert::IsTrue(cat2.keywords().size() == 1);

         // Add another
         cat1.addKeyword(keyName2);

         // read it back and check
         CoreCategoryKeywords cat3(catName1);
         Assert::IsTrue(cat3.keywords().size() == 2);

         // remove both
         cat1.removeKeyword(keyName1);
         cat1.removeKeyword(keyName2);

         // read it back and check
         CoreCategoryKeywords cat4(catName1);
         Assert::IsTrue(cat4.keywords().size() == 0);
      }
   };
}

