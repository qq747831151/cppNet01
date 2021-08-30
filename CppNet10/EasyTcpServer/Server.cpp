/****************************************************
	�ļ���Server.cpp
	���ߣ��ո���
	����: 747831151@qq.com
	���ڣ�2021/08/24 20:06   	
	���ܣ������
*****************************************************/
#pragma once
#include "EasyTcpServer.hpp"

#include <vector>
#include <thread>
std::vector<SOCKET> g_clients;
bool g_exit = true;
void cmdThread()
{
	while (true)
	{
		char szBuf[256];
		scanf("%s", szBuf);
		if (strcmp(szBuf,"exit")==0)
		{
			g_exit = false;
			printf("�˳�cmdThread\n");
			break;
		}
		else
		{
			printf("��֧������\n");
		}
	}
}


int main()
{
	EasyTcpServer server;
	server.InitSocket();
	server.Bind(4568);
	server.Listen(5);
	std::thread t1(cmdThread);
	t1.detach();//�̷߳���
	while (g_exit)
	{
		server.OnRun();
	}
	server.Close();
	getchar();
	return 0;
}

	
