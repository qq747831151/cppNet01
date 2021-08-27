/****************************************************
	�ļ���Server.cpp
	���ߣ��ո���
	����: 747831151@qq.com
	���ڣ�2021/08/24 20:06   	
	���ܣ������
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
#include<unistd.h>
#include<string.h>
#include<pthread.h>
#include <arpa/inet.h>
#define SOCKET int
#define INVALID_SOCKET  (SOCKET)(~0)
#define SOCKET_ERROR            (-1)
#endif

/*Ϊ�˿���������ƽ̨Ҳ����ʹ�� �Ҽ���Ŀ���� ѡ�������� ���������� ��ws2_32.lib ��ӽ�ȥ���� �����Ͳ���Ҫ ������Щ */
#pragma  comment(lib,"ws2_32.lib")
#include <stdio.h>
#include <thread>
#include <vector>

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
struct Login:public DataHeader
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
struct LoginResult:public DataHeader
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
struct LoginOut:public DataHeader
{
	LoginOut()
	{
		dataLength = sizeof(LoginOut);
		cmd = CMD_LOGINOUT;
	}
	char userName[32];
};
struct LoginOutResult:public DataHeader
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
std::vector<SOCKET> g_clients;

//����
int Processor(SOCKET clientSockt)
{
	char szRecv[4096] = {};//������
		//5.���տͻ�����������  ���ݴ浽szRecv�е����������ɽ��յ���󳤶� ���һ������Ϊ0
	int nlen = recv(clientSockt, szRecv, sizeof(DataHeader), 0);//����ֵ�ǽ��յĳ��� recv��mac�ķ���ֵ��long ����ǿתint
	DataHeader* header = (DataHeader*)szRecv;
	if (nlen <= 0)
	{
		printf("�ͻ���<Socket=%d>���˳�,�������\n",clientSockt);
		return -1;;
	}
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
	default: {
		header->cmd = CMD_ERROR;
		header->dataLength = 0;
		send(clientSockt, (char*)&header, sizeof(DataHeader), 0);
	}
		   break;
	}
	return 1;
}
int main()
{
#if _WIN32
	/*����socket���绷�� 2.x����*/
	WORD ver = MAKEWORD(2, 2);//�汾��
	WSADATA dat;
	WSAStartup(ver, &dat);
#endif
	

	//1.����һ��socket
	SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	//2.���û����տͻ������ӵ���·�˿�
	sockaddr_in _sin;
	_sin.sin_family = AF_INET;
	_sin.sin_port = htons(4567);
#if _WIN32
	_sin.sin_addr.S_un.S_addr = ADDR_ANY;//���Ƿ���� ���������κε����� ֱ��дany
#else
	_sin.sin_addr.s_addr = ADDR_ANY;
#endif

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
	
//	printf("�¿ͻ��˼��룺socket=%d\n", (int)_clientSock);
	while (true)
	{
		fd_set fd_read;
		fd_set fd_write;
		fd_set fd_error;

		FD_ZERO(&fd_read);
		FD_ZERO(&fd_write);
		FD_ZERO(&fd_error);

		FD_SET(sock, &fd_read);
		FD_SET(sock, &fd_write);
		FD_SET(sock, &fd_error);
		SOCKET maxsock = sock;
		for (int i = g_clients.size() - 1; i >= 0; i--)
		{
			FD_SET(g_clients[i], &fd_read);//���Է� ֻ�� ��������Ҳ���Է�
			if (g_clients[i] > maxsock)
			{
				maxsock = g_clients[i];

			}
		}
		struct timeval time;
		time.tv_sec = 0;//��
		time.tv_usec = 0;//΢��
		int ret = select(maxsock + 1, &fd_read, &fd_write, &fd_error, &time);
		if (ret == -1)
		{
			printf("�ͻ������˳�,�������.\n");
			break;
		}
		// �ж�fd��Ӧ�ı�־λ������0����1, ����ֵ: fd��Ӧ�ı�־λ��ֵ, 0, ����0, 1->����1
		//�������� �ж�������(socket)�Ƿ��ڼ�����
		if (FD_ISSET(sock, &fd_read))
		{
			FD_CLR(sock, &fd_read); //�������ļ�������fd��Ӧ�ı�־λ, ����Ϊ0
			//4.accept �ȴ��ͻ�������
			sockaddr_in addClient = {};
			int len = sizeof(addClient);
			SOCKET _clientSock = INVALID_SOCKET;
			// Ĭ����һ����������, �����ȴ��ͻ�������
			// ���󵽴�, ���տͻ�������
			// �õ�һ������ͨ�ŵ��ļ�������
#if _WIN32
			_clientSock = accept(sock, (sockaddr*)&addClient, &len);
#else
			_clientSock = accept(sock, (sockaddr*)&addClient, (socklen_t*)&len);

#endif

			if (_clientSock == INVALID_SOCKET)
			{
				printf("ERROR,�ȴ����ܿͻ�������ʧ��...\n");
			}
			else
			{
				//����������ͻ��˼��� �����������еĿͻ��˷���  �������������
			/*for (int i = g_clients.size() - 1; i >= 0; i--)
			{
				LoginNewUser loginNewUser;
				loginNewUser.sock = _clientSock;
				send(g_clients[i], (const char*)&loginNewUser, sizeof(LoginNewUser), 0);
			}*/

				g_clients.push_back(_clientSock);
				printf("�¿ͻ��˼��룺socket=%d\n", (int)_clientSock);
			}

		}
		// ͨ��, �пͻ��˷������ݹ���  ��windows�� fd_read.fd_count ��Ϊ�Ǳ��������¼���socket������  fd_read.fd_countֻ��windows����
		//for (int i = 0; i < fd_read.fd_count; i++)
		//{
		//	if (-1==Processor(fd_read.fd_array[i]))
		//	{
		//		auto iter = find(g_clients.begin(), g_clients.end(), fd_read.fd_array[i]);
		//		if (iter!=g_clients.end())
		//		{
		//			//���
		//			g_clients.erase(iter);
		//		}
		//	}
		//}

		for (int i = g_clients.size() - 1; i >= 0; i--)
		{
			if (FD_ISSET(g_clients[i], &fd_read))
			{
				if (-1 == Processor(fd_read.fd_array[i]))
				{
					std::vector<SOCKET>::iterator iter = g_clients.begin() + i;//����Ҫɾ���ĵ�����
					if (iter != g_clients.end())
					{
						g_clients.erase(iter);

					}
					
				}
			}
		}
	
	}
#if _WIN32
	//�رտͻ��˵�socket
	for (int i = g_clients.size() - 1; i >= 0; i--)
	{
		closesocket(g_clients[i]);
	}
	//7.�ر��׽���
	closesocket(sock);
	//���windows socket����
	WSACleanup();
#else
	//�رտͻ��˵�socket
	for (int i = g_clients.size() - 1; i >= 0; i--)
	{
		close(g_clients[i]);
	}
	//7.�ر��׽���
	close(sock);
#endif
	
	printf("�ͻ������˳�,�������\n");
	getchar();
	return 0;
}
