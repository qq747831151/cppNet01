
#ifndef _EasyTcpClient_Hpp_
#pragma once
#define  _EasyTcpClient_Hpp_
#if _WIN32

#define  WIN32_LEAN_AND_MEAN  //不影响 windows.h 和 WinSock2.h 前后顺序 
#define _WINSOCK_DEPRECATED_NO_WARNINGS //这个用于 inet_ntoa   可以在右击项目属性 C/C++ 预处理里面 预处理定义添加
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
/*为了可以在其他平台也可以使用 右键项目属性 选择链接器 附加依赖项 将ws2_32.lib 添加进去就行 这样就不需要 下面这些 */
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
	//当一个类有子类时，该类的析构函数必须是虚函数，原因：会有资源释放不完全的情况 不然子类资源不会释放
	 virtual~EasyTcpClient()
	{

	}
	 //初始化
	 void InitSocket()
	 {
#if _WIN32
		 /*启动socket网络环境 2.x环境*/
		 WORD ver = MAKEWORD(2, 2);//版本号
		 WSADATA dat;
		 WSAStartup(ver, &dat);
#endif
		 // 1 建立一个socket 套接字
		 //判断是否为空
		 if (_sock!=INVALID_SOCKET)
		 {
			 printf("<socket=%d>关闭之前的旧链接\n", _sock);
		 }
		  _sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		 if (_sock == INVALID_SOCKET)
		 {
			 printf("ERROR,建立socket失败...\n");
		 }
		 else
		 {
			 printf("TURE,建立socket成功.....\n");
		 }
	 }

	 int Connect(const char *ip,unsigned short port )
	 {
		 //2.连接服务器
		 sockaddr_in _sin = {};
		 _sin.sin_family = AF_INET;
		 _sin.sin_port = htons(port);
#if _WIN32
		 _sin.sin_addr.S_un.S_addr = inet_addr(ip);
#else
		 inet_pton(AF_INET, ip, &_sin.sin_addr.s_addr);
#endif

		 int ret = connect(_sock, (sockaddr*)&_sin, sizeof(_sin));
		 if (ret == -1)
		 {
			 printf("ERROR,<socket=%d><port=%d>连接服务器connect失败...\n",_sock,port);
		 }
		 else
		 {
			 printf("TURE,<socket=%d> <port=%d>连接服务器connect成功...\n",_sock,port);
		 }
		 return ret;
	 }
	 void Close()
	 {
		 if (_sock!=INVALID_SOCKET)
		 {
			 //关闭套接字
#if _WIN32
//7.关闭套接字
			 closesocket(_sock);//这个是在windows下面的关闭函数
			 //清除windows socket环境
			 WSACleanup();
#else
			 close(sock);
#endif
			 _sock = INVALID_SOCKET;
		 }
	 }
	 /*查询网络消息*/
	 bool OnRun()
	 {
		 if (IsRun())
		 {
			 fd_set fd_read;
			 FD_ZERO(&fd_read);
			 FD_SET(_sock, &fd_read);
			 struct timeval time;
			 time.tv_sec = 0;
			 time.tv_usec = 0;
			 int ret = select(_sock + 1, &fd_read, NULL, NULL, &time);
			 if (ret < 0)
			 {
				 printf("select, 任务结束\n");
				 return false;
			 }
			 //有发消息过来
			 if (FD_ISSET(_sock, &fd_read))
			 {
				 FD_CLR(_sock, &fd_read);
				 if (-1 == RecvData(_sock))
				 {
					 printf("select 任务结束\n");
					 return false;
				 }
			 }
			 return true;
		 }
		 return false;
	 }
	 /*是否在工作中*/
	 bool IsRun()
	 {
		 return _sock != INVALID_SOCKET;
	 }
	 /*接收数据 处理粘包 拆包*/
	 int RecvData(SOCKET clientSock)
	 {
		 char szRecv[4096] = {};//缓冲区
		//5.接收客户端请求数据  数据存到szRecv中第三个参数可接收得最大长度 最后一个设置为0
		 int nlen = (int)recv(clientSock, szRecv, sizeof(DataHeader), 0);//返回值是接收的长度  MAC修改的地方
		 DataHeader* header = (DataHeader*)szRecv;
		 if (nlen <= 0)
		 {
			 printf("与服务端断开连接,任务结束\n");
			 return -1;;
		 }
		
		
		 // 我们前面收到消息头的数据  那么接下来我们要接收的登录消息的数据了  我们第二个参数要加上消息头的地址 就可以从那个位置开始接收数据了  
		//第三个数据 我们接收的数据长度要减去消息头的长度   header->dataLength是总的长度 sizeof(DataHeader)消息头的长度
		 //z这个没理解 指针哈 这边如果szRecv改变的话那么 header也会跟着改的
		recv(clientSock, szRecv + sizeof(DataHeader), header->dataLength - sizeof(DataHeader), 0);

		 OnNetMsg(header);

		 return 0;

	 }
	 /*响应网络消息*/
	 void OnNetMsg(DataHeader * header)
	 {
		 switch (header->cmd)
		 {
		 case CMD_LOGIN_RESULT: {
			
			 LoginResult* login = (LoginResult*)header;
			 printf("<socket=%d>收到服务端消息：CMD_LOGIN_RESULT,数据长度：%d\n", _sock, login->dataLength);
		 }
					   break;
		 case  CMD_LOGINOUT_RESULT: {

			LoginOutResult* logout = (LoginOutResult*)header;
			 printf("<socket=%d>收到服务端消息：CMD_LOGOUT_RESULT,数据长度：%d\n", _sock, logout->dataLength);
		 }
						   break;
		 case  CMD_NEWUSER:
		 {

			 LoginNewUser* userJoin = (LoginNewUser*)&header;
			 printf("<socket=%d>收到服务端消息：CMD_NEW_USER_JOIN,数据长度：%d\n", _sock, userJoin->dataLength);
		 }
		 break;
		 }
	 }
	 //发送数据
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
