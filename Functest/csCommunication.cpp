#include "csCommunication.h"
#pragma comment(lib,"ws2_32.lib")    //加载 ws2_32.dll库

extern libMag serverLib;
extern std::mutex libMut;

csCommunication::csCommunication()
{
	int ret_val;

	printf("***************************************************\n");
	printf("* 初始化服务器中.....\n");

	this->winsock_ver = MAKEWORD(2, 2);
	this->addr_len = sizeof(SOCKADDR_IN);
	this->addr_svr.sin_family = AF_INET;
	this->addr_svr.sin_port = ::htons(SERVER_PORT);
	this->addr_svr.sin_addr.S_un.S_addr = ADDR_ANY;
	memset(this->buf_ip, 0, BUF_SIZE);

	ret_val = ::WSAStartup(this->winsock_ver, &this->wsa_data);
	if (ret_val != 0)
	{
		printf("WSA无法启动\n");return;
	}

	this->sock_svr = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (this->sock_svr == INVALID_SOCKET)
	{
		printf("创建服务器套接字失败......\n");return;
	}

	ret_val = ::bind(this->sock_svr, (SOCKADDR*)&this->addr_svr, this->addr_len);
	if (ret_val  != 0)
	{
		printf("绑定套接字失败......\n"); return;
	}

	ret_val = ::listen(this->sock_svr, SOMAXCONN);
	if (ret_val == SOCKET_ERROR)
	{
		printf("服务器监听失败....\n");return;
	}

	printf("* 服务器启动成功。。。\n");
	printf("****************************************\n");
}

csCommunication::~csCommunication()
{
	::closesocket(this->sock_clt);
	::closesocket(this->sock_svr);
	WSACleanup();
	printf("关闭套接字成功。。。。。\n");
}

int csCommunication::get(SOCKET client,size_t csid,char* buf, size_t len)
{
	int ret_val;
	memset(buf, 0, MSG_BUF_SIZE);
	ret_val = recv(client, buf, MSG_BUF_SIZE, 0);
	if (ret_val > 0)
		printf("ID:%-8d recv: \"%s\"\n", csid, buf);
	else
		throw - 1;		//这里最好自定义一个异常类，后续完善
	return ret_val;
}

void csCommunication::WaitForClient()
{
	size_t csid = 1;
	while (true)
	{
		this->sock_clt = ::accept(this->sock_svr, (SOCKADDR *)&this->addr_clt, &this->addr_len);
		if (this->sock_clt == INVALID_SOCKET)
		{
			::printf("无法连接客户端。。。\n");
			WSACleanup();
			system("pause");
		}
		InetNtop(this->addr_clt.sin_family, &this->addr_clt, this->buf_ip, BUF_SIZE);
		printf("ID:%d,IP:%s,Port:%d, connect successed!\n",csid,this->buf_ip,ntohs(this->addr_clt.sin_port));
		
		thread temp(&csCommunication::CreateClientThread,this, this->sock_clt, csid++);
		this->ClientList.push_back(std::move(temp));
	}
}

DWORD WINAPI csCommunication::CreateClientThread(SOCKET client, size_t csid)
{
	string callname, callpara;
	char buf_msg[MSG_BUF_SIZE];
	int ret_val;

	try {
		get(client, csid, buf_msg, MSG_BUF_SIZE);
		if (!strcmp(buf_msg, "hallo serve!"))
			send(client, "serve ok!", sizeof("serve ok!"), 0);
		else return 1;

		do
		{
			ret_val = get(client, csid, buf_msg, MSG_BUF_SIZE);
			callname.assign(buf_msg);
			ret_val = get(client, csid, buf_msg, MSG_BUF_SIZE);
			callpara.assign(buf_msg);
			//对读取或者写入lib的地方加锁
			std::lock_guard<std::mutex> LockLib(libMut);
			serverLib.callFunc(callname,callpara);
		} while (ret_val > 0);
	}
	catch(int e){
		if (e <= 0)
		{
			printf("Can't accept message from ID:%d ,error code:%d ,connection will be closed soon\n", csid, GetLastError());
			closesocket(client);
			shutdown(client, SD_SEND);
			return 1;
		}
		else
			printf("catch exception error: %d,continue\n",e);
	}

	return 0;
}
