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
	while (true)
	{
		char szRecv[4096] = {};//缓冲区
		//5.接收客户端请求数据  数据存到szRecv中第三个参数可接收得最大长度 最后一个设置为0
		int nlen = recv(_clientSock,szRecv,sizeof(DataHeader),0);//返回值是接收的长度
		if (nlen<=0)
		{
			printf("客户端已退出,任务结束\n");
			break;
		}
		DataHeader* header=(DataHeader*)szRecv;
		switch (header->cmd)
		{
		case CMD_LOGIN: {
			//我们进来这里面 我们前面收到消息头的数据  那么接下来我们要接收的登录消息的数据了  我们第二个参数要加上消息头的地址 就可以从那个位置开始接收数据了  
			//第三个数据 我们接收的数据长度要减去消息头的长度   header->dataLength是总的长度 sizeof(DataHeader)消息头的长度
			recv(_clientSock, szRecv+sizeof(DataHeader), header->dataLength-sizeof(DataHeader), 0);
			Login* login=(Login*)szRecv;
			//忽略 判断用户名密码是否正确
			printf("名字是=%s 密码是%s \n", login->userName, login->passWord);
			LoginResult loginRet;
			send(_clientSock, (const char*)&loginRet, sizeof(loginRet), 0);
		}
				break;
		case  CMD_LOGINOUT: {
		
			recv(_clientSock, szRecv+sizeof(DataHeader), header->dataLength-sizeof(DataHeader), 0);
			LoginOut* loginOut=(LoginOut*)szRecv;
			//忽略 判断用户名密码是否正确
			printf("名字是=%s   \n", loginOut->userName);
			LoginOutResult loginOutRet;
			send(_clientSock, (const char*)&loginOutRet, sizeof(loginOutRet), 0);
		}
			break;
		default: {
			header->cmd = CMD_ERROR;
			header->dataLength = 0;
			send(_clientSock, (char*)&header, sizeof(DataHeader), 0);
		}
			break;
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
