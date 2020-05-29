#include "libMag.h"

libMag::libMag(string filedir)
{
	char scrc[40];
	uint32_t icrc = 0;
	size_t pos;
	if ((pos = filedir.find(".lib", filedir.size() - 4)) == string::npos)
	{		//����׺�Ƿ�Ϸ�
		cout << "bad file name!!!" << endl;
		libOkFlag = FALSE;
		return;
	}

	/*************************���ʹ�lib�ļ�************************************/
	this->libname = filedir;
	libfile.open(filedir, ios::binary | ios::in | ios::out);
	if (!libfile.is_open())
	{
		cout << "Can't open the lib file: " << filedir << endl;
		libOkFlag = FALSE;
		return;
	}

	libfile.read(scrc, 30); scrc[30] = 0;
	libfile.read((char *)&icrc, 3);
	if (strcmp(scrc, "Function Server Libs Body v1.0") || ((icrc & 0x00ffffff) != 0x1a0a0d))
	{			//����ļ��Ƿ�Ϸ�
		cout << filedir << "is bad libfile!!!" << endl;
		libOkFlag = FALSE;
		return;
	}
	cout << filedir << " was opened successed!!!" << endl;

	/*************************���ʹ�idx�ļ�************************************/
	filedir.erase(pos, 4);
	filedir.insert(pos, ".idx");

	this->idxname = filedir;
	idxfile.open(filedir, ios::binary | ios::in | ios::out);
	if (!idxfile.is_open())
	{
		cout << "Can't open the idx file: " << filedir << endl;
		libOkFlag = FALSE;
		return;
	}

	idxfile.read(scrc, 31); scrc[31] = 0;
	idxfile.read((char *)&icrc, 3);

	if (strcmp(scrc, "Function Server Libs Index v1.0") || ((icrc & 0x00ffffff) != 0x1a0a0d))
	{			//����ļ��Ƿ�Ϸ�
		cout << "The idx file: " << filedir << " was illegal" << endl;
		libOkFlag = FALSE;
		return;
	}
	libOkFlag = TRUE;
	cout << filedir << " was opened successed!!!" << endl;
	/*************************��ʼ��map************************************/
	mapInit();

	showIdx();
}

bool libMag::addDll(string dlldir)
{
	IDX idxtemp;
	/*************************���DLL�Ƿ�Ϸ�**************************/
	if (!checkDll(dlldir))return FALSE;

	/************************����Ƿ��Դ���ͬ��DLL***********************/
	string dllname = dirToName(dlldir);

	if (findDll(dllname, idxtemp))
	{
		cout << "�Ѿ����ڴ�DLL�� " << dllname << endl;
		return FALSE;
	}


	/************************�򿪸�DLL************************/
	dllfile.open(dlldir, ios::binary | ios::out | ios::in);
	if (!dllfile.is_open())
	{
		cout << "��DLL�� " << dllname << " ʧ�ܣ�����" << endl;
		return FALSE;
	}

	/*******************��ʼ��DLLд�뵽Lib��*******************/
	dllfile.seekg(0, ios::end);
	libfile.seekp(0, ios::end);
	size_t dllsize = (size_t)dllfile.tellg();
	idxtemp.size = (size_t)dllfile.tellg() + 1;
	idxtemp.qaddr = (size_t)libfile.tellp();
	fileCpy(libfile, (size_t)libfile.tellp(), dllfile, 0, dllsize + 1);

	cout << "The idx.size is: " << idxtemp.size << endl;

	/******************���������뵽IDX�ļ���******************/
	idxfile.clear();
	idxfile.seekp(0, ios::end);
	idxtemp.maddr = (size_t)idxfile.tellp();
	strcpy(idxtemp.dllname, dllname.c_str());
	idxfile.write((char *)&idxtemp, sizeof(idxtemp));

	/******************���������뵽map�ļ���******************/
	pair<string, ddr> mapTp;
	mapTp.first = idxtemp.dllname;
	mapTp.second.maddr = idxtemp.maddr;
	mapTp.second.qaddr = idxtemp.qaddr;
	mapTp.second.size = idxtemp.size;
	this->idxmap.insert(mapTp);

	idxfile.flush();
	libfile.flush();
	dllfile.close();
	return TRUE;
}

