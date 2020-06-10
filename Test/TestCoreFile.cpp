
#include "pch.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#include "Common.h"
#include "Host.h"
#include "HostException.h"
#include "Core.h"
#include "CoreFile.h"

#include "Test.h"

using namespace std;

namespace TestCore
{
   TEST_CLASS(TestCoreFile)
   {
   public:

      TEST_METHOD(ConstuctAndCopy)
      {
         const HString path1 = H_TEXT("TestFile1");
         const HString path2 = H_TEXT("TestFile2");

         CoreFileSystemEntity file1(path1), file2(path2);

         testConstructionAndCopy(file1, file2);
      }

      TEST_METHOD(Attributes)
      {
         const HString path1 = H_TEXT("TestFile1");
         const HString path2 = H_TEXT("TestFile2");
         const HString path3 = H_TEXT("Test.jpg"); // This file does exist in the Test directory 

         std::ofstream outfile2 (path2);
         outfile2 << "Test data in file" << std::endl;
         outfile2.close();

         // file doesnt exist
         CoreFileSystemEntity fileThatDoesNotExist(path1);
         Assert::IsFalse (fileThatDoesNotExist.existsOnFileSystem());

         // file does exist
         CoreFileSystemEntity fileThatExists(path2);
         Assert::IsTrue(fileThatExists.existsOnFileSystem());

         // Check correct date stamp is returned
         filesystem::file_time_type last_write_time = fileThatExists.lastWriteTime();
         fileThatExists.setLastWriteTime(last_write_time);
         Assert::IsTrue(last_write_time == fileThatExists.lastWriteTime());

         // file does not exist, therefore is not a file or a directory
         Assert::IsFalse(fileThatDoesNotExist.isFile());
         Assert::IsFalse(fileThatDoesNotExist.isDirectory());

         // Its a file and not a directory
         Assert::IsTrue(fileThatExists.isFile());
         Assert::IsFalse(fileThatExists.isDirectory());

         // Its an image file file and not a directory
         CoreFileSystemEntity imageFile (path3);
         Assert::IsTrue(imageFile.isFile());
         Assert::IsTrue(imageFile.isImageFile());
         Assert::IsTrue(imageFile.filename().length() > 0);
      }

   };

   TEST_CLASS(TestCoreDirectory)
   {
   public:

      TEST_METHOD(ConstuctAndCopy)
      {
         const HString path1 = H_TEXT(".");
         const HString path2 = H_TEXT("..");

         CoreDirectory file1(path1), file2(path2);

         testConstructionAndCopy(file1, file2);
      }

      TEST_METHOD(Operations)
      {  
         const HString path1 = H_TEXT("emptySandbox");
         const HString path2 = H_TEXT("realSandbox");

         filesystem::create_directories(path1);
         filesystem::create_directories(path2);
         std::ofstream("realSandbox/file1.txt");
         std::ofstream("realSandbox/file2.txt");
         std::ofstream("realSandbox/file1.jpg");
         std::ofstream("realSandbox/file2.png");

         CoreDirectory emptyDir(path1), realDir(path2);

         list<HString> contents, dirs;
         emptyDir.listImagesDirs(contents, dirs);
         Assert::IsTrue(contents.size() == 0);

         realDir.listImagesDirs(contents, dirs);
         Assert::IsTrue(contents.size() == 2); // 2 image files called .jpg 
      }

      TEST_METHOD(defaultImageDir)
      {
         HString pathIn = CoreFileSystemEntity::loadImageDirectory();

         // A default directory always exists
         Assert::IsTrue(pathIn.size() > 0);

         HString pathOut = H_TEXT("testImageDir");
         filesystem::create_directories(pathOut);
         CoreFileSystemEntity::saveImageDirectory(pathOut);

         HString newPath = CoreFileSystemEntity::loadImageDirectory();

         // The new path must be a component of the saved path
         Assert::IsTrue(newPath.find(pathOut) > 0);
      }
   };
}

