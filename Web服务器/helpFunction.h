#include <stdio.h>  
#include <stdlib.h>  
#include <string.h>  
#include <winsock2.h>  
#pragma comment ( lib , "ws2_32" )  

#define MAXLINE 10000  


// ��������  

// ��ȡ�ļ�����  
void GetFileType(char * filename, char * fileType);

// ����̬����  
void HandleStatic(SOCKET clientSocket, char * filename);

// ����̬����ĺ���  
void HandleDynamicFunc(char * filename, char * args, int & result);

// ����̬����  
void HandleDynamic(SOCKET clientSocket, char *filename, char * args);

// ����URI  
bool ParseURI(SOCKET clientSocket, char * uri, char * filename, char *args);

// ��������(Parse Request )�����Ϊ��̬����true,���Ϊ��̬������false  
void ParseRequest(SOCKET clientSocket, char * request);

// ���������У���ȡ��ͻ��˵����󣬸�����Ӧ  
void HandleRequestLine(SOCKET clientSocket, char * request);