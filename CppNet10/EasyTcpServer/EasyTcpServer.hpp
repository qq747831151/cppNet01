#pragma once
#ifndef _EasyTcpServer_Hpp_
#define   _EasyTcpServer_Hpp_
#endif // !_EasyTcpServer_Hpp_

#ifdef _WIN32

#define  WIN32_LEAN_AND_MEAN  //不影响 windows.h 和 WinSock2.h 前后顺序 
#define _WINSOCK_DEPRECATED_NO_WARNINGS //这个用于 inet_ntoa   可以在右击项目属性 C/C++ 预处理里面 预处理定义添加
#include <WinSock2.h>
#include<windows.h>
/*为了可以在其他平台也可以使用 右键项目属性 选择链接器 附加依赖项 将ws2_32.lib 添加进去就行 这样就不需要 下面这些 */
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

/*为了可以在其他平台也可以使用 右键项目属性 选择链接器 附加依赖项 将ws2_32.lib 添加进去就行 这样就不需要 下面这些 */
#pragma  comment(lib,"ws2_32.lib")
#include <stdio.h>
#include <thread>
#include <vector>
#include "MessageHeader.hpp"
#ifndef RECV_BUFF_SIZE
//缓冲区区域最小单元大小
#define  RECV_BUFF_SIZE 10240
#endif // !RECV_BUFF_SIZE 
class ClientSocket
{
public:
	ClientSocket(SOCKET sock)
	{
		_lastPos = 0;
		_sockfd = sock;
		memset(_szMsg, 0, sizeof(_szMsg));
	}
	SOCKET Getsockfd()
	{
		return _sockfd;
	}
	char* GetmsgBuf()
	{
		return _szMsg;
	}
     int GetLastPos()
	{
		return _lastPos;
	}
	void SetLastPos(int pos)
	{
		 _lastPos=pos;
	}
private:
	//	//第二缓冲区 消息缓冲区
	char _szMsg[RECV_BUFF_SIZE * 10];
	//	//消息缓冲的数据尾部位置
	int _lastPos;
	SOCKET _sockfd;
};


