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

	
