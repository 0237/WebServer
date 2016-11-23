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
		printf("打开读取文件失败");
		return 0;
	}

	fOut = fopen(chFileOut, "wb");
	if (fOut == NULL)
	{
		printf("打开写文件失败");
		return 0;
	}

	fseek(fIn, 0, SEEK_END);
	int file_size = ftell(fIn);
	fseek(fIn, 0, SEEK_SET);

	//读文件
	nRead = fread(chBuf, file_size, 1, fIn);
	//chBuf[file_size] = 0;
	//nRead = fread(chBuf, sizeof(char), 200, fIn);
	//写文件
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

	printf("操作结束");

	return 0;
}