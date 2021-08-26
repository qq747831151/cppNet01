/****************************************************
	�ļ���Client.cpp
	���ߣ��ո���
	����: 747831151@qq.com
	���ڣ�2021/08/24 20:26   	
	���ܣ��ͻ���
*****************************************************/
#pragma once
#if _WIN32

#define  WIN32_LEAN_AND_MEAN  //��Ӱ�� windows.h �� WinSock2.h ǰ��˳�� 
#define _WINSOCK_DEPRECATED_NO_WARNINGS //������� inet_ntoa   �������һ���Ŀ���� C/C++ Ԥ�������� Ԥ���������
#include <WinSock2.h>
#include<windows.h>
/*Ϊ�˿���������ƽ̨Ҳ����ʹ�� �Ҽ���Ŀ���� ѡ�������� ���������� ��ws2_32.lib ��ӽ�ȥ���� �����Ͳ���Ҫ ������Щ */
#pragma  comment(lib,"ws2_32.lib")

#else

#include<stdlib.h>
#include<unistd.h>
#include<fcntl.h>
#include<signal.h>
#include<sys/stat.h>
#include<string.h>
#include <sys/time.h>
#include<pthread.h>
#include <arpa/inet.h>
#include<sys/wait.h>
#include<sys/mman.h>
#include <sys/select.h>
#include <poll.h>
#include <sys/epoll.h>
#include <errno.h>

#endif
#define SOCKET int
#define INVALID_SOCKET  (SOCKET)(~0)
#define SOCKET_ERROR            (-1)
#include <stdio.h>
#include <thread>
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
	int nlen =(int)recv(clientSockt, szRecv, sizeof(DataHeader), 0);//����ֵ�ǽ��յĳ���  MAC�޸ĵĵط�
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
	return 1;
}
bool g_bExit = true;//�߳��˳�
//�������������
void cmdThread(SOCKET sock)
{
	while (true)
	{
		////3.��������
		char buf[] = "";
		scanf("%s", buf);
		//4.��������
		if (0 == strcmp(buf, "exit"))
		{
			printf("�յ�exit����,�������");
			g_bExit = false;
			break;
		}
		else if(0 == strcmp(buf, "login"))
		{
			Login login;
			strcpy(login.userName, "sfl");
			strcpy(login.passWord, "123");
			send(sock, (char*)&login, sizeof(login), 0);
		}
		else if(0 == strcmp(buf, "loginOut"))
		{
			LoginOut loginOut;
			strcpy(loginOut.userName, "sfl");
			send(sock, (char*)&loginOut, sizeof(loginOut), 0);
		}
		
		Sleep(1000);
	}
	
}
int main()
{
#if _WIN32
	/*����socket���绷�� 2.x����*/
	WORD ver = MAKEWORD(2, 2);//�汾��
	WSADATA dat;
	WSAStartup(ver, &dat);
#endif
	
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
#if _WIN32
	_sin.sin_addr.S_un.S_addr = inet_addr("192.168.17.1");
#else
	inet_pton(AF_INET, "192.168.17.1", &_sin.sin_addr.s_addr);
#endif
	
	int ret = connect(sock, (sockaddr*)&_sin, sizeof(_sin));
	if (ret == -1)
	{
		printf("ERROR,���ӷ�����connectʧ��...\n");
	}
	else
	{
		printf("TURE,���ӷ�����connect�ɹ�...\n");
	}
	//�����߳�
	std::thread t1(cmdThread, sock);
	t1.detach();//�̷߳���
	while (g_bExit)
	{
		fd_set fd_read;
		FD_ZERO(&fd_read);
		FD_SET(sock, &fd_read);
		struct timeval time;
		time.tv_sec = 0;
		time.tv_usec = 0;
		int ret= select(sock+1, &fd_read, NULL, NULL, &time);
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
	
	}
#if _WIN32
	//7.�ر��׽���
	closesocket(sock);//�������windows����Ĺرպ���
	//���windows socket����
	WSACleanup();
#else
	close(sock);
#endif
	
	printf("�ͻ������˳�,�������\n");
	getchar();
	return 0;
}