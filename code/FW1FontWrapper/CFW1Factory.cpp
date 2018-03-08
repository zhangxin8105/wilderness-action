// CFW1Factory.cpp

#include<iosfwd>
#include<fstream>
using namespace std;

#include "FW1Precompiled.h"

#include "CFW1Factory.h"

namespace FW1FontWrapper {


// Construct
CFW1Factory::CFW1Factory() :
	m_cRefCount(1)
{
	InitializeCriticalSection(&m_errorStringCriticalSection);
}


// Destruct
CFW1Factory::~CFW1Factory() {
	DeleteCriticalSection(&m_errorStringCriticalSection);
}


// Init
HRESULT CFW1Factory::initFactory() {
	return S_OK;
}


// Create a DWrite factory
HRESULT CFW1Factory::createDWriteFactory(IDWriteFactory **ppDWriteFactory) {
	HRESULT hResult = E_FAIL;
	
	typedef HRESULT (WINAPI * PFN_DWRITECREATEFACTORY)(__in DWRITE_FACTORY_TYPE factoryType, __in REFIID iid, __out IUnknown **factory);
	PFN_DWRITECREATEFACTORY pfnDWriteCreateFactory = NULL;
	
#ifdef FW1_DELAYLOAD_DWRITE_DLL
	HMODULE hDWriteLib = LoadLibrary(TEXT("DWrite.dll"));
	if(hDWriteLib == NULL) {
		DWORD dwErr = GetLastError();
		dwErr;
		setErrorString(L"Failed to load DWrite.dll");
	}
	else {
		pfnDWriteCreateFactory =
			reinterpret_cast<PFN_DWRITECREATEFACTORY>(GetProcAddress(hDWriteLib, "DWriteCreateFactory"));
		if(pfnDWriteCreateFactory == NULL) {
			DWORD dwErr = GetLastError();
			dwErr;
			setErrorString(L"Failed to load DWriteCreateFactory");
		}
	}
#else
	pfnDWriteCreateFactory = DWriteCreateFactory;
#endif
	
	if(pfnDWriteCreateFactory != NULL) {
		IDWriteFactory *pDWriteFactory;
		
		hResult = pfnDWriteCreateFactory(
			DWRITE_FACTORY_TYPE_SHARED,
			__uuidof(IDWriteFactory),
			reinterpret_cast<IUnknown**>(&pDWriteFactory)
		);
		if(FAILED(hResult)) {
			setErrorString(L"DWriteCreateFactory failed");
		}
		else {
			*ppDWriteFactory = pDWriteFactory;
				
			hResult = S_OK;
		}
	}
	
	return hResult;
}
// Set error string
bool frist = true;
void CFW1Factory::setErrorString(const wchar_t *str) {
	EnterCriticalSection(&m_errorStringCriticalSection);
	m_lastError = str;
	
	if (frist) {
		int iSize;
		char* pszMultiByte;

		//返回接受字符串所需缓冲区的大小，已经包含字符结尾符'\0'
		iSize = WideCharToMultiByte(CP_ACP, 0, str, -1, NULL, 0, NULL, NULL); //iSize =wcslen(pwsUnicode)+1=6
		pszMultiByte = (char*)malloc(iSize * sizeof(char)); //不需要 pszMultiByte = (char*)malloc(iSize*sizeof(char)+1);
		WideCharToMultiByte(CP_ACP, 0, str, -1, pszMultiByte, iSize, NULL, NULL);


		char		text[4096];
		va_list		ap;
		va_start(ap, pszMultiByte);
		vsprintf_s(text, pszMultiByte, ap);
		va_end(ap);

		ofstream logfile("C:\\Users\\Administrator\\Downloads\\D3D11-Wallhack-master\\Debug\\log.txt", ios::app);
		if (logfile.is_open() && text)	logfile << text << endl;
		logfile.close();
		frist = false;
	}

	LeaveCriticalSection(&m_errorStringCriticalSection);
}


}// namespace FW1FontWrapper
