// testDll.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include <string>
extern "C"
{
	__declspec(dllexport) int my_add(int a, int b);
}

int my_add(int a,int b)
{
	return a+b;
}