bool libMag::findDll(string fdllname, IDX &fidx)
{
	std::map<string, ddr>::const_iterator fit = this->idxmap.find(fdllname);
	if (fit != this->idxmap.end())
	{
		strcpy(fidx.dllname, fdllname.c_str());
		fidx.maddr = fit->second.maddr;
		fidx.qaddr = fit->second.qaddr;
		fidx.size = fit->second.size;
		return TRUE;
	}
	else
	{
		fidx.dllname[0] = 0;
		fidx.maddr = 0;
		fidx.qaddr = 0;
		fidx.size = 0;
	}
	idxfile.clear();
	return FALSE;
}

bool libMag::subDll(string dllname)
{
	IDX idxtemp;
	/*************************���DLL�Ƿ�Ϸ�**************************/
	if (!checkDll(dllname))return FALSE;

	/************************����Ƿ��Դ��ڸ�DLL***********************/
	if (!findDll(dllname, idxtemp))
	{
		cout << "�޷�ɾ���������ڴ�DLL�� " << dllname << endl;
		return FALSE;
	}

	this->libfile.clear();
	this->idxfile.clear();
	/************************ɾ��Lib�е�DLL***********************/
	char *buf = new char[0xffff];
	libfile.seekp(0, ios::end);
	size_t sunSize = (size_t)libfile.tellp();
	size_t handleSize = sunSize - (idxtemp.size + idxtemp.qaddr);
	libfile.seekp(idxtemp.qaddr, ios::beg);

	for (; handleSize / 0xffff != 0; handleSize -= 0xffff)
	{
		libfile.seekp(idxtemp.size, ios::cur);
		libfile.read(buf, 0xffff);
		libfile.seekp(-(long)(idxtemp.size + 0xffff), ios::cur);
		libfile.write(buf, 0xffff);
	}
	libfile.seekp(idxtemp.size, ios::cur);
	libfile.read(buf, handleSize);
	libfile.seekp(-(long)(idxtemp.size + handleSize), ios::cur);
	libfile.write(buf, handleSize);

	libfile.flush();
	libfile.close();
	FILE *libfp = fopen(this->libname.c_str(), "ab+");
	int libno = _fileno(libfp);
	_chsize(libno, sunSize - idxtemp.size);
	fclose(libfp);
	libfile.open(this->libname, ios::binary | ios::in | ios::out);

	/************************ɾ��IDX�е�����***********************/
	idxfile.seekp(0, ios::end);
	sunSize = (size_t)idxfile.tellp();
	handleSize = sunSize - (idxtemp.maddr + sizeof(idxtemp));

	idxfile.seekp(idxtemp.maddr, ios::beg);
	for (; handleSize / 0xffff != 0; handleSize -= 0xffff)
	{
		idxfile.seekp(sizeof(idxtemp), ios::cur);
		idxfile.read(buf, 0xffff);
		idxfile.seekp(-(long)(sizeof(idxtemp) + 0xffff), ios::cur);
		idxfile.write(buf, 0xffff);
	}
	idxfile.seekp(sizeof(idxtemp), ios::cur);
	idxfile.read(buf, handleSize);
	idxfile.seekp(-(long)(sizeof(idxtemp) + handleSize), ios::cur);
	idxfile.write(buf, handleSize);

	idxfile.flush();
	idxfile.clear();
	idxfile.close();
	FILE *idxfp = fopen(this->idxname.c_str(), "ab+");
	int idxno = _fileno(idxfp);
	_chsize(idxno, sunSize - sizeof(IDX));
	fclose(idxfp);
	idxfile.open(this->idxname, ios::binary | ios::in | ios::out);

	refreshIdx(idxtemp.maddr);
	mapInit();

	delete[] buf;
	return TRUE;
}

