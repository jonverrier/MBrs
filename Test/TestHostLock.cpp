#include "pch.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#include "CommonDefinitions.h"
#include "CommonStandardLibraryIncludes.h"

#include "Host.h"
#include "HostLocks.h"
#include "HostException.h"

#include "Test.h"

namespace TestHost
{
	TEST_CLASS(TestHostInterlockedCount)
	{
	public:
		
		TEST_METHOD(ConstuctAndCopy)
		{
         HostInterlockedCount count1 (1), count2(2);

         testRelationalOperators(count1, count1, count2);
		}

      TEST_METHOD(RelationalOperators)
      {
         HostInterlockedCount count1(1), count2(2);

         testRelationalOperators(count1, count1, count2);
      }

      TEST_METHOD(LockSingleThread)
      {
         // TODO: This test currently just does this on one thread to get method coverage
         // To really test function it needs 2 threads
         HostInterlockedCount count1, count2(1);

         count1.set(0);
         count1.increment();
         count1.decrement();
         count1.count();

         Assert::IsTrue (count1 < count2);
         Assert::IsTrue (count2 > count1);
      }
	};
}
