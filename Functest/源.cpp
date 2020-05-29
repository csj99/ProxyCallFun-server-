#include "stdafx.h"

using namespace std;

typedef enum mainselect
{
	add = 1,sub,replace,extren,call,nosele
}mainSelect;

mainSelect mainMenu();

libMag serverLib("../Lib/testfun.lib");
std::mutex libMut;

int main()
{
	csCommunication serve;
	serve.WaitForClient();
	::system("pause");
}

void Menu()
{
	//;
	mainSelect ms;

	while ((ms = mainMenu()) != mainSelect::nosele)
	{
		switch (ms)
		{
		case add:
		{
			string addDll;
			printf("请输入您要添加的dll库：\n");
			cin >> addDll;
			if (serverLib.addDll(addDll))
			{
				cout << "添加成功，新加入的DLL名为：" << serverLib.dirToName(addDll) << endl;
			}
			else
				cout << "添加失败：" + addDll + " 可能不存在！！！" << endl;
		}break;

		case sub:
		{
			string subDll;
			printf("请输入您要删除的dll库：\n");
			cin >> subDll;
			if (serverLib.subDll(subDll))
			{
				cout << subDll + " 删除成功" << endl;
			}
			else
				cout << "删除失败：" + subDll + " 可能不存在！！！" << endl;

		}break;

		case replace:
		{
			string replaceDll;
			printf("请输入您要替换的dll库：\n");
			cin >> replaceDll;
			if (serverLib.refreshDll(replaceDll))
			{
				cout << replaceDll + " 替换成功" << endl;
			}
			else
				cout << "替换失败：" + libMag::dirToName(replaceDll) + " 可能不存在！！！" << endl;
		}break;

		case extren:
		{
			string extrenDll, extrenDir;
			cout << "请输入您要提取的dll库：" << endl;
			cin >> extrenDll;
			cout << "请输入您提取出的dll库名：" << endl;
			cin >> extrenDir;
			if (serverLib.extreDll(extrenDll, extrenDir))
			{
				cout << extrenDll + " 替换成功" << endl;
			}
			else
				cout << "替换失败：" + extrenDll + " 可能不存在！！！" << endl;
		}break;

		case call:
		{
			::system("cls");
			serverLib.showIdx();
			string callfname;
			printf("please input the name that you want to call: \n");
			cin >> callfname;
			serverLib.callFunc(callfname, "{\"int\": \"6 \",\"int\": \" 5\",\"char\": [\" 5\",\" 6\"]}");
		}break;

		case nosele:
		default: break;
		}
		system("pause");
	}

	::system("pause");

}

mainSelect mainMenu()
{
	int ms;
	::system("cls");

	cout << "1). 添加库\n" ;
	cout << "2). 删除库\n" ;
	cout << "3). 替换库\n" ;
	cout << "4). 提取库\n" ;
	cout << "5). 调用库\n";
	cout << "6). 退出\n";
	cout << "请输入您需要的操作" << endl;
	ms = _getch()-0x30;
	while (ms < mainselect::add || ms > mainselect::nosele)
		ms = _getch() - 0x30;

	return (mainSelect)ms;
}