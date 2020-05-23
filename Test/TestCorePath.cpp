
#include "pch.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#include "CommonDefinitions.h"
#include "CommonStandardLibraryIncludes.h"

#include "Host.h"
#include "HostException.h"
#include "Core.h"
#include "CoreCompoundName.h"

#include "Test.h"

using namespace std;

void testFormatParse(CorePathParser& parser, const HString& path);

namespace TestCore
{
   TEST_CLASS(TestCorePath)
   {
   public:

      TEST_METHOD(ConstuctAndCopy)
      {
         const vector<HString> path1 = { H_TEXT("TestFile1") };
         const vector<HString> path2 = { H_TEXT("TestFile2") };

         CoreCompoundName compound1 (path1, true), compound2 (path2, false);

         testConstructionAndCopy(compound1, compound2);
         Assert::IsTrue(compound1.isFullPath() == true);
         Assert::IsTrue(compound2.isFullPath() == false);
      }

      TEST_METHOD(FormatAndParse)
      {
         HString path; 
         CoreWindowsPathParser parser;

         // Simple file name only 
         path = H_TEXT("TestFile1");
         testFormatParse(parser, path);

         // Director and file name  
         path = H_TEXT("dir\\TestFile1");
         testFormatParse(parser, path);

         // Multiple directories and file name  
         path = H_TEXT("dir\\dir\\TestFile1");
         testFormatParse(parser, path);

         // Full local path to multiple directories and file name  
         path = H_TEXT("\\dir\\dir\\TestFile1");
         testFormatParse(parser, path);

         // Full local path from root to multiple directories and file name  
         path = H_TEXT("c:\\dir\\dir\\TestFile1");
         testFormatParse(parser, path);

         // Full local path from root to multiple directories and file name  
         path = H_TEXT("\\server\\dir\\TestFile1");
         testFormatParse(parser, path);
      }

   };
}

void testFormatParse (CorePathParser& parser, const HString& path)
{
   HString formatted;
   CoreCompoundName compound;

   compound = parser.parsePath(path);
   formatted = parser.formatPath(compound);

   Assert::IsTrue(formatted == path);
}

