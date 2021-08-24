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
	char cmBuf[128] = "";
	while (true)
	{
		//5.���տͻ�����������  ���һ������Ϊ0
		int nlen = recv(_clientSock, cmBuf, sizeof(cmBuf), 0);//����ֵ�ǽ��յĳ���
		if (nlen<=0)
		{
			printf("�ͻ������˳�,�������\n");
			break;
		}
		printf("�յ�����%s\n", cmBuf);
		//��������
		if (0==strcmp(cmBuf,"getName"))
		{
			char buf[] = "�ҽ�Сǿ";
			send(_clientSock, buf, strlen(buf)+1, 0);
		}
		else if(0==strcmp(cmBuf,"getAge"))
		{
			char buf[] = "��18��";
			send(_clientSock, buf, strlen(buf) + 1, 0);
		}
		else
		{
			char buf[] = "?????";
			send(_clientSock, buf, strlen(buf) + 1, 0);
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
