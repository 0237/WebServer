#define _CRT_SECURE_NO_WARNINGS 
//#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[])
{
	char chFileIn[] = "d:\\aa.png";
	char chFileOut[] = "d:\\bb.png";
	FILE *fIn;
	FILE *fOut;
	int nRead;
	char chBuf[1024];

	fIn = fopen(chFileIn, "rb");
	if (fIn == NULL)
	{
		printf("�򿪶�ȡ�ļ�ʧ��");
		return 0;
	}

	fOut = fopen(chFileOut, "wb");
	if (fOut == NULL)
	{
		printf("��д�ļ�ʧ��");
		return 0;
	}

	fseek(fIn, 0, SEEK_END);
	int file_size = ftell(fIn);
	fseek(fIn, 0, SEEK_SET);

	//���ļ�
	nRead = fread(chBuf, file_size, 1, fIn);
	//chBuf[file_size] = 0;
	//nRead = fread(chBuf, sizeof(char), 200, fIn);
	//д�ļ�
	fwrite(chBuf, file_size, nRead, fOut);
	//fwrite(chBuf, sizeof(char), nRead, fOut);
	/*while (1)
	{
		fwrite(chBuf, sizeof(char), nRead, fOut);
		if (nRead<200)
		{
			break;
		}

		nRead = fread(chBuf, sizeof(char), 200, fIn);
	}*/

	fclose(fIn);
	fclose(fOut);

	printf("��������");

	return 0;
}