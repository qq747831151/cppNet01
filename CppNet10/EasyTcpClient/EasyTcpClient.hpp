
#ifndef _EasyTcpClient_Hpp_
#pragma once
#define  _EasyTcpClient_Hpp_
#ifdef _WIN32

#define  WIN32_LEAN_AND_MEAN  //��Ӱ�� windows.h �� WinSock2.h ǰ��˳�� 
#define _WINSOCK_DEPRECATED_NO_WARNINGS //������� inet_ntoa   �������һ���Ŀ���� C/C++ Ԥ�������� Ԥ���������
#include <WinSock2.h>
#include<windows.h>


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

#define SOCKET int
#define INVALID_SOCKET  (SOCKET)(~0)
#define SOCKET_ERROR            (-1)
#endif
/*Ϊ�˿���������ƽ̨Ҳ����ʹ�� �Ҽ���Ŀ���� ѡ�������� ���������� ��ws2_32.lib ��ӽ�ȥ���� �����Ͳ���Ҫ ������Щ */
#pragma  comment(lib,"ws2_32.lib")
#include "MessageHeader.hpp"
#include <stdio.h>
#include <thread>
class EasyTcpClient
{
public:
	SOCKET _sock;
public:
	EasyTcpClient()
	{
		_sock = INVALID_SOCKET;
	}
	//��һ����������ʱ����������������������麯����ԭ�򣺻�����Դ�ͷŲ���ȫ����� ��Ȼ������Դ�����ͷ�
	 virtual~EasyTcpClient()
	{
		 Close();
	}
	 //��ʼ��
	 void InitSocket()
	 {
#ifdef _WIN32
		 /*����socket���绷�� 2.x����*/
		 WORD ver = MAKEWORD(2, 2);//�汾��
		 WSADATA dat;
		 WSAStartup(ver, &dat);
#endif
		 // 1 ����һ��socket �׽���
		 //�ж��Ƿ�Ϊ��
		 if (_sock!=INVALID_SOCKET)
		 {
			 printf("<socket=%d>�ر�֮ǰ�ľ�����\n", _sock);
			 Close();
		 }
		  _sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		 if (_sock == INVALID_SOCKET)
		 {
			 printf("ERROR,����socketʧ��...\n");
		 }
		 else
		 {
			 printf("TURE,����socket�ɹ�.....\n");
		 }
	 }

	 int Connect(const char *ip,unsigned  int port )
	 {
		 //2.���ӷ�����
		 sockaddr_in _sin = {};
		 _sin.sin_family = AF_INET;
		 _sin.sin_port = htons(port);
#ifdef _WIN32
		 _sin.sin_addr.S_un.S_addr = inet_addr(ip);
#else
		 inet_pton(AF_INET, ip, &_sin.sin_addr.s_addr);
#endif
		 printf("<socket=%d>�������ӷ�����<%s:%d>...\n", _sock, ip, port);
		 int ret = connect(_sock, (sockaddr*)&_sin, sizeof(sockaddr_in));
		 if (ret == -1)
		 {
			 printf("ERROR,<socket=%d><port=%d>���ӷ�����connectʧ��...\n",_sock,port);
		 }
		 else
		 {
			 printf("TURE,<socket=%d> <port=%d>���ӷ�����connect�ɹ�...\n",_sock,port);
		 }
		 return ret;
	 }
	 void Close()
	 {
		 if (_sock!=INVALID_SOCKET)
		 {
			 //�ر��׽���
#ifdef _WIN32
//7.�ر��׽���
			 closesocket(_sock);//�������windows����Ĺرպ���
			 //���windows socket����
			 WSACleanup();
#else
			 close(_sock);
#endif
			 _sock = INVALID_SOCKET;
		 }
	 }
	 int nCount = 0;
	 /*��ѯ������Ϣ*/
	 bool OnRun()
	 {
		 if (IsRun())
		 {
			 fd_set fd_read;
			 FD_ZERO(&fd_read);
			 FD_SET(_sock, &fd_read);
			 struct timeval time;
			 time.tv_sec = 1;
			 time.tv_usec = 0;
			 int ret = select(_sock + 1, &fd_read, NULL, NULL, &time);
			 if (ret < 0)
			 {
				 printf("select, �������\n");
				 return false;
			 }
			 //�з���Ϣ����
			 if (FD_ISSET(_sock, &fd_read))
			 {
				 FD_CLR(_sock, &fd_read);
				 if (-1 == RecvData(_sock))
				 {
					 printf("select �������\n");
					 return false;
				 }
			 }
			 return true;
		 }
		 return false;
	 }
	 /*�Ƿ��ڹ�����*/
	 bool IsRun()
	 {
		 return _sock != INVALID_SOCKET;
	 }
#ifndef  RECV_BUFF_SIZE
	 //������������С��Ԫ��С
     #define  RECV_BUFF_SIZE 10240
#endif // ! RECV_BUFF_SIZE