bool libMag::extreDll(string dllname, string tempdir)
{
	IDX idxtemp;

	/*************************���DLL�Ƿ�Ϸ�**************************/
	checkDll(dllname);
	/************************����Ƿ��Դ��ڸ�DLL***********************/
	if (!findDll(dllname, idxtemp))
	{
		cout << "�޷���ȡ�������ڴ�DLL�� \"" << dllname << '\"' << endl;
		return FALSE;
	}

	/*************************����DLL��ʱ�ļ�************************/
	dllfile.close();
	dllfile.open(tempdir, ios::binary | ios::out | ios::trunc);
	if (!dllfile.is_open())
	{
		cout << "dll��ʱ�ļ�����ʧ�ܣ�" << libfile.is_open() << endl;
		return FALSE;
	}

	/*******************��lib�е����ݶ�ȡ����ʱDLL��******************/
	libfile.clear();
	if (!fileCpy(dllfile, 0, libfile, idxtemp.qaddr, idxtemp.size))
	{
		dllfile.close();
		cout << "д����ʱDLLʧ�ܣ�����" << endl;
		return FALSE;
	}

	dllfile.close();

	return TRUE;

}

bool libMag::createLib(string libdir)
{
	size_t pos;
	const char *libcrc = "Function Server Libs Body v1.0";
	const char *idxcrc = "Function Server Libs Index v1.0";
	uint32_t crc = 0x1a0a0d;
	if ((pos = libdir.find(".lib", libdir.size() - 4)) == string::npos)
	{		//����׺�Ƿ�Ϸ�
		cout << "Create lib filed bad file name!!!" << endl;
		return FALSE;
	}

	libfile.open(libdir, ios::binary | ios::out | ios::trunc);
	if (!libfile.is_open())
	{
		cout << "file open flag" << libfile.is_open() << endl;
		return FALSE;
	}

	libfile.write(libcrc, strlen(libcrc));
	libfile.write((char *)&crc, 3);

	cout << "Create the libfile: \"" << libdir << "\" successed!!!" << endl;
	libfile.close();

	string idxfir(libdir);
	idxfir.erase(pos, 4);
	idxfir.insert(pos, ".idx");
	idxfile.open(idxfir, ios::binary | ios::out | ios::trunc);
	if (!idxfile.is_open())
	{
		cout << "file open flag" << idxfile.is_open() << endl;
		return FALSE;
	}

	idxfile.write(idxcrc, strlen(idxcrc));
	idxfile.write((char *)&crc, 3);

	cout << "Create the libfile: \"" << idxfir << "\" successed!!!" << endl;
	idxfile.close();

	this->libname = libdir; this->idxname = libdir;
	libfile.open(libdir, ios::binary | ios::out | ios::in);
	idxfile.open(idxfir, ios::binary | ios::out | ios::in);
	if (libfile.is_open() && idxfile.is_open())
	{
		cout << "��lib�ļ���" << libdir << " ��idx�ļ���" << idxfir << " �ɹ�" << endl;
		libOkFlag = TRUE;
	}

	return TRUE;
}

bool libMag::mapInit()
{
	/************************�����lib�������Ƿ�ɹ��򿪿��ļ�***********************/
	if (!this->libOkFlag) { cout << "map dose not initialize successed!!!" << endl; return false; }

	/************************��λidx�ļ���Ŀ¼��ַ�������Ƿ�ɹ��򿪿��ļ�***********************/
	this->idxmap.clear();
	this->idxfile.clear();
	this->idxfile.seekg(34, ios::beg);

	while (this->idxfile.peek() != EOF)
	{
		IDX idxtemp;
		std::pair<string, ddr> mapTp;
		this->idxfile.read((char *)&idxtemp, sizeof(IDX));

		if (!this->idxfile.eof())
		{
			mapTp.first = idxtemp.dllname;
			mapTp.second.maddr = idxtemp.maddr;
			mapTp.second.qaddr = idxtemp.qaddr;
			mapTp.second.size = idxtemp.size;

			this->idxmap.insert(mapTp);
		}
	}
	return true;
}

