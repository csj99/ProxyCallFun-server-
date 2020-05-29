#include "paraParser.h"

//���ܷ������һ�ν��������Ƿ�ɹ�
bool paraParser::flag()
{
	return this->sflag;
}

//���ܣ������������
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
	if (val.size() > 1)					//�������ֵΪһ������
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

//Ѱ���ַ����в����ַ����е��ַ�
size_t paraParser::findVaildLetter(const string &str, char Letter, size_t start)
{
	if (start >= str.length())return -1;
	size_t pos = start;
	string::const_iterator sit = str.begin() + start;
	for (; sit != str.end(); pos++, sit++)
	{
		//�����ַ������ž�ֱ������ȥ
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
	//�����б���Ϊ���ͣ�ֵ�洢�ڶ�Ӧ��vector
	list<pair<string, vector<string>>> agList;

	size_t last_pos = 0;
	size_t pos = para.find("{");
	size_t end_pos = para.rfind('}');
	if (end_pos == string::npos)return false;
	if (pos == string::npos)return false;

	//��ȡ����
	last_pos = para.find('\"', pos + 1);
	pos = para.find('\"', last_pos + 1);
	if (pos == para.npos)return false;
	//��ѭ�������������ͺ�ֵ
	while (pos < end_pos)
	{
		pair<string, vector<string>> kvp;
		
		kvp.first = para.substr(last_pos + 1, pos - last_pos - 1);

		//�ҵ�ð�ŵ�λ��
		size_t colon_pos = para.find(':', last_pos + 1);
		if (colon_pos == para.npos)return false;

		//Ѱ���Ƿ���������
		size_t backet_pos = para.find('[', colon_pos + 1);
		size_t endbacket;

		//�ҵ���Ӧ��ֵ
		last_pos = para.find('\"', pos + 1);
		pos = para.find('\"', last_pos + 1);
		if (pos == para.npos)return false;
		//��������Ŵ��ڲ���λ��ð�ź�ĵ�һ��˫����֮ǰ
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
		else  //��������������ţ���ô��ֻ����ȡһ��ֵ
			kvp.second.push_back(para.substr(last_pos + 1, pos - last_pos - 1));

		agList.push_back(kvp);
		//�ҵ���־��ֵ�����Ķ���
		pos = para.find(',', pos);

		//��ȡ��һ������
		last_pos = para.find('\"', pos + 1);
		if (last_pos == para.npos)break;				//�����һ��û���ҵ�����˵������û������
		pos = para.find('\"', last_pos + 1);
		if (pos == para.npos)return false;		//����ڶ���û���ҵ�����˵����������
	}

	//�������
	for (auto& it : agList)
		transPara(it.first, it.second);

	return true;
}
