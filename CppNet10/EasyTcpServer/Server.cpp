/****************************************************
	文件：Server.cpp
	作者：苏福龙
	邮箱: 747831151@qq.com
	日期：2021/08/24 20:06   	
	功能：服务端
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
			printf("退出cmdThread\n");
			break;
		}
		else
		{
			printf("不支持命令\n");
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
	t1.detach();//线程分离
	while (g_exit)
	{
		server.OnRun();
	}
	server.Close();
	getchar();
	return 0;
}

	
