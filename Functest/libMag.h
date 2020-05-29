#pragma once
#ifndef _LIBMAG_H
#define _LIBMAG_H

#include "stdafx.h"
#include <fstream>

#ifndef __LibLocked
#define __LibLocked		//�ú궨���ʾ���ܷ�����Դ��ͻ
#endif // !__LibLocked


using namespace std;

typedef struct idx
{
	char dllname[255];
	size_t qaddr;		//idx��¼dll����lib�е�λ��
	size_t size;		//idx��¼dll��ռ��lib�Ĵ�С
	size_t maddr;		//idx�Լ����ڵ�λ��
}IDX;

typedef struct
{
	size_t qaddr;		//idx��¼dll����lib�е�λ��
	size_t size;		//idx��¼dll��ռ��lib�Ĵ�С
	size_t maddr;		//idx�Լ����ڵ�λ��
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

	__LibLocked bool createLib(string libdir);		//���ڴ���һ���µ�lib���ļ�
	__LibLocked bool addDll(string dlldir);			//������ǰ�򿪵�lib�������DLL
	__LibLocked bool subDll(string dlldir);			//����ɾ����ǰ�򿪵�lib���е�DLL
	__LibLocked bool refreshDll(string dlldir);		//����������ӵ�ǰ�򿪵�lib���е�DLL
	__LibLocked bool extreDll(string dllname, string tempdir);		//������ȡָ����DLL�ļ�����ʱ�ļ���

	__LibLocked bool callFunc(string funName,string param);
};

#endif // !_LIBMAG_H
