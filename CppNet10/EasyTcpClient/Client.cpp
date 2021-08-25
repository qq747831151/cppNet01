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
	CMD_LOGIN_RESULT,
	CMD_LOGINOUT,
	CMD_LOGINOUT_RESULT,
	CMD_ERROR,
	CMD_NEWUSER,
};
struct DataHeader
{
	short cmd;//����
	short dataLength;//���ݳ���

};
//��¼
struct Login :public DataHeader
{
	Login()
	{
		dataLength = sizeof(Login);
		cmd = CMD_LOGIN;
	}
	char userName[32];
	char passWord[32];
};
//
struct LoginResult :public DataHeader
{
	LoginResult()
	{
		dataLength = sizeof(LoginResult);
		cmd = CMD_LOGIN_RESULT;
		result = 1;
	}
	int result;
};
//�ǳ�
struct LoginOut :public DataHeader
{
	LoginOut()
	{
		dataLength = sizeof(LoginOut);
		cmd = CMD_LOGINOUT;
	}
	char userName[32];
};
struct LoginOutResult :public DataHeader
{
	LoginOutResult()
	{
		dataLength = sizeof(LoginOutResult);
		cmd = CMD_LOGINOUT_RESULT;
		result = 1;
	}
	int result;
};
struct LoginNewUser :public DataHeader
{
	LoginNewUser()
	{
		dataLength = sizeof(LoginNewUser);
		cmd = CMD_NEWUSER;
		sock = 0;
	}
	int sock;
};
//����
int Processor(SOCKET clientSockt)
{
	char szRecv[4096] = {};//������
		//5.���տͻ�����������  ���ݴ浽szRecv�е����������ɽ��յ���󳤶� ���һ������Ϊ0
	int nlen = recv(clientSockt, szRecv, sizeof(DataHeader), 0);//����ֵ�ǽ��յĳ���
	if (nlen <= 0)
	{
		printf("�ͻ������˳�,�������\n");
		return -1;;
	}
	DataHeader* header = (DataHeader*)szRecv;
	switch (header->cmd)
	{
	case CMD_LOGIN: {
		//���ǽ��������� ����ǰ���յ���Ϣͷ������  ��ô����������Ҫ���յĵ�¼��Ϣ��������  ���ǵڶ�������Ҫ������Ϣͷ�ĵ�ַ �Ϳ��Դ��Ǹ�λ�ÿ�ʼ����������  
		//���������� ���ǽ��յ����ݳ���Ҫ��ȥ��Ϣͷ�ĳ���   header->dataLength���ܵĳ��� sizeof(DataHeader)��Ϣͷ�ĳ���
		recv(clientSockt, szRecv + sizeof(DataHeader), header->dataLength - sizeof(DataHeader), 0);
		Login* login = (Login*)szRecv;
		//���� �ж��û��������Ƿ���ȷ
		printf("������=%s ������%s \n", login->userName, login->passWord);
		LoginResult loginRet;
		send(clientSockt, (const char*)&loginRet, sizeof(loginRet), 0);
	}
				  break;
	case  CMD_LOGINOUT: {

		recv(clientSockt, szRecv + sizeof(DataHeader), header->dataLength - sizeof(DataHeader), 0);
		LoginOut* loginOut = (LoginOut*)szRecv;
		//���� �ж��û��������Ƿ���ȷ
		printf("������=%s   \n", loginOut->userName);
		LoginOutResult loginOutRet;
		send(clientSockt, (const char*)&loginOutRet, sizeof(loginOutRet), 0);
	}
					  break;
	case  CMD_NEWUSER:
	{
		
		recv(clientSockt, szRecv + sizeof(DataHeader), header->dataLength - sizeof(DataHeader), 0);
		LoginNewUser* loginNewUser = (LoginNewUser*)&szRecv;
		printf("�����Ŀͻ���Socket=%d\n", loginNewUser->sock);
	}
	break;
	default: {
		header->cmd = CMD_ERROR;
		header->dataLength = 0;
		send(clientSockt, (char*)&header, sizeof(DataHeader), 0);
	}
		   break;
	}
}
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
		fd_set fd_read;
		FD_ZERO(&fd_read);
		FD_SET(sock, &fd_read);
		struct timeval time;
		time.tv_sec = 0;
		time.tv_usec = 0;
		int ret= select(sock, &fd_read, NULL, NULL, &time);
		if (ret<0)
		{
			printf("select, �������\n");
			break;
		}
		//�з���Ϣ����
		if (FD_ISSET(sock,&fd_read))
		{
			FD_CLR(sock, &fd_read);
			if (-1==Processor(sock))
			{
				printf("select �������\n");
				break;
			}
		}
		//printf("����ʱ�䴦������ҵ��\n");
		////3.��������
		char buf[] = "";
		scanf("%s", buf);
		//4.��������
		if (0==strcmp(buf,"exit"))
		{
			printf("�յ�exit����,�������");
			break;
		}
		Login login;
		strcpy(login.userName, "sfl");
		strcpy(login.passWord, "123");
		send(sock, (char*)&login, sizeof(login), 0);
		Sleep(1000);
	}
	//7.�ر��׽���
	closesocket(sock);
	//���windows socket����
	WSACleanup();
	printf("�ͻ������˳�,�������\n");
	getchar();
	return 0;
}