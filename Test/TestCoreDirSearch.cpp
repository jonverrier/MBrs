
#include "pch.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#include "Common.h"
#include "Host.h"
#include "HostLocks.h"
#include "HostException.h"
#include "Core.h"
#include "CoreFile.h"

#include "Test.h"

using namespace std;

namespace TestCore
{

   TEST_CLASS(TestCoreDirectorySearch)
   {
   public:

      TEST_METHOD(ConstuctAndCopy)
      {
         const HString path1 = H_TEXT(".");
         const HString path2 = H_TEXT("..");

         CoreDirectorySearch dir1(path1), dir2(path2);

         testConstructionAndCopy(dir1, dir2);
      }

      TEST_METHOD(Operations)
      {  
         const HString path1 = H_TEXT(".");

         CoreDirectorySearch dir1(path1);
         CoreDirectorySearch::SearchQueue queueImg;

         dir1.listImages (queueImg);

         Assert::IsTrue(queueImg.size() > 0);   // Always > 1 JPG file in test directory
      }
   };
}

