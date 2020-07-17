#include "pch.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#include "Common.h"
#include "Host.h"
#include "HostUserData.h"
#include "HostResources.h"
#include "Core.h"
#include "CoreModelCommand.h"
#include "CoreMbrsModelCommand.h"
#include "Test.h"

using namespace std;

void makePaths(HString& newPath, HString& oldPath)
{
   oldPath = std::filesystem::current_path(); // oldPath is wherever we start
   const HString newSubPath = H_TEXT("newPath");

   filesystem::create_directories(newSubPath);
   std::filesystem::current_path(newSubPath); // newPath is now a subdirectory
   newPath = std::filesystem::current_path();
   std::filesystem::current_path(oldPath);    // go back to where we started
}

namespace TestCore
{
   TEST_CLASS(TestModelCommand)
   {
      static HString m_newPath, m_oldPath;

   public:
      TEST_CLASS_INITIALIZE(Paths)
      {
         makePaths(m_newPath, m_oldPath);
      }

      TEST_METHOD(ConstructAndCopyCompoundCommand)
      {
         HString path = H_TEXT("test.jpg");
         std::vector<HString> sel;
         sel.push_back ( path );

         list<HString> addTags = { H_TEXT("NewTag1"), H_TEXT("NewTag2") };
         list<HString> removeTags = { H_TEXT("NewTag3"), H_TEXT("NewTag4") };

         std::shared_ptr< CoreImageListModel> pModel(COMMON_NEW CoreImageListModel());
         std::shared_ptr< CoreImageListSelection> pSelection(COMMON_NEW CoreImageListSelection(sel));

         CoreCompoundImageTagChangeCommand cmd1(addTags, removeTags, pModel, pSelection),
                                           cmd2(addTags, addTags, pModel, pSelection);

         testConstructionAndCopy(cmd1, cmd2);
      }


      TEST_METHOD(ConstructAndCopyImageSelection)
      {
         std::vector<HString> imagePaths1 = { H_TEXT("Test1.jpg") };
         std::vector<HString> imagePaths2 = { H_TEXT("Test2.jpg") };

         CoreImageListSelection sel1 (imagePaths1);
         CoreImageListSelection sel2 (imagePaths2);

         testConstructionAndCopy(sel1, sel2);
      }

      TEST_METHOD(DoUndoCompoundTag)
      {
         HString path = std::filesystem::absolute(H_TEXT("test.JPG"));
         std::vector<HString> sel;
         sel.push_back(path);

         CoreImageFile file1(path);
         std::list<HString> tags = file1.subjectTags();
         list<HString> addTags = { H_TEXT("NewTag1"), H_TEXT("NewTag2")};
         list<HString> removeTags = { H_TEXT("NewTag3"), H_TEXT("NewTag4") };

         std::shared_ptr< CoreImageListModel> pModel(COMMON_NEW CoreImageListModel(H_TEXT(".")));
         std::shared_ptr< CoreImageListSelection> pSelection(COMMON_NEW CoreImageListSelection(sel));
         shared_ptr<CoreCommand> pCompoundCmd(COMMON_NEW CoreCompoundImageTagChangeCommand(addTags, removeTags, pModel, pSelection));

         CoreCommandProcessor processor(pModel);
         Assert::IsFalse(processor.canUndo());
         Assert::IsFalse(processor.canRedo());

         // Add a tag
         processor.adoptAndDo(pCompoundCmd);
         CoreImageFile file2(path);
         tags = file2.subjectTags();
         Assert::IsTrue(find(tags.begin(), tags.end(), *(addTags.begin())) != tags.end());
         Assert::IsTrue(processor.canUndo());
         Assert::IsFalse(processor.canRedo());

         // Undo to remove it
         processor.undo();
         CoreImageFile file3(path);
         tags = file3.subjectTags();
         Assert::IsTrue(find(tags.begin(), tags.end(), *(addTags.begin())) == tags.end());
         Assert::IsFalse(processor.canUndo());
         Assert::IsTrue(processor.canRedo());
      }

      TEST_METHOD(Filtering)
      {
         std::shared_ptr< CoreImageListModel> pModel(COMMON_NEW CoreImageListModel(H_TEXT("."))); // 2 images stored in working directory for test

         Assert::IsTrue(pModel->images().size() == 3);
         Assert::IsTrue(pModel->imagesWrittenIn(2014).size() == 0);
         Assert::IsTrue(pModel->imagesWrittenIn(2015).size() == 3); // all images taken in 2015
         Assert::IsTrue(pModel->imagesWrittenIn(2015, 1).size() == 0);
         Assert::IsTrue(pModel->imagesWrittenIn(2015, 7).size() == 2); // all images taken in 2015 
      }

      TEST_METHOD(Benchmark)
      {
         // Record start time
         auto start = std::chrono::high_resolution_clock::now();

         // Read lots of images
         std::shared_ptr< CoreImageListModel> pModel(COMMON_NEW CoreImageListModel(H_TEXT("D:\\data\\pictures"))); // Many images stored in this directory

         // Record end time
         auto finish = std::chrono::high_resolution_clock::now();

         std::chrono::duration<double> elapsed = finish - start;
         auto count = elapsed.count();
         std::cout << "Elapsed time: " << count << " s\n";
      }
   };

   HString TestModelCommand::m_newPath;
   HString TestModelCommand::m_oldPath;
}
