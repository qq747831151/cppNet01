/****************************************************
	�ļ���Server.cpp
	���ߣ��ո���
	����: 747831151@qq.com
	���ڣ�2021/08/24 20:06   	
	���ܣ������
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

	//1.����һ��socket
	SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	//2.���û����տͻ������ӵ���·�˿�
	sockaddr_in _sin;
	_sin.sin_family = AF_INET;
	_sin.sin_port = htons(4567);
	_sin.sin_addr.S_un.S_addr = ADDR_ANY;//���Ƿ���� ���������κε����� ֱ��дany
	int ret=bind(sock, (sockaddr*)&_sin, sizeof(_sin));
	if (ret == SOCKET_ERROR)
	{
		printf("ERROR,�����ڽ��ܿͻ������ӵ�����˿�ʧ��...\n");
	}
	else
	{
		printf("TURE,�����ڽ��ܿͻ������ӵ�����˿ڳɹ�.....\n");
	}
	//3.listen �����˿�  ����������д����1����
	ret = listen(sock, 5);
	if (ret == SOCKET_ERROR)
	{
		printf("ERROR, ��������˿�ʧ��...\n");
	}
	else
	{
		printf("TURE, ��������˿ڳɹ�.....\n");
	}
	//4.accept �ȴ��ͻ�������
	sockaddr_in addClient = {};
	int len = sizeof(addClient);
	SOCKET _clientSock = INVALID_SOCKET;
	 // Ĭ����һ����������, �����ȴ��ͻ�������
     // ���󵽴�, ���տͻ�������
     // �õ�һ������ͨ�ŵ��ļ�������
	_clientSock = accept(sock, (sockaddr*)&addClient, &len);
	if (_clientSock == INVALID_SOCKET)
	{
		printf("ERROR,�ȴ����ܿͻ�������ʧ��...\n");
	}
	printf("�¿ͻ��˼��룺socket=%d\n", (int)_clientSock);
	while (true)
	{
		DataHeader header;
		//5.���տͻ�����������  ���һ������Ϊ0
		int nlen = recv(_clientSock,(char *)&header, sizeof(header), 0);//����ֵ�ǽ��յĳ���
		if (nlen<=0)
		{
			printf("�ͻ������˳�,�������\n");
			break;
		}
		printf("�յ�����%d\n", header.cmd);
		switch (header.cmd)
		{
		case CMD_LOGIN: {
			Login login;
			recv(_clientSock, (char*)&login, sizeof(login), 0);
			//���� �ж��û��������Ƿ���ȷ
			printf("������=%s ������%s \n", login.userName, login.passWord);
			LoginResult loginRet = { 1 };
			send(_clientSock, (char*)&header, sizeof(DataHeader), 0);
			send(_clientSock, (const char*)&loginRet, sizeof(loginRet), 0);
		}
				break;
		case  CMD_LOGINOUT: {
			LoginOut loginOut;
			recv(_clientSock, (char*)&loginOut, sizeof(loginOut), 0);
			//���� �ж��û��������Ƿ���ȷ
			printf("������=%s   \n", loginOut.userName);
			LoginOutResult loginOutRet = { 1 };
			send(_clientSock, (char*)&header, sizeof(DataHeader), 0);
			send(_clientSock, (const char*)&loginOutRet, sizeof(loginOutRet), 0);
		}
			break;
		default: {
			header.cmd = CMD_ERROR;
			header.dataLength = 0;
			send(_clientSock, (char*)&header, sizeof(DataHeader), 0);
		}
			break;
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
