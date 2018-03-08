// Injector.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "stdafx.h"
#include <windows.h>
#include <iostream>
#include <tlhelp32.h>
#include <memdll.h>
#pragma comment(lib, "memLoad.lib")
using namespace std;
INT pid;


INT GetPid(CHAR *PwName) {
	HANDLE hwnd;
	PROCESSENTRY32 pro;
	pro.dwSize = sizeof(PROCESSENTRY32);
	BOOL flag;
	hwnd = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hwnd == 0) {
		return 0;
	}
	flag = Process32First(hwnd, &pro);
	while (flag) {
		if (strcmp(PwName, pro.szExeFile) == 0) {
			CloseHandle(hwnd);
			return pro.th32ProcessID;
		}
		else {
			flag = Process32Next(hwnd, &pro);
		}
	}
	CloseHandle(hwnd);
	return 0;
}

char dlldir[320];
int memInject(int pid, const char* path) {
	HANDLE procHandle = OpenProcess(PROCESS_CREATE_THREAD | PROCESS_QUERY_INFORMATION | PROCESS_VM_OPERATION | PROCESS_VM_WRITE | PROCESS_VM_READ, false, pid);

	FILE *fp;
	unsigned char *data = NULL;
	size_t size;
	DWORD resourceSize;
	LPVOID resourceData;
	TCHAR buffer[100];

	fp = _tfopen(path, _T("rb"));
	if (fp == NULL)
	{
		_tprintf(_T("Can't open DLL file \"%s\"."), path);
		return 1;
	}

	fseek(fp, 0, SEEK_END);
	size = ftell(fp);
	data = (unsigned char *)malloc(size);
	fseek(fp, 0, SEEK_SET);
	fread(data, 1, size, fp);
	fclose(fp);
	remoteInject(procHandle, data, (DWORD)size);
	return 0;
}

int main()
{
	GetModuleFileName(NULL, dlldir, MAX_PATH);
	for (size_t i = strlen(dlldir); i > 0; i--) { if (dlldir[i] == '\\') { dlldir[i + 1] = 0; break; } }
	static char path[320];
	strcpy_s(path, dlldir);
	strcat_s(path, "d3d11hook.dll");

	pid = GetPid("hyxd.exe");
	if (pid <= 0) {
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12);
		printf("初始化失败\n\n");
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
		system("pause");
		return 0;
	}

	if (memInject(pid, path) != 0) {
		printf("注入失败！\n\n");
	}
	else {
		printf("注入成功！请等待10秒钟\n\n");
	}
	system("pause");
	return 0;
}

