#pragma once
#ifndef _CS_COMMUNICATION_H
#define _CS_COMMUNICATION_H

#include "stdafx.h"

class csCommunication
{
private:
#define BUF_SIZE 1024
#define SERVER_PORT 798
#define DEFAULT_PORT "798"
#define MSG_BUF_SIZE 1024
public:
	csCommunication();
	~csCommunication();
private:
	WORD winsock_ver;
	WSADATA wsa_data;
	SOCKET sock_svr;
	SOCKET sock_clt;
	SOCKADDR_IN addr_svr;
	SOCKADDR_IN addr_clt;

	int addr_len;
	char buf_ip[BUF_SIZE];
	list<thread> ClientList;

protected:
	int get(SOCKET client,size_t csid,char *buf,size_t len);

public:
	void WaitForClient();
	DWORD WINAPI CreateClientThread(SOCKET clt,size_t csid);
};

#endif
