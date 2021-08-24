/****************************************************
	�ļ���Client.cpp
	���ߣ��ո���
	����: 747831151@qq.com
	���ڣ�2021/08/24 20:26   	
	���ܣ��ͻ���
*****************************************************/
#pragma once
#define  WIN32_LEAN_AND_MEAN  //��Ӱ�� windows.h �� WinSock2.h ǰ��˳�� 
#define _WINSOCK_DEPRECATED_NO_WARNINGS //������� inet_ntoa   �������һ���Ŀ���� C/C++ Ԥ�������� Ԥ���������
#include <WinSock2.h>
#include<windows.h>
#include <stdio.h>
/*Ϊ�˿���������ƽ̨Ҳ����ʹ�� �Ҽ���Ŀ���� ѡ�������� ���������� ��ws2_32.lib ��ӽ�ȥ���� �����Ͳ���Ҫ ������Щ */
#pragma  comment(lib,"ws2_32.lib")
enum CMD
{
	CMD_LOGIN,
	CMD_LOGINOUT,
	CMD_ERROR,
};
struct DataHeader
{
	short cmd;//����
	short dataLength;//���ݳ���

};
//��¼
struct Login
{
	char userName[32];
	char passWord[32];
};
//
struct LoginResult
{
	int result;
};
//�ǳ�
struct LoginOut
{
	char userName[32];
};
struct LoginOutResult
{
	int result;
};
int main()
{
	/*����socket���绷�� 2.x����*/
	WORD ver = MAKEWORD(2, 2);//�汾��
	WSADATA dat;
	WSAStartup(ver, &dat);
	SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock == INVALID_SOCKET)
	{
		printf("ERROR,����socketʧ��...\n");
	}
	else
	{
		printf("TURE,����socket�ɹ�.....\n");
	}
	//2.���ӷ�����
	sockaddr_in _sin = {};
	_sin.sin_family = AF_INET;
	_sin.sin_port = htons(4567);
	_sin.sin_addr.S_un.S_addr = inet_addr("192.168.17.1");
	int ret = connect(sock, (sockaddr*)&_sin, sizeof(_sin));
	if (ret == -1)
	{
		printf("ERROR,���ӷ�����connectʧ��...\n");
	}
	else
	{
		printf("TURE,���ӷ�����connect�ɹ�...\n");
	}
	while (true)
	{
		//3.��������
		char buf[] = "";
		scanf("%s", buf);
		//4.������������
		if (0==strcmp(buf,"exit"))
		{
			printf("�յ�exit����,�������");
			break;
		}
		else if(0==strcmp(buf,"login"))
		{
			Login login;
			strcpy(login.passWord, "123456");
			strcpy(login.userName, "Сǿ");
			DataHeader dh = { CMD_LOGIN,sizeof(login) };
			//5.�����˷�������
			send(sock, (const char*)&dh, sizeof(dh), 0);
			send(sock, (const char*)&login, sizeof(login), 0);
			//6.���շ���˷��ص�����
			DataHeader retHeader;
			LoginResult loginRet;
			recv(sock, (char *)&retHeader, sizeof(retHeader), 0);
			recv(sock, (char*)&loginRet, sizeof(loginRet), 0);
			printf("LoginResult %d\n", loginRet.result);
		}
		else if (0 == strcmp(buf, "loginOut"))
		{
			LoginOut loginOut;
			strcpy(loginOut.userName, "Сǿ");
			DataHeader dh = { CMD_LOGINOUT,sizeof(loginOut) };
			//5.�����˷�������
			send(sock, (const char*)&dh, sizeof(dh), 0);
			send(sock, (const char*)&loginOut, sizeof(loginOut), 0);
			//6.���շ���˷��ص�����
			DataHeader retHeader;
			LoginOutResult loginRet;
			recv(sock, (char*)&retHeader, sizeof(retHeader), 0);
			recv(sock, (char*)&loginRet, sizeof(loginRet), 0);
			printf("LoginOutResult %d\n", loginRet.result);
		}
		else
		{
			printf("�յ���֧������,����������.\n");
		}
	}
	//7.�ر��׽���
	closesocket(sock);
	//���windows socket����
	WSACleanup();
	printf("�ͻ������˳�,�������\n");
	getchar();
	return 0;
}