class EasyTcpServer
{
private:
	SOCKET _sock;
	std::vector<ClientSocket*> g_clients;
public:
	EasyTcpServer()
	{
		_sock = INVALID_SOCKET;
	}
	virtual ~EasyTcpServer()
	{
		Close();
	}
	//初始化Socket
	int InitSocket()
	{
#ifdef _WIN32
		/*启动socket网络环境 2.x环境*/
		WORD ver = MAKEWORD(2, 2);//版本号
		WSADATA dat;
		WSAStartup(ver, &dat);
#endif

		if (_sock!=INVALID_SOCKET)
		{
			printf("关闭旧链接<Socket=%d>\n", _sock);
			Close();
		}
		//1.建立一个socket
		_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (_sock == INVALID_SOCKET)
		{
			printf("ERROR,建立socket失败...\n");
		}
		else
		{
			printf("TURE,建立socket成功.....\n");
		}
		return _sock;

	}
	//绑定IP和端口
	int Bind(unsigned short port)
	{

		//2.绑定用户接收客户端连接的网路端口
		sockaddr_in _sin;
		_sin.sin_family = AF_INET;
		_sin.sin_port = htons(port);

#ifdef _WIN32
		/*if (ip) {
			_sin.sin_addr.S_un.S_addr = inet_addr(ip);
		}
		else {*/
			_sin.sin_addr.S_un.S_addr = INADDR_ANY;
		//}
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
			printf("ERROR,绑定用于接受客户端连接的网络端口失败...\n");
		}
		else
		{
			printf("TURE,绑定用于接受客户端连接的网络端口成功.....\n");
		}
		return ret;
	}
	//监听端口
	int Listen(int n)
	{
		//3.listen 监听端口  这个数量随便写超过1的数
		int ret = listen(_sock, n);
		if (ret == SOCKET_ERROR)
		{
			printf("ERROR, 监听网络端口失败...\n");
		}
		else
		{
			printf("TURE, 监听网络端口成功.....\n");
		}
		return ret;
	}
	//接收客户端连接
	SOCKET Accept()
	{
		//等待接收客户端连接
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
			printf("ERROR,等待接受客户端连接失败");
		}
		else
		{
			//如果有新客户端加入,就向其他现有的客户端发送消息
		//	LoginNewUser newUser;
			g_clients.push_back(new ClientSocket(clientSocket));
			printf("新客户端加入：socket=%d IP=%s\n", (int)clientSocket, inet_ntoa(addClin.sin_addr));

		}
		return clientSocket;
	}
	//关闭Socket
	void Close()
	{
		if (_sock != INVALID_SOCKET)
		{
#ifdef _WIN32
			//关闭客户端的socket
			for (int i = g_clients.size() - 1; i >= 0; i--)
			{
				closesocket(g_clients[i]->Getsockfd());
				delete g_clients[i];
			}
			//7.关闭套接字
			closesocket(_sock);
			//清除windows socket环境
			WSACleanup();
#else
			//关闭客户端的socket
			for (int i = g_clients.size() - 1; i >= 0; i--)
			{
				close(g_clients[i]->Getsockfd());
				delete g_clients[i];
			}
			//7.关闭套接字
			close(_sock);
#endif
			g_clients.clear();
		}

	}
	//处理网络消息
	int nCount = 0;
	bool OnRun()
	{
		if (IsRun())
		{
			//套接字
			fd_set fd_read;//描述符(socket)集合
			fd_set fd_write;
			fd_set fd_exp;
			//清除集合
			FD_ZERO(&fd_read);
			FD_ZERO(&fd_write);
			FD_ZERO(&fd_exp);

			//加入集合
			FD_SET(_sock, &fd_read);
			FD_SET(_sock, &fd_write);
			FD_SET(_sock, &fd_exp);

			SOCKET maxSock = _sock;
			for (int i = g_clients.size() - 1; i >= 0; i--)
			{
				FD_SET(g_clients[i]->Getsockfd(), &fd_read);
				if (g_clients[i]->Getsockfd() > maxSock)
				{
					maxSock = g_clients[i]->Getsockfd();
				}
			}
			struct timeval tm;
			tm.tv_sec = 1;
			tm.tv_usec = 0;
			//nfds 是一个整数值 是指fd_set集合中所有的描述符socket 的范围,而不是数量,
			//既是所有文件描述符最大值+1在windows无所谓 在linux是这样的
			int ret = select(maxSock+1, &fd_read, &fd_write, &fd_exp, &tm);
			//	printf("select ret=%d  count=%d\n", ret, nCount++);
			if (ret < 0)
			{
				printf("客户端已退出,任务结束\n");
				Close();
				return false;
			}
			// 判断fd对应的标志位到底是0还是1, 返回值: fd对应的标志位的值, 0, 返回0, 1->返回1
			//有新连接
			//判断描述符(socket)是否在集合中
			if (FD_ISSET(_sock, &fd_read))
			{
				FD_CLR(_sock, &fd_read);
				Accept();
			}

			//通信 有客户端发消息过来
			for (int i = (int)g_clients.size() - 1; i >= 0; i--)
			{
				if (FD_ISSET(g_clients[i]->Getsockfd(), &fd_read))
				{

					if (-1 == RecvData(g_clients[i]))
					{
						std::vector<ClientSocket*>::iterator iter = g_clients.begin() + i;//就是要删除的迭代器
						if (iter != g_clients.end())
						{
							delete g_clients[i];
							g_clients.erase(iter);

						}

					}
				}
			}
			return true;

		}
		return false;
	}
	//是否在工作中
	bool IsRun()
	{
		return _sock != INVALID_SOCKET;
	}

	//缓冲区
	char szRecv[RECV_BUFF_SIZE] = {};
	//接收数据 处理粘包 拆分包
	int RecvData(ClientSocket* pClient)
	{
		
		//5.接收客户端请求数据
		//数据存到szRecv中  第三个参数可接收数据的最大长度
		int nlen = recv(pClient->Getsockfd(), szRecv, 1024, 0);//返回值是接收的长度  revcz在mac返回值是long 建议强转int
		if (nlen <= 0)
		{
			printf("客户端<Socket%d>已退出,任务结束\n", pClient);
			return -1;
		}
		//将收取的数据拷贝到消息缓冲区
		memcpy(pClient->GetmsgBuf() + pClient->GetLastPos(), szRecv, nlen);
		//消息缓冲区的数据尾部位置后移
		pClient->SetLastPos(pClient->GetLastPos()+nlen);

		//解决粘包问题
		//判断消息缓冲区的数据长度是否大于消息头的数据
		while (pClient->GetLastPos()>=sizeof(DataHeader))
		{
			//这时就可以知道当前消息的长度
			DataHeader* header = (DataHeader*)pClient->GetmsgBuf();
			//判断消息缓冲的数据长度大于消息长度
			if (pClient->GetLastPos()>=header->dataLength)
			{
				//剩余未处理的消息缓冲区的数据长度
				int nSize = pClient->GetLastPos() - header->dataLength;
				//处理网络消息
				OnNetMsg(header, pClient->Getsockfd());
				//将消息缓冲区剩余未处理数据前移
				memcpy(pClient->GetmsgBuf(), pClient->GetmsgBuf() + header->dataLength, nSize);
				//将消息缓冲区的数据尾部位置前移
				pClient->SetLastPos(nSize);
			}
			else
			{
				//消息缓冲区剩余数据 不够一条完整的消息
				break;
			}
		}
		return 1;


	}
	//响应网络消息
	virtual void OnNetMsg(DataHeader* header, SOCKET clienSocket)
	{
		switch (header->cmd)
		{
		case  CMD_LOGIN:
		{
			Login* login = (Login*)&header;
			printf("收到客户端<Socket=%d>请求：CMD_LOGIN,数据长度：%d,userName=%s PassWord=%s\n", clienSocket, login->dataLength, login->userName, login->passWord);
			//忽略 判断用户密码是否正确
			LoginResult loginResult;
			SendData(&loginResult, clienSocket);
		}
		break;
		case  CMD_LOGINOUT:
		{
			LoginOut* loginOut = (LoginOut*)&header;
			printf("收到命令:%d 数据长度:%d\n", header->cmd, header->dataLength);
			//忽略 判断用户密码是否正确
			LoginOutResult loginOutResult;
			SendData(&loginOutResult, clienSocket);
		}
		break;
		default:
			printf("<socket=%d> 收到为定义消息 数据长度:%d\n", clienSocket, header->dataLength);
			DataHeader dp = { ERROR,0 };
			SendData(&dp, clienSocket);
			break;
		}
	}
	//单发指定的Socket数据
	int SendData(DataHeader* header, SOCKET clienSocket)
	{
		if (IsRun() && header != NULL)
		{
			send(clienSocket, (const char*)header, sizeof(header), 0);

		}
		return SOCKET_ERROR;
	}
	//群发数据
	void SendData2All(DataHeader* header)
	{
		if (IsRun() && header != NULL)
		{
			//如果有新客户端加入 就向其他现有的客户端发送
			for (int n = g_clients.size() - 1; n >= 0; n--)
			{
				SendData(header, g_clients[n]->Getsockfd());
			}

		}
	}

private:

};


