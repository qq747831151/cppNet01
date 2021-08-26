/****************************************************
	文件：Client.cpp
	作者：苏福龙
	邮箱: 747831151@qq.com
	日期：2021/08/24 20:26   	
	功能：客户端
*****************************************************/
#pragma once
#if _WIN32

#define  WIN32_LEAN_AND_MEAN  //不影响 windows.h 和 WinSock2.h 前后顺序 
#define _WINSOCK_DEPRECATED_NO_WARNINGS //这个用于 inet_ntoa   可以在右击项目属性 C/C++ 预处理里面 预处理定义添加
#include <WinSock2.h>
#include<windows.h>
/*为了可以在其他平台也可以使用 右键项目属性 选择链接器 附加依赖项 将ws2_32.lib 添加进去就行 这样就不需要 下面这些 */
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
	short cmd;//命令
	short dataLength;//数据长度

};
//登录
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
//登出
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
//处理
int Processor(SOCKET clientSockt)
{
	char szRecv[4096] = {};//缓冲区
		//5.接收客户端请求数据  数据存到szRecv中第三个参数可接收得最大长度 最后一个设置为0
	int nlen =(int)recv(clientSockt, szRecv, sizeof(DataHeader), 0);//返回值是接收的长度  MAC修改的地方
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
	case  CMD_NEWUSER:
	{
		
		recv(clientSockt, szRecv + sizeof(DataHeader), header->dataLength - sizeof(DataHeader), 0);
		LoginNewUser* loginNewUser = (LoginNewUser*)&szRecv;
		printf("其他的客户端Socket=%d\n", loginNewUser->sock);
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
bool g_bExit = true;//线程退出
//这个负责发送数据
void cmdThread(SOCKET sock)
{
	while (true)
	{
		////3.输入请求
		char buf[] = "";
		scanf("%s", buf);
		//4.处理命令
		if (0 == strcmp(buf, "exit"))
		{
			printf("收到exit命令,任务结束");
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
	/*启动socket网络环境 2.x环境*/
	WORD ver = MAKEWORD(2, 2);//版本号
	WSADATA dat;
	WSAStartup(ver, &dat);
#endif
	
	SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock == INVALID_SOCKET)
	{
		printf("ERROR,建立socket失败...\n");
	}
	else
	{
		printf("TURE,建立socket成功.....\n");
	}
	//2.连接服务器
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
		printf("ERROR,连接服务器connect失败...\n");
	}
	else
	{
		printf("TURE,连接服务器connect成功...\n");
	}
	//启动线程
	std::thread t1(cmdThread, sock);
	t1.detach();//线程分离
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
			printf("select, 任务结束\n");
			break;
		}
		//有发消息过来
		if (FD_ISSET(sock,&fd_read))
		{
			FD_CLR(sock, &fd_read);
			if (-1==Processor(sock))
			{
				printf("select 任务结束\n");
				break;
			}
		}
		//printf("空余时间处理其他业务\n");
	
	}
#if _WIN32
	//7.关闭套接字
	closesocket(sock);//这个是在windows下面的关闭函数
	//清除windows socket环境
	WSACleanup();
#else
	close(sock);
#endif
	
	printf("客户端已退出,任务结束\n");
	getchar();
	return 0;
}