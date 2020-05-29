// my_div.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"

extern "C"
{
	_declspec(dllexport) double my_div(int a, int b);
}

double my_div(int a, int b)
{
	return a / b;
}
