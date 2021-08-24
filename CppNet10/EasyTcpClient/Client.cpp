#pragma once
#define  WIN32_LEAN_AND_MEAN  //不影响 windows.h 和 WinSock2.h 前后顺序 
#define _WINSOCK_DEPRECATED_NO_WARNINGS //这个用于 inet_ntoa   可以在右击项目属性 C/C++ 预处理里面 预处理定义添加
#include <WinSock2.h>
#include<windows.h>
#include <stdio.h>
/*为了可以在其他平台也可以使用 右键项目属性 选择链接器 附加依赖项 将ws2_32.lib 添加进去就行 这样就不需要 下面这些 */
#pragma  comment(lib,"ws2_32.lib")

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
		else
		{
			//5.向服务端发送请求
			send(sock, buf, strlen(buf)+1, 0);
		}
		//6.接收服务端信息
		char msg[256] = "";
		int nlen = recv(sock, msg, sizeof(msg), 0);
		printf("接收到数据长度为%d\n", nlen);
		if (nlen>0)
		{
			printf("数据为:%s\n", msg);
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