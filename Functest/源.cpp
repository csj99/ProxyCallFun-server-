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
			printf("��������Ҫ��ӵ�dll�⣺\n");
			cin >> addDll;
			if (serverLib.addDll(addDll))
			{
				cout << "��ӳɹ����¼����DLL��Ϊ��" << serverLib.dirToName(addDll) << endl;
			}
			else
				cout << "���ʧ�ܣ�" + addDll + " ���ܲ����ڣ�����" << endl;
		}break;

		case sub:
		{
			string subDll;
			printf("��������Ҫɾ����dll�⣺\n");
			cin >> subDll;
			if (serverLib.subDll(subDll))
			{
				cout << subDll + " ɾ���ɹ�" << endl;
			}
			else
				cout << "ɾ��ʧ�ܣ�" + subDll + " ���ܲ����ڣ�����" << endl;

		}break;

		case replace:
		{
			string replaceDll;
			printf("��������Ҫ�滻��dll�⣺\n");
			cin >> replaceDll;
			if (serverLib.refreshDll(replaceDll))
			{
				cout << replaceDll + " �滻�ɹ�" << endl;
			}
			else
				cout << "�滻ʧ�ܣ�" + libMag::dirToName(replaceDll) + " ���ܲ����ڣ�����" << endl;
		}break;

		case extren:
		{
			string extrenDll, extrenDir;
			cout << "��������Ҫ��ȡ��dll�⣺" << endl;
			cin >> extrenDll;
			cout << "����������ȡ����dll������" << endl;
			cin >> extrenDir;
			if (serverLib.extreDll(extrenDll, extrenDir))
			{
				cout << extrenDll + " �滻�ɹ�" << endl;
			}
			else
				cout << "�滻ʧ�ܣ�" + extrenDll + " ���ܲ����ڣ�����" << endl;
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

	cout << "1). ��ӿ�\n" ;
	cout << "2). ɾ����\n" ;
	cout << "3). �滻��\n" ;
	cout << "4). ��ȡ��\n" ;
	cout << "5). ���ÿ�\n";
	cout << "6). �˳�\n";
	cout << "����������Ҫ�Ĳ���" << endl;
	ms = _getch()-0x30;
	while (ms < mainselect::add || ms > mainselect::nosele)
		ms = _getch() - 0x30;

	return (mainSelect)ms;
}