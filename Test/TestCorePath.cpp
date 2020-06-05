
#include "pch.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#include "Common.h"
#include "Host.h"
#include "HostException.h"
#include "Core.h"
#include "CoreCompoundName.h"

#include "Test.h"

using namespace std;

void testFormatParse(CorePathParser& parser, const HString& path);
void testUIFormat(CorePathParser& parser, CorePathParser& formatter, const HString& path, const HString& ui);

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
         CoreUIPathFormatter formatter;

         // Simple file name only 
         path = H_TEXT("TestFile1");
         testFormatParse(parser, path);
         testUIFormat(parser, formatter, path, H_TEXT("TestFile1"));

         // Director and file name  
         path = H_TEXT("dir\\TestFile1");
         testFormatParse(parser, path);
         testUIFormat(parser, formatter, path, H_TEXT("dir > TestFile1"));

         // Multiple directories and file name  
         path = H_TEXT("dir\\dir\\TestFile1");
         testFormatParse(parser, path);
         testUIFormat(parser, formatter, path, H_TEXT("dir > dir > TestFile1"));

         // Full local path to multiple directories and file name  
         path = H_TEXT("\\dir\\dir\\TestFile1");
         testFormatParse(parser, path);
         testUIFormat(parser, formatter, path, H_TEXT("dir > dir > TestFile1"));

         // Full local path from root to multiple directories and file name  
         path = H_TEXT("c:\\dir\\dir\\TestFile1");
         testFormatParse(parser, path);
         testUIFormat(parser, formatter, path, H_TEXT("c: > dir > dir > TestFile1"));

         // Full local path from root to multiple directories and file name  
         path = H_TEXT("\\server\\dir\\TestFile1");
         testFormatParse(parser, path);
         testUIFormat(parser, formatter, path, H_TEXT("server > dir > TestFile1"));
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

void testUIFormat(CorePathParser& parser, CorePathParser& formatter, const HString& path, const HString& ui)
{
   HString formatted;
   CoreCompoundName compound;

   compound = parser.parsePath(path);
   formatted = formatter.formatPath(compound);

   Assert::IsTrue(formatted == ui);
}