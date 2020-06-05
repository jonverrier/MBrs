#include "pch.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#include "Common.h"
#include "Host.h"
#include "HostStringLoader.h"
#include "HostResources.h"

#include "Test.h"
#include "winerror.h"

namespace TestHost
{
   TEST_CLASS(TestHostStringLoader)
   {
   public:

      TEST_METHOD(LoadSystem)
      {
         HString out;

         HostStringLoader::loadSystemResourceString(ERROR_FILE_NOT_FOUND, out);
         Assert::IsTrue(out.length() > 0);
      }

      TEST_METHOD(LoadPackage)
      {
         HString out;
         HostStringLoader::loadResourceString(HOST_PACKAGE_NAME, IDS_HOSTSYSTEMTYPE,
                                              out);
         Assert::IsTrue (out.length() > 0);
      }

      TEST_METHOD(LoadWithParms)
      {
         HString out, val1(H_TEXT("Banana"));

         HostStringLoader::loadResourceStringWithParms(HOST_PACKAGE_NAME, IDS_HOSTSYSTEMTYPE,
            out,
            val1, val1,
            val1, val1);
         Assert::IsTrue(out.length() > 0);
      }
   };
}
