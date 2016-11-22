//////////////////////////////////////////////////  
//  
//      ����һ����С��WebServer������HTTPSЭ��  
//  
/////////////////////////////////////////////////////  
//////////////////  
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "helpFunction.h"  


// ������  
int main()
{
	// ��ʼ��windows socket  
	WSADATA wd;
	SOCKET s;
	if (WSAStartup(MAKEWORD(2, 2), &wd) < 0)
	{
		fprintf(stderr, "winsock startup failed\n");
		exit(-1);
	}

	// �����������׽���  
	s = socket(AF_INET, SOCK_STREAM, 0);

	// �󶨲�����  
	SOCKADDR_IN addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(827);
	addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	int ret = bind(s, (struct sockaddr*)&addr, sizeof(addr));
	if (ret < 0)
	{
		fprintf(stderr, "bind failed\n");
		closesocket(s);
		exit(-1);
	}

	// ���м���  
	ret = listen(s, 1024);
	if (ret < 0)
	{
		fprintf(stderr, "listen failed\n");
		closesocket(s);
		exit(-1);
	}

	char request[8000] = { 0 };  // ����  
	while (true)
	{
		// ���ӵ��ͻ��˵��׽���  
		SOCKET clientSocket = accept(s, 0, 0);

		// ���ܿͻ��˵���Ϣ  
		recv(clientSocket, request, sizeof(request), 0);

		// ���ͻ��˵���Ϣ������ն�  
		printf(request);

		// ����������Ҫ���ܺ���  
		HandleRequestLine(clientSocket, request);
	}

	return 0;
}