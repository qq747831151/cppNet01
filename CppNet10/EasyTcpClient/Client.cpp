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
const int Count = 1000;//�ͻ�������
const int tCount = 4;//�����̵߳�����
//�ͻ�������
EasyTcpClient* Clients[Count];

void SendThread(int id)
{
	//�߳�ID
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
	//���������߳�
	for (int i = 0; i < tCount; i++)
	{
		std::thread t1(SendThread, i+1);
		t1.detach();
	}
	
	
	//�����߳�
	//std::thread t1(cmdThread,&client1);
	//t1.detach();//�̷߳���
	// 
	std::thread t1(cmdThread);
	t1.detach();//�̷߳���

	while (g_Exit)
	{
		Sleep(100);
	}
	printf("�ͻ������˳�,�������\n");
	getchar();
	return 0;
}