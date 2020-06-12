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

      TEST_METHOD(ConstructAndCopy)
      {
         std::shared_ptr< CoreImageListModel> pModel(COMMON_NEW CoreImageListModel());

         CoreChangeDirectoryCommand cmd1 (m_newPath, m_oldPath, pModel),
                                    cmd2 (m_oldPath, m_newPath, pModel);

         testConstructionAndCopy(cmd1, cmd2);         
      }

      TEST_METHOD(DoUndo)
      {
         std::shared_ptr< CoreImageListModel> pModel (COMMON_NEW CoreImageListModel());

         shared_ptr<CoreCommand> pCmd1 (COMMON_NEW CoreChangeDirectoryCommand(m_newPath, m_oldPath, pModel)),
                                 pCmd2 (COMMON_NEW CoreChangeDirectoryCommand(m_oldPath, m_newPath, pModel));

         CoreCommandProcessor processor1 (pModel);
         Assert::IsFalse(processor1.canUndo());
         Assert::IsFalse(processor1.canRedo());

         processor1.adoptAndDo(pCmd1);
         HString currentPath = pModel->path();
         Assert::IsTrue(currentPath == m_newPath);
         Assert::IsTrue(processor1.canUndo());
         Assert::IsFalse(processor1.canRedo());

         processor1.undo();
         currentPath = pModel->path();
         Assert::IsFalse(processor1.canUndo());
         Assert::IsTrue(processor1.canRedo());
         Assert::IsTrue(currentPath == m_oldPath);
         list<CoreImageFile> files = pModel->images();
         Assert::IsTrue(files.size() > 0); // Always > 0 image files in original directory

         processor1.redo();
         currentPath = pModel->path();
         Assert::IsTrue(currentPath == m_newPath);
         Assert::IsTrue(processor1.canUndo());
         Assert::IsFalse(processor1.canRedo());
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
