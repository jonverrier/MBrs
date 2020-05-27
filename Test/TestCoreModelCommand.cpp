#include "pch.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#include "CommonDefinitions.h"
#include "CommonStandardLibraryIncludes.h"

#include "Host.h"
#include "HostUserData.h"
#include "HostResources.h"
#include "Core.h"
#include "CoreModelCommand.h"
#include "Test.h"

using namespace std;

namespace TestCore
{
   TEST_CLASS(TestModelCommand)
   {
   public:

      TEST_METHOD(ConstructAndCopy)
      {
         shared_ptr<CoreCommand> pCmd1 (COMMON_NEW CoreCommand()),
                                 pCmd2 (COMMON_NEW CoreCommand());

         CoreCommandProcessor processor1, processor2;

         processor1.adoptAndProcess(pCmd1);
         processor2.adoptAndProcess(pCmd2);

         testConstructionAndCopy(processor1, processor2);         
      }

   };
}
