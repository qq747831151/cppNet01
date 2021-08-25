/****************************************************
	文件：Server.cpp
	作者：苏福龙
	邮箱: 747831151@qq.com
	日期：2021/08/24 20:06   	
	功能：服务端
*****************************************************/
#pragma once
#define  WIN32_LEAN_AND_MEAN  //不影响 windows.h 和 WinSock2.h 前后顺序 
#define _WINSOCK_DEPRECATED_NO_WARNINGS //这个用于 inet_ntoa   可以在右击项目属性 C/C++ 预处理里面 预处理定义添加
#include <WinSock2.h>
#include<windows.h>
#include <stdio.h>
#include <vector>
/*为了可以在其他平台也可以使用 右键项目属性 选择链接器 附加依赖项 将ws2_32.lib 添加进去就行 这样就不需要 下面这些 */
#pragma  comment(lib,"ws2_32.lib")
enum CMD
{
	CMD_LOGIN,
	CMD_LOGIN_RESULT,
	CMD_LOGINOUT,
	CMD_LOGINOUT_RESULT,
	CMD_ERROR,
};
struct DataHeader
{
	short cmd;//命令
	short dataLength;//数据长度

};
//登录
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
//登出
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
std::vector<SOCKET> g_clients;

//处理
int Processor(SOCKET clientSockt)
{
	char szRecv[4096] = {};//缓冲区
		//5.接收客户端请求数据  数据存到szRecv中第三个参数可接收得最大长度 最后一个设置为0
	int nlen = recv(clientSockt, szRecv, sizeof(DataHeader), 0);//返回值是接收的长度
	if (nlen <= 0)
	{
		printf("客户端已退出,任务结束\n");
		return -1;;
	}
	DataHeader* header = (DataHeader*)szRecv;
	switch (header->cmd)
	{
	case CMD_LOGIN: {
		//我们进来这里面 我们前面收到消息头的数据  那么接下来我们要接收的登录消息的数据了  我们第二个参数要加上消息头的地址 就可以从那个位置开始接收数据了  
		//第三个数据 我们接收的数据长度要减去消息头的长度   header->dataLength是总的长度 sizeof(DataHeader)消息头的长度
		recv(clientSockt, szRecv + sizeof(DataHeader), header->dataLength - sizeof(DataHeader), 0);
		Login* login = (Login*)szRecv;
		//忽略 判断用户名密码是否正确
		printf("名字是=%s 密码是%s \n", login->userName, login->passWord);
		LoginResult loginRet;
		send(clientSockt, (const char*)&loginRet, sizeof(loginRet), 0);
	}
				  break;
	case  CMD_LOGINOUT: {

		recv(clientSockt, szRecv + sizeof(DataHeader), header->dataLength - sizeof(DataHeader), 0);
		LoginOut* loginOut = (LoginOut*)szRecv;
		//忽略 判断用户名密码是否正确
		printf("名字是=%s   \n", loginOut->userName);
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
}
int main()
{
	/*启动socket网络环境 2.x环境*/
	WORD ver = MAKEWORD(2, 2);//版本号
	WSADATA dat;
	WSAStartup(ver, &dat);

	//1.建立一个socket
	SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	//2.绑定用户接收客户端连接的网路端口
	sockaddr_in _sin;
	_sin.sin_family = AF_INET;
	_sin.sin_port = htons(4567);
	_sin.sin_addr.S_un.S_addr = ADDR_ANY;//这是服务端 可以连接任何的网络 直接写any
	int ret=bind(sock, (sockaddr*)&_sin, sizeof(_sin));
	if (ret == SOCKET_ERROR)
	{
		printf("ERROR,绑定用于接受客户端连接的网络端口失败...\n");
	}
	else
	{
		printf("TURE,绑定用于接受客户端连接的网络端口成功.....\n");
	}
	//3.listen 监听端口  这个数量随便写超过1的数
	ret = listen(sock, 5);
	if (ret == SOCKET_ERROR)
	{
		printf("ERROR, 监听网络端口失败...\n");
	}
	else
	{
		printf("TURE, 监听网络端口成功.....\n");
	}
	
//	printf("新客户端加入：socket=%d\n", (int)_clientSock);
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

		for (int i =g_clients.size()-1; i >=0; i--)
		{
			FD_SET(g_clients[i], &fd_read);//可以放 只读 另外两个也可以放
		}
		struct timeval time;
		time.tv_sec = 0;//秒
		time.tv_usec = 0;//微妙
		int ret= select(sock + 1, &fd_read, &fd_write, &fd_error, &time);
		if (ret==-1)
		{
			printf("客户端已退出,任务结束.\n");
			break;
		}
		// 判断fd对应的标志位到底是0还是1, 返回值: fd对应的标志位的值, 0, 返回0, 1->返回1
		//有新连接
		if (FD_ISSET(sock,&fd_read))
		{
			FD_CLR(sock, &fd_read); //将参数文件描述符fd对应的标志位, 设置为0
			//4.accept 等待客户端连接
			sockaddr_in addClient = {};
			int len = sizeof(addClient);
			SOCKET _clientSock = INVALID_SOCKET;
			// 默认是一个阻塞函数, 阻塞等待客户端请求
			// 请求到达, 接收客户端连接
			// 得到一个用于通信的文件描述符
			_clientSock = accept(sock, (sockaddr*)&addClient, &len);
			if (_clientSock == INVALID_SOCKET)
			{
				printf("ERROR,等待接受客户端连接失败...\n");
			}
			printf("新客户端加入：socket=%d\n", (int)_clientSock);
			g_clients.push_back(_clientSock);

		}
		// 通信, 有客户端发送数据过来
		for (int i = 0; i < fd_read.fd_count; i++)
		{
			if (-1==Processor(fd_read.fd_array[i]))
			{
				auto iter = find(g_clients.begin(), g_clients.end(), fd_read.fd_array[i]);
				if (iter!=g_clients.end())
				{
					//清除
					g_clients.erase(iter);
				}
			}
		}
		
	}
	//7.关闭套接字
	closesocket(sock);
	//清除windows socket环境
	WSACleanup();
	printf("客户端已退出,任务结束\n");
	getchar();
	return 0;
}