	 //���ջ�����
	 char szRecv[RECV_BUFF_SIZE] = {};
	 //�ڶ�������  ��Ϣ������
	 char szMsg[RECV_BUFF_SIZE * 10] = {};
	 //��Ϣ���������β��λ��
	 int LastPos = 0;

	 /*�������� ����ճ�� ���*/
	 int RecvData(SOCKET clientSock)
	 {
		//5.���տͻ�����������  ���ݴ浽szRecv�е����������ɽ��յ���󳤶� ���һ������Ϊ0
		 int nlen = (int)recv(clientSock, szRecv, RECV_BUFF_SIZE, 0);//����ֵ�ǽ��յĳ���  MAC�޸ĵĵط�
		 DataHeader* header = (DataHeader*)szRecv;
		 if (nlen <= 0)
		 {
			 printf("�����˶Ͽ�����,�������\n");
			 return -1;;
		 }
		 //����ȡ�����ݿ�������Ϣ������
		 memcpy(szMsg + LastPos, szRecv, nlen);
		 //��Ϣ������������β��λ�ú���
		 LastPos += nlen;

		 //���ճ������
		 //�ж���Ϣ�����������ݳ����ǵĴ�����ϢͷDataHeader�ĳ���
		 while (LastPos>=sizeof(DataHeader))
		 {
			 //��ʱ�Ϳ���֪����ǰ��Ϣ�ĳ���
			 DataHeader* header = (DataHeader*)szMsg;
			 //�ж���Ϣ��������ݳ��ȴ�����Ϣ����
			 if (LastPos>=header->dataLength)
			 {
				 //ʣ��δ�������Ϣ���������ݵĳ���
				 int nSize = LastPos - header->dataLength;
				 //����������Ϣ
				 OnNetMsg(header);
				 //����Ϣ������ʣ��δ��������ǰ��
				 memcpy(szMsg, szMsg + header->dataLength, nSize);
				 //��Ϣ������������β��λ��ǰ��
				 LastPos = nSize;
			 }
			 else
			 {
				 //��Ϣ������ʣ������ ����һ����������Ϣ
				 break;
			 }
		 }
		 return 1;
		
		 // ����ǰ���յ���Ϣͷ������  ��ô����������Ҫ���յĵ�¼��Ϣ��������  ���ǵڶ�������Ҫ������Ϣͷ�ĵ�ַ �Ϳ��Դ��Ǹ�λ�ÿ�ʼ����������  
		//���������� ���ǽ��յ����ݳ���Ҫ��ȥ��Ϣͷ�ĳ���   header->dataLength���ܵĳ��� sizeof(DataHeader)��Ϣͷ�ĳ���
		 //z���û��� ָ��� ������szRecv�ı�Ļ���ô headerҲ����Ÿĵ�  ���û�� �������
	//	recv(clientSock, szRecv + sizeof(DataHeader), header->dataLength - sizeof(DataHeader), 0);

		

	 }
	 /*��Ӧ������Ϣ*/
	 void OnNetMsg(DataHeader * header)
	 {
		 switch (header->cmd)
		 {
		 case CMD_LOGIN_RESULT: {
			
			 LoginResult* login = (LoginResult*)header;
			 printf("<socket=%d>�յ��������Ϣ��CMD_LOGIN_RESULT,���ݳ��ȣ�%d\n", _sock, login->dataLength);
		 }
					   break;
		 case  CMD_LOGINOUT_RESULT: {

			LoginOutResult* logout = (LoginOutResult*)header;
			 printf("<socket=%d>�յ��������Ϣ��CMD_LOGOUT_RESULT,���ݳ��ȣ�%d\n", _sock, logout->dataLength);
		 }
						   break;
		 case  CMD_NEWUSER:
		 {

			 LoginNewUser* userJoin = (LoginNewUser*)&header;
			 printf("<socket=%d>�յ��������Ϣ��CMD_NEW_USER_JOIN,���ݳ��ȣ�%d\n", _sock, userJoin->dataLength);
		 }
		 break;
		 case  CMD_ERROR:
		 {

			 printf("<socket=%d>�յ��������Ϣ��CMD_ERROR,���ݳ��ȣ�%d\n", _sock, header->dataLength);
		 }
		 break;
		 default:
			 //δ֪��Ϣ
			 printf("<socket=%d>�յ�δ������Ϣ ���ݳ��ȣ�%d \n", _sock, header->dataLength);
			 break;
		 }
	 }
	 //��������
	 int SendData(DataHeader*header)
	 {
		 if (IsRun()&&header)
		 {
			
			 return send(_sock, (const char *)header, header->dataLength, 0);
		 }
		 return SOCKET_ERROR;
	 }

private:

};


#endif // !1


