#include "stdafx.h"
#include "CppUnitTest.h"
#include "../Functest/libMag.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace std;
namespace UnitTestForFun
{		
	TEST_CLASS(UnitTest1)
	{
	public:
		
		TEST_METHOD(TestMethod1)
		{
			// TODO: �ڴ�������Դ���
			string testdllname("../Lib/testfun.lib");
			libMag testlib;
			//cin >> testdllname;
			Assert::IsTrue(testlib.createLib(testdllname));
			Assert::IsTrue(testlib.addDll("../DLL_out/my_add.dll"));
			Assert::IsTrue(testlib.addDll("../DLL_out/my_sub.dll"));
		}

		TEST_METHOD(TestMethod4)
		{
			// TODO: �ڴ�������Դ���
			libMag testlib("../Lib/testfun.lib");
			string testdllname("DLL_out.testDll.dll");
			Assert::IsTrue(testlib.refreshDll("../DLL_out/my_sub.dll"));
		}

		TEST_METHOD(TestMethod3)
		{
			// TODO: �ڴ�������Դ���
			libMag testlib("../Lib/testfun.lib");
			string testdllname("DLL_out.testDll.dll");
			IDX idx{"",36865,36865};
			//cin >> testdllname;
			Assert::IsTrue(testlib.findDll(testdllname,idx));
			Assert::IsTrue((idx.size==36865));
		}
	};
}