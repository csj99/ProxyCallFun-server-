#include "stdafx.h"

map<string,vector<string> >registerFunc::funReg = {
		{"DLL_out.testDll.dll",{"my_add","my_div"}} ,
		{"DLL_out.testDll.dll2",{"sub"}}
};

bool registerFunc::findDll(string dllName)
{
	auto fit = funReg.begin();
	for (; fit != funReg.end(); fit++)
	{
		if (fit->first == dllName)
			return true;
	}

	return false;
}

bool registerFunc::findFunc(string dllName,string funcNane)
{
	if (!findDll(dllName))return false;

	auto fit = funReg.begin();
	for (; fit != funReg.end(); fit++)
	{

	}
	return true;
}