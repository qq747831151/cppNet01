
#ifndef _EasyTcpClient_Hpp_
#pragma once
#define  _EasyTcpClient_Hpp_
#ifdef _WIN32

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
		 Close();
	}
	 //初始化
	 void InitSocket()
	 {
#ifdef _WIN32
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
			 Close();
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

	 int Connect(const char *ip,unsigned  int port )
	 {
		 //2.连接服务器
		 sockaddr_in _sin = {};
		 _sin.sin_family = AF_INET;
		 _sin.sin_port = htons(port);
#ifdef _WIN32
		 _sin.sin_addr.S_un.S_addr = inet_addr(ip);
#else
		 inet_pton(AF_INET, ip, &_sin.sin_addr.s_addr);
#endif
		 printf("<socket=%d>正在连接服务器<%s:%d>...\n", _sock, ip, port);
		 int ret = connect(_sock, (sockaddr*)&_sin, sizeof(sockaddr_in));
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
#ifdef _WIN32
//7.关闭套接字
			 closesocket(_sock);//这个是在windows下面的关闭函数
			 //清除windows socket环境
			 WSACleanup();
#else
			 close(_sock);
#endif
			 _sock = INVALID_SOCKET;
		 }
	 }
	 int nCount = 0;
	 /*查询网络消息*/
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
#ifndef  RECV_BUFF_SIZE
	 //缓冲区区域最小单元大小
     #define  RECV_BUFF_SIZE 10240
#endif // ! RECV_BUFF_SIZE


	 //接收缓冲区
	 char szRecv[RECV_BUFF_SIZE] = {};
	 //第二缓冲区  消息缓冲区
	 char szMsg[RECV_BUFF_SIZE * 10] = {};
	 //消息缓冲的数据尾部位置
	 int LastPos = 0;

	 /*接收数据 处理粘包 拆包*/
	 int RecvData(SOCKET clientSock)
	 {
		//5.接收客户端请求数据  数据存到szRecv中第三个参数可接收得最大长度 最后一个设置为0
		 int nlen = (int)recv(clientSock, szRecv, RECV_BUFF_SIZE, 0);//返回值是接收的长度  MAC修改的地方
		 DataHeader* header = (DataHeader*)szRecv;
		 if (nlen <= 0)
		 {
			 printf("与服务端断开连接,任务结束\n");
			 return -1;;
		 }
		 //将收取的数据拷贝到消息缓冲区
		 memcpy(szMsg + LastPos, szRecv, nlen);
		 //消息缓冲区的数据尾部位置后移
		 LastPos += nlen;

		 //解决粘包问题
		 //判断消息缓冲区的数据长度是的大于消息头DataHeader的长度
		 while (LastPos>=sizeof(DataHeader))
		 {
			 //这时就可以知道当前消息的长度
			 DataHeader* header = (DataHeader*)szMsg;
			 //判断消息缓冲的数据长度大于消息长度
			 if (LastPos>=header->dataLength)
			 {
				 //剩余未处理的消息缓冲区数据的长度
				 int nSize = LastPos - header->dataLength;
				 //处理网络消息
				 OnNetMsg(header);
				 //将消息缓冲区剩余未处理数据前移
				 memcpy(szMsg, szMsg + header->dataLength, nSize);
				 //消息缓冲区的数据尾部位置前移
				 LastPos = nSize;
			 }
			 else
			 {
				 //消息缓冲区剩余数据 不够一条完整的消息
				 break;
			 }
		 }
		 return 1;
		
		 // 我们前面收到消息头的数据  那么接下来我们要接收的登录消息的数据了  我们第二个参数要加上消息头的地址 就可以从那个位置开始接收数据了  
		//第三个数据 我们接收的数据长度要减去消息头的长度   header->dataLength是总的长度 sizeof(DataHeader)消息头的长度
		 //z这个没理解 指针哈 这边如果szRecv改变的话那么 header也会跟着改的  这个没用 放着理解
	//	recv(clientSock, szRecv + sizeof(DataHeader), header->dataLength - sizeof(DataHeader), 0);

		

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
		 case  CMD_ERROR:
		 {

			 printf("<socket=%d>收到服务端消息：CMD_ERROR,数据长度：%d\n", _sock, header->dataLength);
		 }
		 break;
		 default:
			 //未知消息
			 printf("<socket=%d>收到未定义消息 数据长度：%d \n", _sock, header->dataLength);
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


