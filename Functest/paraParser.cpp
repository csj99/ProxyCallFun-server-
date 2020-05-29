#include "paraParser.h"

//功能返回最近一次解析参数是否成功
bool paraParser::flag()
{
	return this->sflag;
}

//功能：清除参数数组
void paraParser::clear()
{
	memset(&this->arg, 0, sizeof(Arg));
	this->ap = arg.a;
}

Arg * paraParser::para()
{
	return &(this->arg);
}

template <typename T>
bool paraParser::templatefPara(const vector<string>& val)
{
	if (val.size() > 1)					//如果传入值为一个数组
	{
		T* a = (T *)::malloc(sizeof(T)*val.size());
		this->freePrt.push_back(a);

		if (a == nullptr)return false;
		for (size_t i = 0; i < val.size(); i++)
		{
			stringstream ss;
			ss << val.at(i);
			ss >> a[i];
		}
		(*(T**)(((ap) += _INTSIZEOF(T*)) - _INTSIZEOF(T*))) = std::move(a);
	}
	else
	{
		T a;
		stringstream ss;
		ss << val.at(0);
		ss >> a;
		(*(T*)(((ap) += _INTSIZEOF(T)) - _INTSIZEOF(T))) = std::move(a);
	}
	return true;
}

bool paraParser::transPara(const string& keywold, const vector<string>& val)
{
	if (keywold == "int")
		templatefPara<int>(val);
	else if (keywold == "short")
		templatefPara<short>(val);
	else if (keywold == "string")
		templatefPara<string>(val);
	else if (keywold == "float")
		templatefPara<float>(val);
	else if (keywold == "double")
		templatefPara<double>(val);
	else
	{
		this->sflag = false;
		return sflag;
	}

	return sflag;
}

//寻找字符串中不在字符串中的字符
size_t paraParser::findVaildLetter(const string &str, char Letter, size_t start)
{
	if (start >= str.length())return -1;
	size_t pos = start;
	string::const_iterator sit = str.begin() + start;
	for (; sit != str.end(); pos++, sit++)
	{
		//遇到字符串引号就直接跳过去
		if (*sit == '\"')
		{
			sit++; pos++;
			while (*sit == '\"')
			{
				sit++; pos++;
			}
		}
		if (*sit == Letter)
			return pos;
	}
	return -1;
}

bool paraParser::analyArg(string& para)
{
	//参数列表，键为类型，值存储在对应的vector
	list<pair<string, vector<string>>> agList;

	size_t last_pos = 0;
	size_t pos = para.find("{");
	size_t end_pos = para.rfind('}');
	if (end_pos == string::npos)return false;
	if (pos == string::npos)return false;

	//提取类型
	last_pos = para.find('\"', pos + 1);
	pos = para.find('\"', last_pos + 1);
	if (pos == para.npos)return false;
	//次循环用作解析类型和值
	while (pos < end_pos)
	{
		pair<string, vector<string>> kvp;
		
		kvp.first = para.substr(last_pos + 1, pos - last_pos - 1);

		//找到冒号的位置
		size_t colon_pos = para.find(':', last_pos + 1);
		if (colon_pos == para.npos)return false;

		//寻找是否有中括号
		size_t backet_pos = para.find('[', colon_pos + 1);
		size_t endbacket;

		//找到对应的值
		last_pos = para.find('\"', pos + 1);
		pos = para.find('\"', last_pos + 1);
		if (pos == para.npos)return false;
		//如果中括号存在并且位于冒号后的第一个双引号之前
		if (backet_pos != para.npos && backet_pos <= last_pos)
		{
			endbacket = findVaildLetter(para, ']', pos + 1);
			while (pos < endbacket)
			{
				kvp.second.push_back(para.substr(last_pos + 1, pos - last_pos - 1));

				pos = findVaildLetter(para, ',', pos + 1);
				if (pos == para.npos || pos > endbacket)break;

				last_pos = para.find('\"', pos + 1);
				pos = para.find('\"', last_pos + 1);

			}
		}
		else  //如果不存在中括号，那么就只需提取一个值
			kvp.second.push_back(para.substr(last_pos + 1, pos - last_pos - 1));

		agList.push_back(kvp);
		//找到标志此值结束的逗号
		pos = para.find(',', pos);

		//提取下一个类型
		last_pos = para.find('\"', pos + 1);
		if (last_pos == para.npos)break;				//如果第一次没有找到，则说明后续没有内容
		pos = para.find('\"', last_pos + 1);
		if (pos == para.npos)return false;		//如果第二次没有找到，则说明参数错误
	}

	//传入参数
	for (auto& it : agList)
		transPara(it.first, it.second);

	return true;
}
