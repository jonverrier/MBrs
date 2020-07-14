
#include "pch.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#include "Common.h"
#include "Host.h"
#include "HostException.h"
#include "Core.h"
#include "CoreDateFilter.h"

#include "Test.h"

using namespace std;


namespace TestCore
{
   TEST_CLASS(TestCoreDateFilter)
   {
   public:

      TEST_METHOD(ConstuctAndCopy)
      {
         std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
         CoreDateFilter filter1 (now, CoreDateFilter::EPeriod::kMonth),
                        filter2 (now, CoreDateFilter::EPeriod::kYear);

         testConstructionAndCopy(filter1, filter2);
      }

      TEST_METHOD(SaveAndLoad)
      {
         std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
         CoreDateFilter filter1(now, CoreDateFilter::EPeriod::kMonth), filter2;

         filter1.save();
         filter2.load();

         Assert::IsTrue(filter2.date() == filter1.date());
         Assert::IsTrue(filter2.period() == CoreDateFilter::EPeriod::kMonth);
      }

   };
}
