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
//�������������
void cmdThread(EasyTcpClient*client)
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
	
	//�����߳�
	std::thread t1(cmdThread,&client1);
	t1.detach();//�̷߳���

	Login login;
	strcpy(login.userName, "sfl");
	strcpy(login.passWord, "123");

	while (client1.IsRun())
	{
		client1.OnRun();
		client1.SendData(&login);
	}

	client1.Close();
	printf("�ͻ������˳�,�������\n");
	getchar();
	return 0;
}