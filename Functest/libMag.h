#pragma once
#ifndef _LIBMAG_H
#define _LIBMAG_H

#include "stdafx.h"
#include <fstream>

#ifndef __LibLocked
#define __LibLocked		//该宏定义表示可能发生资源冲突
#endif // !__LibLocked


using namespace std;

typedef struct idx
{
	char dllname[255];
	size_t qaddr;		//idx记录dll所在lib中的位置
	size_t size;		//idx记录dll所占用lib的大小
	size_t maddr;		//idx自己所在的位置
}IDX;

typedef struct
{
	size_t qaddr;		//idx记录dll所在lib中的位置
	size_t size;		//idx记录dll所占用lib的大小
	size_t maddr;		//idx自己所在的位置
}ddr;

class libMag {
private :
	map < string, ddr> idxmap;
	fstream libfile;
	fstream idxfile;
	fstream dllfile;
	string libname;
	string idxname;
	string dllname;


	bool libOkFlag;
	bool mapInit();
	bool fileCpy(fstream &dfilie, size_t daddr, fstream &sfile, size_t saddr,size_t count);
	bool checkDll(string &dllname);
	bool refreshIdx(size_t begin);

public :
	libMag() = default;
	libMag(string s);
	~libMag();
	
	bool is_open();
	void showIdx();
	static string dirToName(string dir);
	bool findDll(string dllname,IDX &fidx);

	__LibLocked bool createLib(string libdir);		//用于创建一个新的lib库文件
	__LibLocked bool addDll(string dlldir);			//用于向当前打开的lib库中添加DLL
	__LibLocked bool subDll(string dlldir);			//用于删除当前打开的lib库中的DLL
	__LibLocked bool refreshDll(string dlldir);		//用于重新添加当前打开的lib库中的DLL
	__LibLocked bool extreDll(string dllname, string tempdir);		//用于提取指定的DLL文件到临时文件中

	__LibLocked bool callFunc(string funName,string param);
};

#endif // !_LIBMAG_H