bool libMag::refreshIdx(size_t begin)
{
	IDX idx;
	idxfile.clear();
	idxfile.seekp(begin, ios::beg);
	while (idxfile.peek() != EOF)
	{
		idxfile.read((char *)&idx, sizeof(idx));
		idx.maddr -= sizeof(idx);

		if (!idxfile.eof())
		{
			idxfile.seekp(-(long)sizeof(IDX), ios::cur);
			idxfile.write((char *)&idx, sizeof(idx));
			break;
		}
	}

	idxfile.clear();
	return TRUE;
}

void libMag::showIdx()
{
	cout << "<the list of dll>: " << endl;
	for (auto it = this->idxmap.begin(); it != this->idxmap.end(); it++)
	{
		cout << "The idxname: " << it->first << " maddr: "
			<< it->second.maddr << " qaddr: " << it->second.qaddr << " size: " << it->second.size << endl;
	}

	return;
}

bool libMag::fileCpy(fstream &dfilie, size_t daddr, fstream& sfile, size_t saddr, size_t count)
{
	const size_t arrsize = 100;
	char *buf = new char[arrsize];
	size_t handleSize = count;

	sfile.seekg(saddr, ios::beg);
	dfilie.seekp(daddr, ios::beg);
	for (; handleSize / arrsize != 0; handleSize -= arrsize)
	{
		sfile.read(buf, arrsize);
		dfilie.write(buf, arrsize);
	}
	sfile.read(buf, handleSize);
	dfilie.write(buf, handleSize);

	dfilie.flush();
	sfile.flush();
	dfilie.clear();
	sfile.clear();
	delete[] buf;
	return TRUE;
}

bool libMag::refreshDll(string dlldir)
{
	/*************************���DLL�Ƿ�Ϸ�**************************/
	if (!checkDll(dlldir))return FALSE;

	IDX idxtemp;
	string dllname = dirToName(dlldir);
	if (findDll(dllname, idxtemp))
		if (!subDll(dllname))
			return FALSE;
	if (!addDll(dlldir))
		return FALSE;
	return TRUE;
}

string libMag::dirToName(string dir)
{
	string dllname = dir;
	size_t pos;
	while ((pos = dllname.find("../")) != string::npos)
		dllname.erase(pos, 3);

	while ((pos = dllname.find("/")) != string::npos)
		dllname.replace(dllname.begin() + pos, dllname.begin() + pos + 1, ".");

	return dllname;
}

bool libMag::checkDll(string &dllname)
{
	/*********************������Ƿ��Ѿ��ɹ���lib**********************/
	if (!libOkFlag)
	{
		cout << "δ��lib���޷�ִ����Ӳ���" << endl;
		return FALSE;
	}
	/*************************����׺�Ƿ�Ϸ�****************************/
	if (dllname.find(".dll", dllname.size() - 4) == string::npos)
	{
		cout << "bad file name :\"" << dllname << '\"' << endl;
		return FALSE;
	}
	return TRUE;
}

bool libMag::is_open()
{
	return libfile.is_open() && idxfile.is_open();
}

bool libMag::callFunc(string funcName, string param)
{
	if (!extreDll(funcName, "../tempDll/temp.dll")) { cout << "��ȡʧ��" << endl; return false; }

	size_t last_pos = funcName.rfind(".");
	size_t sec_pos = funcName.rfind(".", last_pos - 1);
	string fun(funcName.substr(sec_pos + 1, last_pos - sec_pos - 1));

	HINSTANCE s = LoadLibrary("../tempDll/temp.dll");

	if (s)
	{
		cout << "dll �򿪳ɹ�������" << endl;
		try
		{
			FUNADDR funcPoniter = (FUNADDR)GetProcAddress(s, fun.c_str());
			if (!funcPoniter)
				throw - 1;
			paraParser parser;
			
			parser.analyArg(param);
			cout << funcPoniter(*parser.para()) << endl;
			FreeLibrary(s);
		}
		catch (int s)
		{
			if (s == -1)
				cout << "the function " + fun + " was not found!!!" << endl;
			else
				cout << s << " is undefine expection!!!" << endl;
			return false;
		}
	}
	else
	{
		cout << "The " + funcName + " load filed!!!" << endl;
		return false;
	}
	return true;
}

libMag::~libMag()
{
	libfile.close();
	idxfile.close();
	dllfile.close();
}
