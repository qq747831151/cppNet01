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
//这个负责发送数据
void cmdThread(EasyTcpClient*client)
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
			client->Close();
			break;
		}
		else if(0 == strcmp(buf, "login"))
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
		}
		
		//Sleep(1000);
	}
	
}
int main()
{

	
	EasyTcpClient client1;
	client1.InitSocket();
	client1.Connect("192.168.17.1",4567);
	
	//启动线程
	std::thread t1(cmdThread,&client1);
	t1.detach();//线程分离

	Login login;
	strcpy(login.userName, "sfl");
	strcpy(login.passWord, "123");

	while (client1.IsRun())
	{
		client1.OnRun();
		client1.SendData(&login);
	}

	client1.Close();
	printf("客户端已退出,任务结束\n");
	getchar();
	return 0;
}