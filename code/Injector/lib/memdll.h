/*
 * Memory DLL loading code
 * Version 0.0.2
 *
 * Copyright (c) 2004-2005 by Joachim Bauch / mail@joachim-bauch.de
 * http://www.joachim-bauch.de
 *
 * The contents of this file are subject to the Mozilla Public License Version
 * 1.1 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * The Original Code is MemoryModule.h
 *
 * The Initial Developer of the Original Code is Joachim Bauch.
 *
 * Portions created by Joachim Bauch are Copyright (C) 2004-2005
 * Joachim Bauch. All Rights Reserved.
 *
 * 以上是原作者信息 , Diy  by 透明色 
 */

#ifndef __MEMORY_MODULE_HEADER
#define __MEMORY_MODULE_HEADER

#include <Windows.h>

typedef struct {
	PIMAGE_NT_HEADERS headers;
	unsigned char *imageBase;
	HMODULE *modules;
	int numModules;
	int initialized;
} MEMORYMODULE, *PMEMORYMODULE;

//PMEMORYMODULE MemoryLoadExecute( const void * , bool);


PMEMORYMODULE MemoryLoadLibrary(const void *);
PMEMORYMODULE MemoryLoadLibraryRes(HMODULE h , int id , LPCTSTR type);


FARPROC MemoryGetProcAddress(PMEMORYMODULE, const char *);

void MemoryFreeLibrary(PMEMORYMODULE);

PMEMORYMODULE LoadDllFromResouse(HMODULE h , int id , LPCTSTR type);


//	void prepareInject();
BOOL remoteInject(HANDLE hProcess, LPVOID data , DWORD dataSize);
BOOL remoteInject(HANDLE hProcess, HMODULE h , int id , LPCTSTR type );

BOOL trapInject(HANDLE hProcess, DWORD addr,  LPVOID data , DWORD dataSize);
BOOL trapInject(HANDLE hProcess,LPVOID addr, HMODULE h , int id , LPCTSTR type);



#endif
