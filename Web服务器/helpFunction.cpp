#define _CRT_SECURE_NO_WARNINGS
#include "helpFunction.h"  


// ��ȡ�ļ�����  
void GetFileType(char * filename, char * fileType)
{
	memset(fileType, 0, 20);
	if (strstr(filename, ".html") || 0 == strcmp(filename, "/"))
	{
		strcpy(fileType, "text/html");
	}
	else if (strstr(filename, ".gif"))
	{
		strcpy(fileType, "image/gif");
	}
	else if (strstr(filename, ".jpg"))
	{
		strcpy(fileType, "imgae/jpeg");
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
	if (0 == strcmp(filename, "/"))
	{
		strcpy(filename, "/index.html");

	}

	FILE *fp = fopen(filename, "r");
	if (fp == 0)
	{
		// ����Ҳ�����Ӧ�����ݾͷ���404״̬�룬�����䷢�͸��ͻ���  
		char response[] = "HTTP/1.1 404 NOT FOUND\r\n\r\n";
		send(clientSocket, response, strlen(response), 0);
	}
	else
	{
		// ���ļ�Ŀ¼������Ӧ����Դ  
		int file_size;
		char *content;
		char response[1024];
		fseek(fp, 0, SEEK_END);
		file_size = ftell(fp);
		fseek(fp, 0, SEEK_SET);
		content = (char*)malloc(file_size + 1);
		fread(content, file_size, 1, fp);
		content[file_size] = 0;

		// ��������ļ�������  
		char fileType[20];
		GetFileType(filename, fileType);

		// ��������headers���ͻ���  
		sprintf(response, "HTTP/1.1 200 OK\r\n");
		sprintf(response, "%sContent-Type: %s\r\n", response, fileType);
		sprintf(response, "%sContent-Length: %d\r\n", response, file_size);
		sprintf(response, "%sServer: Bobo Server\r\n\r\n", response);
		send(clientSocket, response, strlen(response), 0);

		// ��������body���ͻ���  
		sprintf(response, "%s", content);
		send(clientSocket, response, strlen(response), NULL);

		free(content);
	}
}

// ����̬����ĺ���  
void HandleDynamicFunc(char * filename, char * args, int & result)
{
	// ����̬�������  
	char * a = strtok(args, "&");
	char * b = strtok(NULL, "&");
	int x = atoi(a);
	int y = atoi(b);

	if (strstr(filename, "Add"))
	{
		result = x + y;
	}
	else if (strstr(filename, "Sub"))
	{
		result = x - y;
	}
}

// ����̬����  
void HandleDynamic(SOCKET clientSocket, char *filename, char * args)
{
	int result = 0;
	char response[8000];  // ��Ӧ����  
						  // ִ�ж�̬����  
	HandleDynamicFunc(filename, args, result);

	// ������Ӧͷ���ͻ���  
	sprintf(response, "HTTP/1.1 200 OK\r\n");
	sprintf(response, "%sServer: Bobo Server\r\n\r\n", response);
	send(clientSocket, response, strlen(response), NULL);

	//������ӦBody���ͻ���  
	sprintf(response, "<head><head><title>Welcome to Bobo Server!</title></head>");
	sprintf(response, "%s<body>The result is %d</body>", response, result);
	send(clientSocket, response, strlen(response), NULL);
}


//����URI  
bool ParseURI(SOCKET clientSocket, char * uri, char * filename, char *args)
{
	sprintf(filename, ".%s", uri);
	if (!strstr(uri, "dynamic"))  // ��̬����  
	{
		strcpy(args, "");
		strcpy(filename, ".");
		strcat(filename, uri);
		if (uri[strlen(uri) - 1] == '/')
		{
			strcat(filename, "index.html");
		}
		return true;
	}
	else  // ��̬����  
	{
		char * ptr = strstr(uri, "?");
		if (ptr)
		{
			strcpy(args, ptr + 1);
		}
		else
		{
			strcpy(args, "");
		}
		ptr = strstr(uri, "?");
		*ptr = '\0';

		strcpy(filename, uri);
		strcat(uri, "?");
		strcat(uri, args);
		return false;
	}
}

// ��������(Parse Request )�����Ϊ��̬����true,���Ϊ��̬������false  
void ParseRequest(SOCKET clientSocket, char * request)
{
	// �ж�URI������Ƿ�Ϊ��̬����  
	bool is_static;

	// ��ȡ�������������  
	char *method = strtok(request, " ");
	// ��ȡ�����uri  
	char *uri = strtok(0, " ");

	// ����URI  
	char args[20] = { 0 };    // URI�еĶ�̬�������  
	char filename[100] = { 0 };  // URI�о�̬�����е��ļ���  

	is_static = ParseURI(clientSocket, uri, filename, args);

	if (is_static)  // ����Ϊ��̬  
	{
		HandleStatic(clientSocket, filename);
	}
	//else  // ����Ϊ��̬  
	//{
	//	HandleDynamic(clientSocket, filename, args);
	//}
}


// ���������У���ȡ��ͻ��˵����󣬸�����Ӧ  
void HandleRequestLine(SOCKET clientSocket, char * request)
{
	// ����URI  
	ParseRequest(clientSocket, request);
}