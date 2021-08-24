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
		else
		{
			//5.�����˷�������
			send(sock, buf, strlen(buf)+1, 0);
		}
		//6.���շ������Ϣ
		char msg[256] = "";
		int nlen = recv(sock, msg, sizeof(msg), 0);
		printf("���յ����ݳ���Ϊ%d\n", nlen);
		if (nlen>0)
		{
			printf("����Ϊ:%s\n", msg);
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