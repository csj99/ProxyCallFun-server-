#pragma once
#ifndef _REGISTER_H
#define _REGISTER_H

#include "stdafx.h"

using namespace std;

class registerFunc {
private:
	typedef string DllNameType;
	typedef vector<string> FunArgType;
	typedef map<DllNameType, FunArgType> RegFunType;
public:
	//注册函数所在的dll以及其中的被注册函数
	static map<string, vector<string> > funReg;
private:
	registerFunc() = default;		//防止类被实例化
public:
	static bool findFunc(string dllName,string funcNane);
	static bool findDll(string dllName);
	static bool regFunc(DllNameType DllName,FunArgType fArg);
	static bool regDll();
};

#endif // !_REGISTER_H
