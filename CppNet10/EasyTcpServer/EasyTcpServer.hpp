#pragma once
#ifndef _EasyTcpServer_Hpp_
#define   _EasyTcpServer_Hpp_
#endif // !_EasyTcpServer_Hpp_

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
#include "MessageHeader.hpp"
class EasyTcpServer
{
private:
	SOCKET _sock;
	std::vector<SOCKET> g_clients;
public:
	EasyTcpServer()
	{
		_sock = INVALID_SOCKET;
	}
	virtual ~EasyTcpServer()
	{
		Close();
	}
	//��ʼ��Socket
	int InitSocket()
	{
#if _WIN32
		/*����socket���绷�� 2.x����*/
		WORD ver = MAKEWORD(2, 2);//�汾��
		WSADATA dat;
		WSAStartup(ver, &dat);
#endif

		if (_sock!=INVALID_SOCKET)
		{
			printf("�رվ�����<Socket=%d>\n", _sock);
			Close();
		}
		//1.����һ��socket
		_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (_sock == INVALID_SOCKET)
		{
			printf("ERROR,����socketʧ��...\n");
		}
		else
		{
			printf("TURE,����socket�ɹ�.....\n");
		}
		return _sock;

	}
	//��IP�Ͷ˿�
	int Bind(const char* ip, unsigned short port)
	{

		//2.���û����տͻ������ӵ���·�˿�
		sockaddr_in _sin;
		_sin.sin_family = AF_INET;
		_sin.sin_port = htons(port);

#ifdef _WIN32
		if (ip) {
			_sin.sin_addr.S_un.S_addr = inet_addr(ip);
		}
		else {
			_sin.sin_addr.S_un.S_addr = INADDR_ANY;
		}
#else
		if (ip) {
			_sin.sin_addr.s_addr = inet_addr(ip);
		}
		else {
			_sin.sin_addr.s_addr = INADDR_ANY;
		}
#endif

		int ret = bind(_sock, (sockaddr*)&_sin, sizeof(_sin));
		if (ret == SOCKET_ERROR)
		{
			printf("ERROR,�����ڽ��ܿͻ������ӵ�����˿�ʧ��...\n");
		}
		else
		{
			printf("TURE,�����ڽ��ܿͻ������ӵ�����˿ڳɹ�.....\n");
		}
		return ret;
	}
	//�����˿�
	int Listen(int n)
	{
		//3.listen �����˿�  ����������д����1����
		int ret = listen(_sock, n);
		if (ret == SOCKET_ERROR)
		{
			printf("ERROR, ��������˿�ʧ��...\n");
		}
		else
		{
			printf("TURE, ��������˿ڳɹ�.....\n");
		}
		return ret;
	}
	//���տͻ�������
	SOCKET Accept()
	{
		//�ȴ����տͻ�������
		struct sockaddr_in addClin = {};
		int len = sizeof(sockaddr_in);
		SOCKET clientSocket = INVALID_SOCKET;
#ifdef _WIN32
		clientSocket = accept(_sock, (sockaddr*)&addClin, &len);
#else
		clientSocket = accept(_sock, (sockaddr*)&addClin, (socklen_t*)&len);
#endif
		if (clientSocket == INVALID_SOCKET)
		{
			printf("ERROR,�ȴ����ܿͻ�������ʧ��");
		}
		else
		{
			//������¿ͻ��˼���,�����������еĿͻ��˷�����Ϣ
		//	LoginNewUser newUser;
			g_clients.push_back(clientSocket);

		}
		return clientSocket;
	}
	//�ر�Socket
	void Close()
	{
		if (_sock != INVALID_SOCKET)
		{
#if _WIN32
			//�رտͻ��˵�socket
			for (int i = g_clients.size() - 1; i >= 0; i--)
			{
				closesocket(g_clients[i]);
			}
			//7.�ر��׽���
			closesocket(_sock);
			//���windows socket����
			WSACleanup();
#else
			//�رտͻ��˵�socket
			for (int i = g_clients.size() - 1; i >= 0; i--)
			{
				close(g_clients[i]);
			}
			//7.�ر��׽���
			close(_sock);
#endif
		}

	}
	//����������Ϣ
	int nCount = 0;
	bool OnRun()
	{
		if (IsRun())
		{
			//�׽���
			fd_set fd_read;//������(socket)����
			fd_set fd_write;
			fd_set fd_exp;
			//�������
			FD_ZERO(&fd_read);
			FD_ZERO(&fd_write);
			FD_ZERO(&fd_exp);

			//���뼯��
			FD_SET(_sock, &fd_read);
			FD_SET(_sock, &fd_write);
			FD_SET(_sock, &fd_exp);

			SOCKET maxSock = _sock;
			for (int i = g_clients.size() - 1; i >= 0; i--)
			{
				FD_SET(g_clients[i], &fd_read);
				if (g_clients[i] > maxSock)
				{
					maxSock = g_clients[i];
				}
			}
			struct timeval tm;
			tm.tv_sec = 1;
			tm.tv_usec = 0;
			//nfds ��һ������ֵ ��ָfd_set���������е�������socket �ķ�Χ,����������,
			//���������ļ����������ֵ+1��windows����ν ��linux��������
			int ret = select(maxSock, &fd_read, &fd_write, &fd_exp, &tm);
			//	printf("select ret=%d  count=%d\n", ret, nCount++);
			if (ret < 0)
			{
				printf("�ͻ������˳�,�������\n");
				Close();
				return false;
			}
			// �ж�fd��Ӧ�ı�־λ������0����1, ����ֵ: fd��Ӧ�ı�־λ��ֵ, 0, ����0, 1->����1
			//��������
			//�ж�������(socket)�Ƿ��ڼ�����
			if (FD_ISSET(_sock, &fd_read))
			{
				FD_CLR(_sock, &fd_read);
				Accept();
			}

			//ͨ�� �пͻ��˷���Ϣ����
			for (int i = g_clients.size() - 1; i >= 0; i--)
			{
				if (FD_ISSET(g_clients[i], &fd_read))
				{
					if (-1 == RecvData(g_clients[i]))
					{
						std::vector<SOCKET>::iterator iter = g_clients.begin() + i;//����Ҫɾ���ĵ�����
						if (iter != g_clients.end())
						{
							g_clients.erase(iter);

						}

					}
				}
			}
			return true;

		}
		return false;
	}
	//�Ƿ��ڹ�����
	bool IsRun()
	{
		return _sock != INVALID_SOCKET;
	}

