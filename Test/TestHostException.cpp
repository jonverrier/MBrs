#include "pch.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#include "Common.h"
#include "Host.h"
#include "HostLocks.h"
#include "HostException.h"

#include "Test.h"
#include "winerror.h"

namespace TestHost
{
   TEST_CLASS(TestHostException)
   {
   public:

      TEST_METHOD(ConstuctAndCopy)
      {
         const HChar* pFile = H_TEXT(__FILE__);
         HInt line = __LINE__;

         //////////////////////
         // Test Host Exception
         //////////////////////
         HostException exOS(MAKE_HRESULT(SEVERITY_ERROR, FACILITY_WIN32, ERROR_ACCESS_DENIED), pFile, line);
         HostException exOS2(MAKE_HRESULT(SEVERITY_ERROR, FACILITY_WIN32, ERROR_INVALID_FUNCTION), pFile, line + 1);

         testConstructionAndCopy(exOS, exOS2);
      }

      TEST_METHOD(Attributes)
      {
         const HChar* pFile = H_TEXT(__FILE__);
         HInt line = __LINE__;
         HostException exOS2(MAKE_HRESULT(SEVERITY_ERROR, FACILITY_WIN32, ERROR_INVALID_FUNCTION), pFile, line);

         Assert::IsTrue(exOS2.errorCode() == MAKE_HRESULT(SEVERITY_ERROR, FACILITY_WIN32, ERROR_INVALID_FUNCTION));
         Assert::IsTrue(exOS2.sourceFilename() == HString(pFile));
         Assert::IsTrue(exOS2.sourceLineNumber() == line);
         Assert::IsTrue(exOS2.formattedAsString().length() > 0);
      }

      TEST_METHOD(Logger)
      {
         const HChar* pFile = H_TEXT(__FILE__);
         HInt line = __LINE__;

         HostExceptionLogger logger;
         HostException exOS2(MAKE_HRESULT(SEVERITY_ERROR, FACILITY_WIN32, ERROR_INVALID_FUNCTION), pFile, line + 1);

         logger.logException(
            exOS2.errorCode(),
            exOS2.sourceFilename(),
            exOS2.sourceLineNumber(),
            exOS2.formattedAsString());

         logger.logAssertionFailure(
            H_TEXT("Test Assertion Error Expression"),
            exOS2.sourceFilename(),
            exOS2.sourceLineNumber());
      }
   };
}
