#define _CRT_SECURE_NO_WARNINGS

#include<winsock2.h>
#include<iostream>
#include<string>
#include<io.h>
#include<thread>

#pragma comment(lib, "ws2_32.lib")

// ��ȡ�ļ�����  
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

// ����̬����  
void HandleStatic(SOCKET clientSocket, char * filename)
{
	// ֱ�ӷ��ط�������ҳ  
	if (0 == strcmp(filename, "./www/"))
	{
		strcpy(filename, "./www/index.html");
	}

	// ��������ļ�������  
	char fileType[20];
	GetFileType(filename, fileType);
	
	FILE *fp = fopen(filename, "rb");

	if (fp == 0)
	{
		// ����Ҳ�����Ӧ�����ݾͷ���404״̬�룬�����䷢�͸��ͻ���  
		char response[] = "HTTP/1.1 404 NOT FOUND\r\n\r\n";
		send(clientSocket, response, strlen(response), 0);
	}
	else
	{
		// ���ļ�Ŀ¼������Ӧ����Դ  
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

		// ��������headers���ͻ���  
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
			// ��ʾheaders
			printf(response);
		}

		// ��������body���ͻ���  
		sprintf(response, "%s", content);
		//send(clientSocket, response, strlen(response), NULL);

		if (send(clientSocket, content, file_size, 0) == SOCKET_ERROR)
		{
			printf("Send body failed!");
			closesocket(clientSocket);
		}
		else
		{
			// ��ʾbody
			//printf(content);
			printf("\r\n\r\n");
		}

		//free(content);
	}
}

void thread_task(int NewConnection)
{
	char				 revBuff[1000];
	// ��������
	ZeroMemory(revBuff, 1000);
	if (recv(NewConnection, revBuff, 1000, 0) == SOCKET_ERROR)
	{
		printf("recv failed\n");
		closesocket(NewConnection);
	}

	// ��ʾ����
	printf(revBuff);

	// ��Ӧ�����
	char temp[BUFSIZ];
	char file[BUFSIZ];

	strcpy(file, "./www");

	if (sscanf(revBuff, "%s %s", temp, file + 5) == 2)
	{	
		// ��������
		HandleStatic(NewConnection, file);
	}
	else
	{
		printf("parameter error\n");
	}
	// �ر�Socket
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

	// ��ʼ��Windows Socket 2.0
	if (WSAStartup(MAKEWORD(2, 0), &wsaData) != 0)
	{
		printf("sock init failed\n");
		WSACleanup();
		return -1;
	}
	printf("sock init succeed\n");

	// ����һ���µ�Socket����Ӧ�ͻ��˵���������
	ListeningSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (ListeningSocket == SOCKET_ERROR)
	{
		printf("sock create failed\n");
		WSACleanup();
		return -1;
	}
	printf("sock create successed\n");

	// ��д��������ַ��Ϣ
	// �˿�Ϊ81
	// IP��ַΪINADDR_ANY��htonl��IP��ַת��Ϊ�����ʽ
	ServerAddr.sin_family = AF_INET;// TCP/IP
	ServerAddr.sin_port = htons(Port);
	ServerAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	// �󶨼����˿�
	if (bind(ListeningSocket, (SOCKADDR *)&ServerAddr, sizeof(ServerAddr)) == SOCKET_ERROR)
	{
		printf("bind is failed\n");
		closesocket(ListeningSocket);
		WSACleanup();
		return -1;
	}
	printf("bind is successsed\n");

	// ��ʼ������ָ��Ϊ���������
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
		// �����µ�����
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

	// �رռ���Socket��Ȼ���˳�Ӧ�ó���  
	closesocket(ListeningSocket);

	// �ͷ�Windows Socket �������Դ
	WSACleanup();

	return 0;
}