	//������
	char szRecv[1024] = {};
	//�������� ����ճ�� ��ְ�
	int RecvData(SOCKET clienSock)
	{
		
		//5.���տͻ�����������
		//���ݴ浽szRecv��  �����������ɽ������ݵ���󳤶�
		int nlen = recv(clienSock, szRecv, 1024, 0);//����ֵ�ǽ��յĳ���  revcz��mac����ֵ��long ����ǿתint
		if (nlen <= 0)
		{
			printf("�ͻ���<Socket%d>���˳�,�������\n", clienSock);
			return -1;
		}
		LoginResult loginResult;
		SendData(&loginResult, clienSock);
		/*DataHeader* header = (DataHeader*)szRecv;
		recv(clienSock, szRecv + sizeof(DataHeader), header->dataLength - sizeof(DataHeader), 0);
		OnNetMsg(header, clienSock);*/
		return 1;


	}
	//��Ӧ������Ϣ
	virtual void OnNetMsg(DataHeader* header, SOCKET clienSocket)
	{
		switch (header->cmd)
		{
		case  CMD_LOGIN:
		{
			Login* login = (Login*)&header;
			printf("�յ��ͻ���<Socket=%d>����CMD_LOGIN,���ݳ��ȣ�%d,userName=%s PassWord=%s\n", clienSocket, login->dataLength, login->userName, login->passWord);
			//���� �ж��û������Ƿ���ȷ
			LoginResult loginResult;
			SendData(&loginResult, clienSocket);
		}
		break;
		case  CMD_LOGINOUT:
		{
			LoginOut* loginOut = (LoginOut*)&header;
			printf("�յ�����:%d ���ݳ���:%d\n", header->cmd, header->dataLength);
			//���� �ж��û������Ƿ���ȷ
			LoginOutResult loginOutResult;
			SendData(&loginOutResult, clienSocket);
		}
		break;
		default:
			DataHeader dp = { ERROR,0 };
			SendData(&dp, clienSocket);
			break;
		}
	}
	//����ָ����Socket����
	int SendData(DataHeader* header, SOCKET clienSocket)
	{
		if (IsRun() && header != NULL)
		{
			send(clienSocket, (const char*)header, sizeof(header), 0);

		}
		return SOCKET_ERROR;
	}
	//Ⱥ������
	void SendData2All(DataHeader* header)
	{
		if (IsRun() && header != NULL)
		{
			//������¿ͻ��˼��� �����������еĿͻ��˷���
			for (int n = g_clients.size() - 1; n >= 0; n--)
			{
				SendData(header, g_clients[n]);
			}

		}
	}

private:

};


