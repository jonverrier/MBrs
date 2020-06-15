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

      TEST_METHOD(ConstructAndCopyDirCommand)
      {
         std::shared_ptr< CoreImageListModel> pModel(COMMON_NEW CoreImageListModel());
         std::shared_ptr< CoreSelection> pSelection (COMMON_NEW CoreSelection());

         CoreChangeDirectoryCommand cmd1 (m_newPath, m_oldPath, pModel, pSelection),
                                    cmd2 (m_oldPath, m_newPath, pModel, pSelection);

         testConstructionAndCopy(cmd1, cmd2);         
      }

      TEST_METHOD(ConstructAndCopyAddCommand)
      {
         HString path = H_TEXT("test.jpg");
         std::list<HString> sel;
         sel.push_back ( path );

         HString newTag1(H_TEXT("NewTag1"));
         HString newTag2(H_TEXT("NewTag2"));

         std::shared_ptr< CoreImageListModel> pModel(COMMON_NEW CoreImageListModel());
         std::shared_ptr< CoreImageListSelection> pSelection(COMMON_NEW CoreImageListSelection(sel));

         CoreAddImageTagCommand cmd1(newTag1, pModel, pSelection),
            cmd2(newTag2, pModel, pSelection);

         testConstructionAndCopy(cmd1, cmd2);
      }

      TEST_METHOD(ConstructAndCopyRemoveCommand)
      {
         HString path = H_TEXT("test.jpg");
         std::list<HString> sel;
         sel.push_back(path);

         HString newTag1(H_TEXT("OldTag1"));
         HString newTag2(H_TEXT("OldTag2"));

         std::shared_ptr< CoreImageListModel> pModel(COMMON_NEW CoreImageListModel());
         std::shared_ptr< CoreImageListSelection> pSelection(COMMON_NEW CoreImageListSelection(sel));

         CoreRemoveImageTagCommand cmd1(newTag1, pModel, pSelection),
            cmd2(newTag2, pModel, pSelection);

         testConstructionAndCopy(cmd1, cmd2);
      }

      TEST_METHOD(ConstructAndCopyImageSelection)
      {
         std::list<HString> imagePaths1 = { H_TEXT("Test1.jpg") };
         std::list<HString> imagePaths2 = { H_TEXT("Test2.jpg") };

         CoreImageListSelection sel1 (imagePaths1);
         CoreImageListSelection sel2 (imagePaths2);

         testConstructionAndCopy(sel1, sel2);
      }

      TEST_METHOD(DoUndoDirChange)
      {
         std::shared_ptr< CoreImageListModel> pModel (COMMON_NEW CoreImageListModel());
         std::shared_ptr< CoreSelection> pSelection(COMMON_NEW CoreSelection());

         shared_ptr<CoreCommand> pCmd1 (COMMON_NEW CoreChangeDirectoryCommand(m_newPath, m_oldPath, pModel, pSelection)),
                                 pCmd2 (COMMON_NEW CoreChangeDirectoryCommand(m_oldPath, m_newPath, pModel, pSelection));

         CoreCommandProcessor processor1 (pModel);
         Assert::IsFalse(processor1.canUndo());
         Assert::IsFalse(processor1.canRedo());

         // Change directory
         processor1.adoptAndDo(pCmd1);
         HString currentPath = pModel->path();
         Assert::IsTrue(currentPath == m_newPath);
         Assert::IsTrue(processor1.canUndo());
         Assert::IsFalse(processor1.canRedo());

         // Undo to change back
         processor1.undo();
         currentPath = pModel->path();
         Assert::IsFalse(processor1.canUndo());
         Assert::IsTrue(processor1.canRedo());
         Assert::IsTrue(currentPath == m_oldPath);
         list<CoreImageFile> files = pModel->images();
         Assert::IsTrue(files.size() > 0); // Always > 0 image files in original directory

         // redo to change again
         processor1.redo();
         currentPath = pModel->path();
         Assert::IsTrue(currentPath == m_newPath);
         Assert::IsTrue(processor1.canUndo());
         Assert::IsFalse(processor1.canRedo());
      }

      TEST_METHOD(DoUndoAddTag)
      {
         HString path = std::filesystem::absolute (H_TEXT("test.JPG"));
         std::list<HString> sel;
         sel.push_back(path);

         HString newTag(H_TEXT("NewTag"));

         std::shared_ptr< CoreImageListModel> pModel(COMMON_NEW CoreImageListModel(H_TEXT(".")));
         std::shared_ptr< CoreImageListSelection> pSelection(COMMON_NEW CoreImageListSelection(sel));
         shared_ptr<CoreCommand> pCmd(COMMON_NEW CoreAddImageTagCommand(newTag, pModel, pSelection));

         CoreCommandProcessor processor(pModel);
         Assert::IsFalse(processor.canUndo());
         Assert::IsFalse(processor.canRedo());

         // Add a tag
         processor.adoptAndDo(pCmd);
         CoreImageFile file1(path); 
         std::list<HString> tags = file1.subjectTags();
         Assert::IsTrue(find (tags.begin(), tags.end(), newTag) != tags.end());
         Assert::IsTrue(processor.canUndo());
         Assert::IsFalse(processor.canRedo());

         // Undo to remove it
         processor.undo();
         CoreImageFile file2(path);
         tags = file2.subjectTags();
         Assert::IsTrue(find(tags.begin(), tags.end(), newTag) == tags.end());
         Assert::IsFalse(processor.canUndo());
         Assert::IsTrue(processor.canRedo());

         // Redo to add it back again
         processor.redo();
         CoreImageFile file3(path);
         tags = file3.subjectTags();
         Assert::IsTrue(find(tags.begin(), tags.end(), newTag) != tags.end());
         Assert::IsTrue(processor.canUndo());
         Assert::IsFalse(processor.canRedo());

         // Undo at the end - so next run passes
         processor.undo();
      }

      TEST_METHOD(DoUndoRemoveTag)
      {
         HString path = std::filesystem::absolute(H_TEXT("test.JPG"));
         std::list<HString> sel;
         sel.push_back(path);

         HString newTag(H_TEXT("NewTag"));

         std::shared_ptr< CoreImageListModel> pModel(COMMON_NEW CoreImageListModel(H_TEXT(".")));
         std::shared_ptr< CoreImageListSelection> pSelection(COMMON_NEW CoreImageListSelection(sel));
         shared_ptr<CoreCommand> pAddCmd(COMMON_NEW CoreAddImageTagCommand(newTag, pModel, pSelection));
         shared_ptr<CoreCommand> pRemoveCmd(COMMON_NEW CoreRemoveImageTagCommand(newTag, pModel, pSelection));

         CoreCommandProcessor processor(pModel);
         Assert::IsFalse(processor.canUndo());
         Assert::IsFalse(processor.canRedo());

         // Add a tag
         processor.adoptAndDo(pAddCmd);
         CoreImageFile file1(path);
         std::list<HString> tags = file1.subjectTags();
         Assert::IsTrue(find(tags.begin(), tags.end(), newTag) != tags.end());
         Assert::IsTrue(processor.canUndo());
         Assert::IsFalse(processor.canRedo());

         // Addanother commnd to remove it
         processor.adoptAndDo(pRemoveCmd);
         CoreImageFile file2(path);
         tags = file2.subjectTags();
         Assert::IsTrue(find(tags.begin(), tags.end(), newTag) == tags.end());
         Assert::IsTrue(processor.canUndo());
         Assert::IsFalse(processor.canRedo());

         // Undo to add it back
         processor.undo();
         CoreImageFile file3(path);
         tags = file3.subjectTags();
         Assert::IsTrue(find(tags.begin(), tags.end(), newTag) != tags.end());
         Assert::IsTrue(processor.canUndo());
         Assert::IsTrue(processor.canRedo());

         // Redo to remove it back again
         processor.redo();
         CoreImageFile file4(path);
         tags = file4.subjectTags();
         Assert::IsTrue(find(tags.begin(), tags.end(), newTag) == tags.end());
         Assert::IsTrue(processor.canUndo());
         Assert::IsFalse(processor.canRedo());
      }

      TEST_METHOD(Filtering)
      {
         std::shared_ptr< CoreImageListModel> pModel(COMMON_NEW CoreImageListModel(H_TEXT("."))); // 2 images stored in working directory for test

         Assert::IsTrue(pModel->images().size() == 2);
         Assert::IsTrue(pModel->imagesFor(2014).size() == 0);
         Assert::IsTrue(pModel->imagesFor(2015).size() == 2); // both taken in 2015
         Assert::IsTrue(pModel->imagesFor(2015, 6).size() == 0);
         Assert::IsTrue(pModel->imagesFor(2015, 7).size() == 2); // both taken in july 2015
      }
   };

   HString TestModelCommand::m_newPath;
   HString TestModelCommand::m_oldPath;
}
