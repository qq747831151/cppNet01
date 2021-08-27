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
std::vector<SOCKET> g_clients;



int main()
{
	EasyTcpServer server;
	server.InitSocket();
	server.Bind(nullptr,4567);
	server.Listen(5);
	while (server.IsRun())
	{
		server.OnRun();
	}
	server.Close();
	getchar();
	return 0;
}

	
