/****************************************************
	�ļ���Client.cpp
	���ߣ��ո���
	����: 747831151@qq.com
	���ڣ�2021/08/24 20:26   	
	���ܣ��ͻ���
*****************************************************/
#pragma once
#include "EasyTcpClient.hpp"

#include <stdio.h>
#include <thread>
bool g_Exit = true;//�߳��˳�
//�������������  ���� EasyTcpClient*client
void cmdThread()
{
	while (true)
	{
		////3.��������
		char buf[256];
		scanf("%s", buf);
		//4.��������
		if (0 == strcmp(buf, "exit"))
		{
			printf("�յ�exit����,�������");
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
int main()
{
	const int Count = 1;
	EasyTcpClient* Clients[Count];
	for (int i = 0; i < Count; i++)
	{
		Clients[i] = new EasyTcpClient();
		
	}
	for (int i = 0; i < Count; i++)
	{
		Clients[i]->InitSocket();
		Clients[i]->Connect("192.168.17.1", 4568);
	}
	
	
	//�����߳�
	//std::thread t1(cmdThread,&client1);
	//t1.detach();//�̷߳���
	// 
	std::thread t1(cmdThread);
	t1.detach();//�̷߳���

	Login login;
	strcpy(login.userName, "sfl");
	strcpy(login.passWord, "123");

	while (g_Exit)
	{
		for (int i = 0; i < Count; i++)
		{
			Clients[i]->SendData(&login);
			Clients[i]->OnRun();
		}
	}
	for (int i = 0; i < Count; i++)
	{
		
		Clients[i]->Close();
	}
	//while (client1.IsRun())
	//{
	//	client1.OnRun();
	//	client1.SendData(&login);
	//}

	//client1.Close();
	printf("�ͻ������˳�,�������\n");
	getchar();
	return 0;
}