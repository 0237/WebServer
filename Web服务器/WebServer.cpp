#define _CRT_SECURE_NO_WARNINGS

#include<winsock2.h>
#include<iostream>
#include<string>
#include<io.h>
#include<thread>

#pragma comment(lib, "ws2_32.lib")

// 获取文件类型  
void GetFileType(char * filename, char * fileType)
{
	memset(fileType, 0, 20);
	if (strstr(filename, ".html") || 0 == strcmp(filename, "/"))
	{
		strcpy(fileType, "text/html");
	}
	else if (strstr(filename, ".bmp"))
	{
		strcpy(fileType, "application/x-bmp");
	}
	else if (strstr(filename, ".gif"))
	{
		strcpy(fileType, "image/gif");
	}
	else if (strstr(filename, ".png"))
	{
		strcpy(fileType, "image/png");
	}
	else if (strstr(filename, ".jpg"))
	{
		strcpy(fileType, "image/jpeg");
	}
	else
	{
		strcpy(fileType, "text/plain");
	}
}

// 处理静态请求  
void HandleStatic(SOCKET clientSocket, char * filename)
{
	// 直接返回服务器主页  
	if (0 == strcmp(filename, "./www/"))
	{
		strcpy(filename, "./www/index.html");
	}

	// 检测请求文件的类型  
	char fileType[20];
	GetFileType(filename, fileType);
	
	FILE *fp = fopen(filename, "rb");

	if (fp == 0)
	{
		// 如果找不到响应的数据就返回404状态码，并将其发送给客户端  
		char response[] = "HTTP/1.1 404 NOT FOUND\r\n\r\n";
		send(clientSocket, response, strlen(response), 0);
	}
	else
	{
		// 在文件目录中找响应的资源  
		int file_size = 0;
		char *content;
		char *response;
		fseek(fp, 0, SEEK_END);
		file_size = ftell(fp);
		fseek(fp, 0, SEEK_SET);
		content = (char*)malloc(file_size + 1);
		int i = fread(content, file_size, 1, fp);
		content[file_size] = 0;

		response = (char*)malloc(file_size + 100);

		// 发送请求headers到客户端  
		sprintf(response, "HTTP/1.1 200 OK\r\n");
		sprintf(response, "%sContent-Type: %s\r\n", response, fileType);
		sprintf(response, "%sContent-Length: %d\r\n", response, file_size);
		sprintf(response, "%sServer: Web Server\r\n\r\n", response);
		//send(clientSocket, response, strlen(response), 0);

		if (send(clientSocket, response, strlen(response), 0) == SOCKET_ERROR)
		{
			printf("Send headers failed!");
			closesocket(clientSocket);
		}
		else
		{
			// 显示headers
			printf(response);
		}

		// 发送请求body到客户端  
		sprintf(response, "%s", content);
		//send(clientSocket, response, strlen(response), NULL);

		if (send(clientSocket, content, file_size, 0) == SOCKET_ERROR)
		{
			printf("Send body failed!");
			closesocket(clientSocket);
		}
		else
		{
			// 显示body
			//printf(content);
			printf("\r\n\r\n");
		}

		//free(content);
	}
}

void thread_task(int NewConnection)
{
	char				 revBuff[1000];
	// 接受请求
	ZeroMemory(revBuff, 1000);
	if (recv(NewConnection, revBuff, 1000, 0) == SOCKET_ERROR)
	{
		printf("recv failed\n");
		closesocket(NewConnection);
	}

	// 显示请求
	printf(revBuff);

	// 响应浏览器
	char temp[BUFSIZ];
	char file[BUFSIZ];

	strcpy(file, "./www");

	if (sscanf(revBuff, "%s %s", temp, file + 5) == 2)
	{	
		// 处理请求
		HandleStatic(NewConnection, file);
	}
	else
	{
		printf("parameter error\n");
	}
	// 关闭Socket
	closesocket(NewConnection);
}

int main()
{
	WSADATA              wsaData;
	SOCKET               ListeningSocket;
	SOCKET               NewConnection;
	SOCKADDR_IN          ServerAddr;
	SOCKADDR_IN          ClientAddr;
	int                  Port = 81;
	int					 ClientAddrLen;
	int					 test = 0;

	// 初始化Windows Socket 2.0
	if (WSAStartup(MAKEWORD(2, 0), &wsaData) != 0)
	{
		printf("sock init failed\n");
		WSACleanup();
		return -1;
	}
	printf("sock init succeed\n");

	// 创建一个新的Socket来响应客户端的连接请求
	ListeningSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (ListeningSocket == SOCKET_ERROR)
	{
		printf("sock create failed\n");
		WSACleanup();
		return -1;
	}
	printf("sock create successed\n");

	// 填写服务器地址信息
	// 端口为81
	// IP地址为INADDR_ANY，htonl将IP地址转换为网络格式
	ServerAddr.sin_family = AF_INET;// TCP/IP
	ServerAddr.sin_port = htons(Port);
	ServerAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	// 绑定监听端口
	if (bind(ListeningSocket, (SOCKADDR *)&ServerAddr, sizeof(ServerAddr)) == SOCKET_ERROR)
	{
		printf("bind is failed\n");
		closesocket(ListeningSocket);
		WSACleanup();
		return -1;
	}
	printf("bind is successsed\n");

	// 开始监听，指定为最大连接数
	if (listen(ListeningSocket, SOMAXCONN) == SOCKET_ERROR)
	{
		printf("listen failed\n");
		closesocket(ListeningSocket);
		WSACleanup();
		return -1;
	}
	printf("listening...\n");

	ClientAddrLen = sizeof(sockaddr_in);
	while (true)
	{
		// 接受新的连接
		NewConnection = accept(ListeningSocket, (SOCKADDR *)&ClientAddr, &ClientAddrLen);
		if (NewConnection == SOCKET_ERROR)
		{
			printf("sock create failed\n");
			closesocket(NewConnection);
			//return -1;
			//continue;
		}
		else
		{
			printf("\nNO.%d\n", test++);
			std::thread t(thread_task, NewConnection);
			t.join();
		}
		//Sleep(1000);
	}

	// 关闭监听Socket，然后退出应用程序  
	closesocket(ListeningSocket);

	// 释放Windows Socket 的相关资源
	WSACleanup();

	return 0;
}