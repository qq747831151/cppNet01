/****************************************************
	文件：Client.cpp
	作者：苏福龙
	邮箱: 747831151@qq.com
	日期：2021/08/24 20:26   	
	功能：客户端
*****************************************************/
#pragma once
#include "EasyTcpClient.hpp"

#include <stdio.h>
#include <thread>
bool g_Exit = true;//线程退出
//这个负责发送数据  参数 EasyTcpClient*client
void cmdThread()
{
	while (true)
	{
		////3.输入请求
		char buf[256];
		scanf("%s", buf);
		//4.处理命令
		if (0 == strcmp(buf, "exit"))
		{
			printf("收到exit命令,任务结束");
			//client->Close();
			g_Exit = false;
			break;
		}
		/*else if(0 == strcmp(buf, "login"))
		{
			Login login;
			strcpy(login.userName, "sfl");
			strcpy(login.passWord, "123");
			client->SendData(&login);
		}
		else if(0 == strcmp(buf, "loginOut"))
		{
			LoginOut loginOut;
			strcpy(loginOut.userName, "sfl");
			client->SendData(&loginOut);
		}*/
		
		//Sleep(1000);
	}
	
}
const int Count = 1000;//客户端数量
const int tCount = 4;//发送线程的数量
//客户端数组
EasyTcpClient* Clients[Count];

void SendThread(int id)
{
	//线程ID
	int c = (Count / tCount);
	int begin = (id - 1) * c;
	int end = id * c;
	for (int i = begin; i < end; i++)
	{
		Clients[i] = new EasyTcpClient();
	}
	for (int i = begin; i < end; i++)
	{
		Clients[i]->InitSocket();
		Clients[i]->Connect("192.168.17.1", 4568);

	}

	Login login;
	strcpy(login.userName, "sfl");
	strcpy(login.passWord, "123");

	while (g_Exit)
	{
		for (int i = begin; i < end; i++)
		{
			Clients[i]->SendData(&login);
			//Clients[i]->OnRun();
		}
	}
	for (int i = begin; i < end; i++)
	{

		Clients[i]->Close();
	}
}
int main()
{
	//启动发送线程
	for (int i = 0; i < tCount; i++)
	{
		std::thread t1(SendThread, i+1);
		t1.detach();
	}
	
	
	//启动线程
	//std::thread t1(cmdThread,&client1);
	//t1.detach();//线程分离
	// 
	std::thread t1(cmdThread);
	t1.detach();//线程分离

	while (g_Exit)
	{
		Sleep(100);
	}
	printf("客户端已退出,任务结束\n");
	getchar();
	return 0;
}