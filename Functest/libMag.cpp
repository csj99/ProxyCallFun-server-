#include "libMag.h"

libMag::libMag(string filedir)
{
	char scrc[40];
	uint32_t icrc = 0;
	size_t pos;
	if ((pos = filedir.find(".lib", filedir.size() - 4)) == string::npos)
	{		//检查后缀是否合法
		cout << "bad file name!!!" << endl;
		libOkFlag = FALSE;
		return;
	}

	/*************************检查和打开lib文件************************************/
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
	{			//检查文件是否合法
		cout << filedir << "is bad libfile!!!" << endl;
		libOkFlag = FALSE;
		return;
	}
	cout << filedir << " was opened successed!!!" << endl;

	/*************************检查和打开idx文件************************************/
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
	{			//检查文件是否合法
		cout << "The idx file: " << filedir << " was illegal" << endl;
		libOkFlag = FALSE;
		return;
	}
	libOkFlag = TRUE;
	cout << filedir << " was opened successed!!!" << endl;
	/*************************初始化map************************************/
	mapInit();

	showIdx();
}

bool libMag::addDll(string dlldir)
{
	IDX idxtemp;
	/*************************检查DLL是否合法**************************/
	if (!checkDll(dlldir))return FALSE;

	/************************检查是否以存在同名DLL***********************/
	string dllname = dirToName(dlldir);

	if (findDll(dllname, idxtemp))
	{
		cout << "已经存在此DLL： " << dllname << endl;
		return FALSE;
	}


	/************************打开该DLL************************/
	dllfile.open(dlldir, ios::binary | ios::out | ios::in);
	if (!dllfile.is_open())
	{
		cout << "打开DLL： " << dllname << " 失败！！！" << endl;
		return FALSE;
	}

	/*******************开始将DLL写入到Lib中*******************/
	dllfile.seekg(0, ios::end);
	libfile.seekp(0, ios::end);
	size_t dllsize = (size_t)dllfile.tellg();
	idxtemp.size = (size_t)dllfile.tellg() + 1;
	idxtemp.qaddr = (size_t)libfile.tellp();
	fileCpy(libfile, (size_t)libfile.tellp(), dllfile, 0, dllsize + 1);

	cout << "The idx.size is: " << idxtemp.size << endl;

	/******************将索引插入到IDX文件中******************/
	idxfile.clear();
	idxfile.seekp(0, ios::end);
	idxtemp.maddr = (size_t)idxfile.tellp();
	strcpy(idxtemp.dllname, dllname.c_str());
	idxfile.write((char *)&idxtemp, sizeof(idxtemp));

	/******************将索引插入到map文件中******************/
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
	/*************************检查DLL是否合法**************************/
	if (!checkDll(dllname))return FALSE;

	/************************检查是否以存在该DLL***********************/
	if (!findDll(dllname, idxtemp))
	{
		cout << "无法删除，不存在此DLL： " << dllname << endl;
		return FALSE;
	}

	this->libfile.clear();
	this->idxfile.clear();
	/************************删除Lib中的DLL***********************/
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

	/************************删除IDX中的索引***********************/
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

	/*************************检查DLL是否合法**************************/
	checkDll(dllname);
	/************************检查是否以存在该DLL***********************/
	if (!findDll(dllname, idxtemp))
	{
		cout << "无法提取，不存在此DLL： \"" << dllname << '\"' << endl;
		return FALSE;
	}

	/*************************创建DLL临时文件************************/
	dllfile.close();
	dllfile.open(tempdir, ios::binary | ios::out | ios::trunc);
	if (!dllfile.is_open())
	{
		cout << "dll临时文件创建失败：" << libfile.is_open() << endl;
		return FALSE;
	}

	/*******************将lib中的内容读取到临时DLL中******************/
	libfile.clear();
	if (!fileCpy(dllfile, 0, libfile, idxtemp.qaddr, idxtemp.size))
	{
		dllfile.close();
		cout << "写入临时DLL失败！！！" << endl;
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
	{		//检查后缀是否合法
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
		cout << "绑定lib文件：" << libdir << " 和idx文件：" << idxfir << " 成功" << endl;
		libOkFlag = TRUE;
	}

	return TRUE;
}

bool libMag::mapInit()
{
	/************************检查是lib管理器是否成功打开库文件***********************/
	if (!this->libOkFlag) { cout << "map dose not initialize successed!!!" << endl; return false; }

	/************************定位idx文件到目录地址管理器是否成功打开库文件***********************/
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
	/*************************检查DLL是否合法**************************/
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
	/*********************检查检查是否已经成功绑定lib**********************/
	if (!libOkFlag)
	{
		cout << "未绑定lib，无法执行添加操作" << endl;
		return FALSE;
	}
	/*************************检查后缀是否合法****************************/
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
	if (!extreDll(funcName, "../tempDll/temp.dll")) { cout << "提取失败" << endl; return false; }

	size_t last_pos = funcName.rfind(".");
	size_t sec_pos = funcName.rfind(".", last_pos - 1);
	string fun(funcName.substr(sec_pos + 1, last_pos - sec_pos - 1));

	HINSTANCE s = LoadLibrary("../tempDll/temp.dll");

	if (s)
	{
		cout << "dll 打开成功！！！" << endl;
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
