/****************************************************
	文件：Client.cpp
	作者：苏福龙
	邮箱: 747831151@qq.com
	日期：2021/08/24 20:26   	
	功能：客户端
*****************************************************/
#pragma once
#define  WIN32_LEAN_AND_MEAN  //不影响 windows.h 和 WinSock2.h 前后顺序 
#define _WINSOCK_DEPRECATED_NO_WARNINGS //这个用于 inet_ntoa   可以在右击项目属性 C/C++ 预处理里面 预处理定义添加
#include <WinSock2.h>
#include<windows.h>
#include <stdio.h>
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
int main()
{
	/*启动socket网络环境 2.x环境*/
	WORD ver = MAKEWORD(2, 2);//版本号
	WSADATA dat;
	WSAStartup(ver, &dat);
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
	_sin.sin_addr.S_un.S_addr = inet_addr("192.168.17.1");
	int ret = connect(sock, (sockaddr*)&_sin, sizeof(_sin));
	if (ret == -1)
	{
		printf("ERROR,连接服务器connect失败...\n");
	}
	else
	{
		printf("TURE,连接服务器connect成功...\n");
	}
	while (true)
	{
		//3.输入请求
		char buf[] = "";
		scanf("%s", buf);
		//4.处理请求命令
		if (0==strcmp(buf,"exit"))
		{
			printf("收到exit命令,任务结束");
			break;
		}
		else if(0==strcmp(buf,"login"))
		{
			Login login;
			strcpy(login.passWord, "123456");
			strcpy(login.userName, "小强");
			//5.向服务端发送请求
			send(sock, (const char*)&login, sizeof(login), 0);
			//6.接收服务端返回的数据
			LoginResult loginRet;
			recv(sock, (char*)&loginRet, sizeof(loginRet), 0);
			printf("LoginResult %d\n", loginRet.result);
		}
		else if (0 == strcmp(buf, "loginOut"))
		{
			LoginOut loginOut;
			strcpy(loginOut.userName, "小强");
			//5.向服务端发送请求
			send(sock, (const char*)&loginOut, sizeof(loginOut), 0);
			//6.接收服务端返回的数据
			LoginOutResult loginRet;
			recv(sock, (char*)&loginRet, sizeof(loginRet), 0);
			printf("LoginOutResult %d\n", loginRet.result);
		}
		else
		{
			printf("收到不支持命令,请重新输入.\n");
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