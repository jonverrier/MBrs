#include "pch.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#include "CommonDefinitions.h"
#include "CommonStandardLibraryIncludes.h"

#include "Host.h"
#include "HostUserData.h"
#include "HostResources.h"

#include "Test.h"

using namespace std;

namespace TestHost
{
   TEST_CLASS(TestHostUserData)
   {
   public:

      TEST_METHOD(ConstructAndCopy)
      {
         HostUserData data1(H_TEXT("TestKey - ConstructAndCopy1")), data2(H_TEXT("TestKey - ConstructAndCopy2"));

         testConstructionAndCopy(data1, data2);
         
         // Clean up
         data1.removeAllData();
         data2.removeAllData();
      }

      TEST_METHOD(ReadWriteUint)
      {
         HUint testValue = 100;
         HString appKey(H_TEXT("TestKey - ReadWriteUint"));
         HString testKey(H_TEXT("ReadWriteUint"));

         HostUserData data(appKey);

         data.writeUint(testKey, testValue);

         Assert::IsTrue(data.readUint(testKey) == testValue);

         // Clean up
         data.removeAllData();
      }

      TEST_METHOD(ReadWriteString)
      {
         HString testValue = H_TEXT("testValue");
         HString appKey(H_TEXT("TestKey - ReadWriteString"));
         HString testKey(H_TEXT("ReadWriteString"));

         HostUserData data(appKey);

         data.writeString (testKey, testValue);

         Assert::IsTrue(data.readString(testKey) == testValue);

         // Clean up
         data.removeAllData();
      }

      TEST_METHOD(ReadWriteMultiString)
      {
         vector<HString> testValue = { H_TEXT("testValue"), H_TEXT("testValue") };
         HString appKey(H_TEXT("TestKey - ReadWriteMultiString"));
         HString testKey(H_TEXT("ReadWriteMultiString"));

         HostUserData data(appKey);

         data.writeMultiString(testKey, testValue);

         Assert::IsTrue(data.readMultiString(testKey) == testValue);

         // Clean up
         data.removeAllData();
      }

      TEST_METHOD(Remove)
      {
         HString appKey(H_TEXT("TestKey - Remove"));
         HUint testValue = 100;
         HString testKey(H_TEXT("TestKey"));

         HostUserData data(appKey);

         data.writeUint(testKey, testValue);
         data.removeData(testKey);

         Assert::IsTrue(data.isDataStoredAt(testKey) == false);

         data.writeUint(testKey, testValue);
         data.removeAllData();

         Assert::IsTrue(data.isDataStoredAt (testKey) == false);
      }

      TEST_METHOD(PictureDirectory)
      {
         HString dir = HostUserData::defaultImageDirectory();

         Assert::IsTrue(dir.size() > 0);
      }
   };
}
