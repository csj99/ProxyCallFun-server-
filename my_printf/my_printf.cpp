// my_printf.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include <stdio.h>

extern "C"
{
	__declspec(dllexport) void my_printf(char *a,...);
}

void my_printf(char *a,...)
{
	va_list vp;
	va_start(vp, a);
	vprintf(a,vp);
}