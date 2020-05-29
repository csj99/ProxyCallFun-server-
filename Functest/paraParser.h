#pragma once
#pragma once
#ifndef _PARA_PARSER_H
#define _PARA_PARSER_H

#include "stdafx.h"
#include <fstream>

using namespace std;

#define ARG_SIZE 128
typedef struct argarr
{
	char a[ARG_SIZE];
}Arg;

class paraParser
{
public:
	paraParser()
	{
		this->sflag = false;
		::memset(&this->arg,0,sizeof(Arg));
		ap = arg.a;
	}

	~paraParser()
	{
		for (auto i : freePrt)
			free(i);

	}

private:
	Arg arg;
	va_list ap;
	vector<void *> freePrt;
	bool sflag;

	size_t findVaildLetter(const string& str, char Letter, size_t pos);
public:

	template<typename T> bool templatefPara( const vector<string>& val);
	bool transPara( const string& keyword, const vector<string>& val);
	Arg *para();
	bool analyArg(string& para);
	void clear();

	bool flag();
};

#endif