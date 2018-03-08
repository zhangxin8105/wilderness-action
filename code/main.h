//get dir
using namespace std;
#include <fstream>
char dlldir[320];
char *GetDirectoryFile(char *filename)
{
	static char path[320];
	strcpy_s(path, dlldir);
	strcat_s(path, filename);
	return path;
}

//log
void Log(const char *fmt, ...)
{
	if (!fmt)	return;

	char		text[4096];
	va_list		ap;
	va_start(ap, fmt);
	vsprintf_s(text, fmt, ap);
	va_end(ap);

	ofstream logfile(GetDirectoryFile("log.txt"), ios::app);
	if (logfile.is_open() && text)	logfile << text << endl;
	logfile.close();
}

//==========================================================================================================================

//generate shader func
HRESULT GenerateShader(ID3D11Device* pD3DDevice, ID3D11PixelShader** pShader, float r, float g, float b)
{
	char szCast[] = "struct VS_OUT"
		"{"
		" float4 Position : SV_Position;"
		" float4 Color : COLOR0;"
		"};"

		"float4 main( VS_OUT input ) : SV_Target"
		"{"
		" float4 fake;"
		" fake.a = 1.0f;"
		" fake.r = %f;"
		" fake.g = %f;"
		" fake.b = %f;"
		" return fake;"
		"}";
	ID3D10Blob* pBlob;
	char szPixelShader[1000];

	sprintf_s(szPixelShader, szCast, r, g, b);

	ID3DBlob* d3dErrorMsgBlob;

	HRESULT hr = D3DCompile(szPixelShader, sizeof(szPixelShader), "shader", NULL, NULL, "main", "ps_4_0", NULL, NULL, &pBlob, &d3dErrorMsgBlob);

	if (FAILED(hr))
		return hr;

	hr = pD3DDevice->CreatePixelShader((DWORD*)pBlob->GetBufferPointer(), pBlob->GetBufferSize(), NULL, pShader);

	if (FAILED(hr))
		return hr;

	return S_OK;
}

//==========================================================================================================================

//wh
char *state;
ID3D11RasterizerState * rwState;
ID3D11RasterizerState * rsState;

enum eDepthState
{
	ENABLED,
	DISABLED,
	READ_NO_WRITE,
	NO_READ_NO_WRITE,
	_DEPTH_COUNT
};

ID3D11DepthStencilState* myDepthStencilStates[static_cast<int>(eDepthState::_DEPTH_COUNT)];

void SetDepthStencilState(eDepthState aState)
{
	pContext->OMSetDepthStencilState(myDepthStencilStates[aState], 1);
}

struct signature_t
{
	void *allocBase;
	void *memInBase;
	size_t memSize;
	void *offset;
	const char *sig;
	size_t siglen;
};


bool ResolveAddress(signature_t *sigmem)
{
	MEMORY_BASIC_INFORMATION mem;

	if (!VirtualQuery(sigmem->memInBase, &mem, sizeof(MEMORY_BASIC_INFORMATION)))
		return false;

	if (mem.AllocationBase == NULL)
		return false;

	HMODULE dll = (HMODULE)mem.AllocationBase;

	//code adapted from hullu's linkent patch
	union
	{
		unsigned long mem;
		IMAGE_DOS_HEADER *dos;
		IMAGE_NT_HEADERS *pe;
	} dllmem;

	dllmem.mem = (unsigned long)dll;

	if (IsBadReadPtr(dllmem.dos, sizeof(IMAGE_DOS_HEADER)) || (dllmem.dos->e_magic != IMAGE_DOS_SIGNATURE))
		return false;

	dllmem.mem = ((unsigned long)dll + (unsigned long)(dllmem.dos->e_lfanew));
	if (IsBadReadPtr(dllmem.pe, sizeof(IMAGE_NT_HEADERS)) || (dllmem.pe->Signature != IMAGE_NT_SIGNATURE))
		return false;

	//end adapted hullu's code

	IMAGE_NT_HEADERS *pe = dllmem.pe;

	sigmem->allocBase = mem.AllocationBase;
	sigmem->memSize = (DWORD)(pe->OptionalHeader.SizeOfImage);

	return true;
}

void* FindMemory(void *memInBase, const char *pattern, size_t siglen)
{
	signature_t sig;

	memset(&sig, 0, sizeof(signature_t));

	sig.sig = (const char *)pattern;
	sig.siglen = siglen;
	sig.memInBase = memInBase;

	if (!ResolveAddress(&sig))
		return NULL;

	const char *paddr = (const char *)sig.allocBase;
	bool found;

	register unsigned int j;

	sig.memSize -= sig.siglen;	//prevent a crash maybe?

	for (size_t i = 0; i<sig.memSize; i++)
		//for (size_t i=0; i<sig.memSize; i+=sizeof(unsigned long *))
	{
		found = true;
		for (j = 0; j<sig.siglen; j++)
		{
			if ((pattern[j] != (unsigned char)0x2A) &&
				(pattern[j] != paddr[j]))
			{
				found = false;
				break;
			}
		}
		if (found)
		{
			sig.offset = (void *)paddr;
			break;
		}
		//we're always gonna be on a four byte boundary
		paddr += 1;
		//paddr += sizeof(unsigned long *);
	}

	return sig.offset;
}
//==========================================================================================================================
