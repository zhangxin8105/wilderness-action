#include <Windows.h>
#include <vector>
#include <d3d11.h>
#include <D3D11Shader.h>
#include <D3Dcompiler.h>//generateshader
#pragma comment(lib, "d3dcompiler43.lib")
#pragma comment(lib, "d3d11.lib")

#define ENABLE_FONT 1

#include "MinHook/include/MinHook.h" //detour x86&x64
#ifdef ENABLE_FONT
#include "FW1FontWrapper/FW1FontWrapper.h" //font
#endif

bool openFeature = true;
bool openColor = true;
bool showCar = true;

#ifdef HeightFeature
HMODULE mainModule;
BYTE wall_bytes[32] = { 0x24, 0x8B, 0x47, 0x04, 0x89, 0x44, 0x8A, 0x28, 0x8B, 0x4D, 0x28, 0x8B, 0x95, 0x34, 0xFE, 0xFF, 0xFF, 0x89, 0x55, 0x0C, 0xF3, 0x0F, 0x10, 0x41, 0x20, 0x0F, 0x2E, 0xC1, 0x9F, 0xF6, 0xC4, 0x44 };
BYTE speed_bytes[21] = { 0x00, 0x00, 0x7A, 0x44, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1D, 0x00, 0x00, 0x00, 0x1C };
//char installPath[255];

int* wall_addr;
float* speed_addr;
int* bullet_addr;
float* dundi_addr;
int* house_addr;
int* big_addr;
int big_back = -1;
float dundi_back = -1;
bool openBig = false;
bool openDundi = false;
bool openHouse = false;
bool openSpeed = false;
bool openBullet = false;

#endif

UINT maxWidth = 22262;
UINT minWidth = 96;

//车辆判断
#define Car (indesc.ByteWidth != 19434\
&& indesc.ByteWidth != 1044\
&& indesc.ByteWidth != 8880\
&& indesc.ByteWidth != 14538\
&&( indesc.ByteWidth < 636 || indesc.ByteWidth > 648 )\
&& indesc.ByteWidth != 22662\
&& indesc.ByteWidth != 144\
&& indesc.ByteWidth != 6240 /*盒子*/\
&& indesc.ByteWidth != 7152\
&& indesc.ByteWidth != 13692\
&& indesc.ByteWidth != 9240\
&& indesc.ByteWidth != 5946\
&& indesc.ByteWidth != 936\
&& indesc.ByteWidth != 18894\
&& indesc.ByteWidth != 2808\
&& indesc.ByteWidth != 1536\
)

//人物判断
#define Player (Stride == 24 && (\
(indesc.ByteWidth == 33912 || indesc.ByteWidth == 23370 || indesc.ByteWidth == 22662) || (\
(indesc.ByteWidth <= maxWidth && indesc.ByteWidth >= minWidth)\
&& (indesc.ByteWidth != 4320)\
&& (indesc.ByteWidth != 1200)\
&& (indesc.ByteWidth != 840)\
&& (indesc.ByteWidth != 528)\
&& (indesc.ByteWidth != 6114)\
&& (indesc.ByteWidth != 2688)\
&& (indesc.ByteWidth != 4932)\
&& (indesc.ByteWidth != 3684)\
&& (indesc.ByteWidth != 2856)\
&& (indesc.ByteWidth != 3960)\
&& (indesc.ByteWidth != 4986)\
&& (indesc.ByteWidth != 4848)\
&& (indesc.ByteWidth != 1326)\
&& (indesc.ByteWidth != 2964)\
&& (indesc.ByteWidth != 3270)\
&& (indesc.ByteWidth != 3288)\
&& (indesc.ByteWidth != 1806)\
&& (indesc.ByteWidth != 21012)\
&& (indesc.ByteWidth != 19200)\
&& (indesc.ByteWidth != 19632)\
&& (indesc.ByteWidth != 15402)\
&& (indesc.ByteWidth != 13590)\
&& (indesc.ByteWidth != 10440)\
&& (indesc.ByteWidth != 6522)\
&& (indesc.ByteWidth != 1764)\
&& (indesc.ByteWidth != 15030)\
&& (indesc.ByteWidth != 12684)\
&& (indesc.ByteWidth != 14220)\
&& (indesc.ByteWidth != 21282)\
&& (indesc.ByteWidth != 17004)\
&& (indesc.ByteWidth != 18654)\
&& (indesc.ByteWidth != 6864)\
&& (indesc.ByteWidth != 13176)\
&& (indesc.ByteWidth != 5640)\
&& (indesc.ByteWidth != 5100)\
)))

typedef HRESULT(__stdcall *D3D11PresentHook) (IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags);
typedef void(__stdcall *D3D11DrawHook) (ID3D11DeviceContext* pContext, UINT VertexCount, UINT StartVertexLocation);
typedef void(__stdcall *D3D11DrawIndexedHook) (ID3D11DeviceContext* pContext, UINT IndexCount, UINT StartIndexLocation, INT BaseVertexLocation);
typedef void(__stdcall *D3D11DrawInstancedHook) (ID3D11DeviceContext* pContext, UINT VertexCountPerInstance, UINT InstanceCount, UINT StartVertexLocation, UINT StartInstanceLocation);
typedef void(__stdcall *D3D11DrawIndexedInstancedHook) (ID3D11DeviceContext* pContext, UINT IndexCountPerInstance, UINT InstanceCount, UINT StartIndexLocation, INT BaseVertexLocation, UINT StartInstanceLocation);
typedef void(__stdcall *D3D11DrawInstancedIndirectHook) (ID3D11DeviceContext* pContext, ID3D11Buffer *pBufferForArgs, UINT AlignedByteOffsetForArgs);
typedef void(__stdcall *D3D11DrawIndexedInstancedIndirectHook) (ID3D11DeviceContext* pContext, ID3D11Buffer *pBufferForArgs, UINT AlignedByteOffsetForArgs);
typedef void(__stdcall *D3D11PSSetShaderResourcesHook) (ID3D11DeviceContext* pContext, UINT StartSlot, UINT NumViews, ID3D11ShaderResourceView *const *ppShaderResourceViews);
typedef void(__stdcall *D3D11CreateQueryHook) (ID3D11Device* pDevice, const D3D11_QUERY_DESC *pQueryDesc, ID3D11Query **ppQuery);



D3D11PresentHook phookD3D11Present = NULL;
D3D11DrawHook phookD3D11Draw = NULL;
D3D11DrawIndexedHook phookD3D11DrawIndexed = NULL;
D3D11DrawInstancedHook phookD3D11DrawInstanced = NULL;
D3D11DrawIndexedInstancedHook phookD3D11DrawIndexedInstanced = NULL;
D3D11DrawInstancedIndirectHook phookD3D11DrawInstancedIndirect = NULL;
D3D11DrawIndexedInstancedIndirectHook phookD3D11DrawIndexedInstancedIndirect = NULL;
D3D11PSSetShaderResourcesHook phookD3D11PSSetShaderResources = NULL;
D3D11CreateQueryHook phookD3D11CreateQuery = NULL;

ID3D11Device *pDevice = NULL;
ID3D11DeviceContext *pContext = NULL;

DWORD_PTR* pSwapChainVtable = NULL;
DWORD_PTR* pContextVTable = NULL;
DWORD_PTR* pDeviceVTable = NULL;

#ifdef ENABLE_FONT
IFW1Factory *pFW1Factory = NULL;
IFW1FontWrapper *pFontWrapper = NULL;
#endif

#include "main.h"

//==========================================================================================================================

//init only once
bool firstTime = true;

//vertex
ID3D11Buffer *veBuffer;
UINT Stride = 0;
UINT veBufferOffset = 0;
D3D11_BUFFER_DESC vedesc;

//index
ID3D11Buffer *inBuffer;
DXGI_FORMAT inFormat;
UINT        inOffset;
D3D11_BUFFER_DESC indesc;

//rendertarget
ID3D11Texture2D* RenderTargetTexture;
ID3D11RenderTargetView* RenderTargetView = NULL;

//shader
ID3D11PixelShader* psRed = NULL;
ID3D11PixelShader* psGreen = NULL;

//pssetshaderresources
UINT pssrStartSlot;
D3D11_SHADER_RESOURCE_VIEW_DESC  Descr;
ID3D11ShaderResourceView* ShaderResourceView;

//used for logging/cycling through values
bool logger = false;
//int countnum = 0;
char szString[64];

//==========================================================================================================================

HRESULT __stdcall hookD3D11Present(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags)
{
	if (firstTime)
	{
		//get device and context
		HRESULT hresult = pSwapChain->GetDevice(__uuidof(ID3D11Device), (void **)&pDevice);
		if (SUCCEEDED(hresult))
		{
			pSwapChain->GetDevice(__uuidof(pDevice), (void**)&pDevice);
			pDevice->GetImmediateContext(&pContext);
		}
		else {
			Log("Get device failed %02x", hresult);
			goto end;
		}

		//create depthstencilstate
		D3D11_DEPTH_STENCIL_DESC  stencilDesc;
		stencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
		stencilDesc.StencilEnable = true;
		stencilDesc.StencilReadMask = 0xFF;
		stencilDesc.StencilWriteMask = 0xFF;
		stencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		stencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
		stencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		stencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
		stencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		stencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
		stencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		stencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

		stencilDesc.DepthEnable = true;
		stencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		pDevice->CreateDepthStencilState(&stencilDesc, &myDepthStencilStates[static_cast<int>(eDepthState::ENABLED)]);

		stencilDesc.DepthEnable = false;
		stencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		pDevice->CreateDepthStencilState(&stencilDesc, &myDepthStencilStates[static_cast<int>(eDepthState::DISABLED)]);

		stencilDesc.DepthEnable = false;
		stencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
		stencilDesc.StencilEnable = false;
		stencilDesc.StencilReadMask = UINT8(0xFF);
		stencilDesc.StencilWriteMask = 0x0;
		pDevice->CreateDepthStencilState(&stencilDesc, &myDepthStencilStates[static_cast<int>(eDepthState::NO_READ_NO_WRITE)]);

		stencilDesc.DepthEnable = true;
		stencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL; //
		stencilDesc.DepthFunc = D3D11_COMPARISON_GREATER_EQUAL;
		stencilDesc.StencilEnable = false;
		stencilDesc.StencilReadMask = UINT8(0xFF);
		stencilDesc.StencilWriteMask = 0x0;

		stencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_ZERO;
		stencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_ZERO;
		stencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		stencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_EQUAL;

		stencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_ZERO;
		stencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_ZERO;
		stencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_ZERO;
		stencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_NEVER;
		pDevice->CreateDepthStencilState(&stencilDesc, &myDepthStencilStates[static_cast<int>(eDepthState::READ_NO_WRITE)]);

		//wireframe
		D3D11_RASTERIZER_DESC rwDesc;
		pContext->RSGetState(&rwState); // retrieve the current state
		rwState->GetDesc(&rwDesc);    // get the desc of the state
		rwDesc.FillMode = D3D11_FILL_WIREFRAME;
		rwDesc.CullMode = D3D11_CULL_NONE;
		// create a whole new rasterizer state
		pDevice->CreateRasterizerState(&rwDesc, &rwState);
#ifdef ENABLE_FONT
		//create font
		HRESULT hResult = FW1CreateFactory(FW1_VERSION, &pFW1Factory);
		hResult = pFW1Factory->CreateFontWrapper(pDevice, L"Regular", &pFontWrapper);
		pFW1Factory->Release();
		if (!pFontWrapper)
			Log("CreateFontWrapper Code:%02X", hResult);
#endif
		//solid
		D3D11_RASTERIZER_DESC rsDesc;
		pContext->RSGetState(&rsState); // retrieve the current state
		rsState->GetDesc(&rsDesc);    // get the desc of the state
		rsDesc.FillMode = D3D11_FILL_SOLID;
		rsDesc.CullMode = D3D11_CULL_BACK;
		// create a whole new rasterizer state
		pDevice->CreateRasterizerState(&rsDesc, &rsState);



		// use the back buffer address to create the render target
		//if (SUCCEEDED(pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<LPVOID*>(&RenderTargetTexture))))
#ifdef ENABLE_FONT
		//执行后可以显示自定义字体，但是改变窗口大小会产生bug
		if (SUCCEEDED(pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&RenderTargetTexture)))
		{
			pDevice->CreateRenderTargetView(RenderTargetTexture, NULL, &RenderTargetView);
			RenderTargetTexture->Release();
		}
#endif // DEBUG
		firstTime = false;
	}

	//ALT + CTRL + M toggles logger
	if ((GetAsyncKeyState(VK_MENU)) && (GetAsyncKeyState(VK_CONTROL)) && (GetAsyncKeyState('M') & 1))
	{
		logger = !logger;
	}

#ifndef ENABLE_FONT	
	if (GetAsyncKeyState(VK_F1) & 1)
	{
		Log("minWidth:%d maxWidth:%d", minWidth, maxWidth);
	}
#endif

	//shaders
	if (!psRed)
		GenerateShader(pDevice, &psRed, 1.0f, 0.0f, 0.0f);

	if (!psGreen)
		GenerateShader(pDevice, &psGreen, 0.0f, 1.0f, 0.0f);

#ifdef ENABLE_FONT
	//call before you draw
	pContext->OMSetRenderTargets(1, &RenderTargetView, NULL);

#endif // DEBUG
#ifdef ENABLE_FONT
	if (logger && pFontWrapper) //&& countnum >= 0)
	{
		//call before you draw
		pContext->OMSetRenderTargets(1, &RenderTargetView, NULL);

		wchar_t reportValue[256];
		swprintf_s(reportValue, L"minWidth = %d,maxWidth = %d", minWidth, maxWidth);
		pFontWrapper->DrawString(pContext, reportValue, 20.0f, 220.0f, 100.0f, 0xfff11111, FW1_RESTORESTATE);
	}
	if (pFontWrapper) {
		pFontWrapper->DrawString(pContext, L"F11 透视", 20.0f, 100.0f, 100.0f, 0xff00ff00, FW1_RESTORESTATE);
		pFontWrapper->DrawString(pContext, L"F12 变绿", 20.0f, 100.0f, 120.0f, 0xff00ff00, FW1_RESTORESTATE);
#ifdef HeightFeature
		pFontWrapper->DrawString(pContext, L"F10 2倍速", 20.0f, 100.0f, 140.0f, 0xff00ff00, FW1_RESTORESTATE);
		pFontWrapper->DrawString(pContext, L"ALT+Q 穿墙", 20.0f, 100.0f, 160.0f, 0xff00ff00, FW1_RESTORESTATE);
		pFontWrapper->DrawString(pContext, L"F9 子弹穿墙", 20.0f, 100.0f, 180.0f, 0xff00ff00, FW1_RESTORESTATE);
		pFontWrapper->DrawString(pContext, L"F8 去除房屋", 20.0f, 100.0f, 160.0f, 0xff00ff00, FW1_RESTORESTATE);
		pFontWrapper->DrawString(pContext, L"F7 人物放大", 20.0f, 100.0f, 140.0f, 0xff00ff00, FW1_RESTORESTATE);
		pFontWrapper->DrawString(pContext, L"F2 不透视车辆", 20.0f, 100.0f, 120.0f, 0xff00ff00, FW1_RESTORESTATE);
#endif
	}
#endif
	end:
	return phookD3D11Present(pSwapChain, SyncInterval, Flags);
}

//==========================================================================================================================

void __stdcall hookD3D11DrawIndexed(ID3D11DeviceContext* pContext, UINT IndexCount, UINT StartIndexLocation, INT BaseVertexLocation)
{
	if (GetAsyncKeyState(VK_F11) & 1)
		openFeature = !openFeature;
	if (GetAsyncKeyState(VK_F12) & 1)
		openColor = !openColor;
	if (GetAsyncKeyState(VK_F2) & 1)
		showCar = !showCar;
#ifdef HeightFeature
	if (GetAsyncKeyState(VK_MENU) && GetAsyncKeyState('Q') & 1 && wall_addr != NULL) {
		if (*wall_addr == 1289097062) {
			*wall_addr = 1154879334;
		}
		else {
			*wall_addr = 1289097062;
		}
	}


	if (GetAsyncKeyState(VK_F9) & 1) {
		if (!openBullet) {
			*bullet_addr = 4089587727;
		}
		else {
			*bullet_addr = 4089915407;
		}
		openBullet = !openBullet;
	}
	if (GetAsyncKeyState(VK_F10) & 1) {
		if (!openSpeed) {
			*speed_addr = 2000;
		}
		else {
			*speed_addr = 1000;
		}
		openSpeed = !openSpeed;
	}

	if (GetAsyncKeyState(VK_F8) & 1) {
		if (!openHouse) {
			*house_addr = 3361214451;
		}
		else {
			*house_addr = 3394768883;
		}
		openHouse = !openHouse;
	}

	if (GetAsyncKeyState(VK_F7) & 1) {
		if (!openBig) {
			*big_addr = 1056964608;
		}
		else {
			*big_addr = 0;
		}
		openBig = !openBig;
	}

	if (GetAsyncKeyState(VK_F6) & 1) {
		if (!openDundi) {
			*dundi_addr = 1;
		}
		else {
			*dundi_addr = 2;
		}
		openDundi = !openDundi;
	}
#endif
	if (!psRed || !psGreen || !pContext)
		goto end;
	//get stride & vdesc.ByteWidth
	pContext->IAGetVertexBuffers(0, 1, &veBuffer, &Stride, &veBufferOffset);
	if (veBuffer)
		veBuffer->GetDesc(&vedesc);

	if (veBuffer != NULL) { veBuffer->Release(); veBuffer = NULL; }
	else
		goto end;

	//get indesc.ByteWidth
	pContext->IAGetIndexBuffer(&inBuffer, &inFormat, &inOffset);
	if (inBuffer)
		inBuffer->GetDesc(&indesc);

	if (inBuffer != NULL) { inBuffer->Release(); inBuffer = NULL; }
	else
		goto end;


	//wallhack
	if (Player && (showCar || Car))
	{
		if (openFeature)
			SetDepthStencilState(DISABLED);

		if (openColor)
			pContext->PSSetShader(psRed, NULL, NULL);

		if (openFeature)
			phookD3D11DrawIndexed(pContext, IndexCount, StartIndexLocation, BaseVertexLocation);

		if (openFeature)
			SetDepthStencilState(ENABLED);

		if (openColor)
			pContext->PSSetShader(psGreen, NULL, NULL);
	}

	//small bruteforce logger
	//ALT + CTRL + M toggles logger
	if (logger)
	{
		if (GetAsyncKeyState(VK_CONTROL) && GetAsyncKeyState('P') & 1) //-
			maxWidth -= 100;
		else if (GetAsyncKeyState(VK_CONTROL) && GetAsyncKeyState('U') & 1) //+
			maxWidth += 100;
		else if (GetAsyncKeyState('P') & 1) //-
			maxWidth -= 10;
		else if (GetAsyncKeyState('U') & 1) //+
			maxWidth += 10;

		if (GetAsyncKeyState('O') & 1) //-
			maxWidth -= 1;
		if (GetAsyncKeyState('I') & 1) //+
			maxWidth += 1;

		if (GetAsyncKeyState(VK_CONTROL) && GetAsyncKeyState('L') & 1) //-
			minWidth -= 100;
		else if (GetAsyncKeyState(VK_CONTROL) && GetAsyncKeyState('H') & 1) //+
			minWidth += 100;
		else if (GetAsyncKeyState('L') & 1) //-
			minWidth -= 10;
		if (GetAsyncKeyState('H') & 1) //+
			minWidth += 10;

		if (GetAsyncKeyState('K') & 1) //-
			minWidth -= 1;
		if (GetAsyncKeyState('J') & 1) //+
			minWidth += 1;
	}

	end:
	return phookD3D11DrawIndexed(pContext, IndexCount, StartIndexLocation, BaseVertexLocation);
}

//==========================================================================================================================

void __stdcall hookD3D11CreateQuery(ID3D11Device* pDevice, const D3D11_QUERY_DESC *pQueryDesc, ID3D11Query **ppQuery)
{
	//Disable Occlusion which prevents rendering player models through certain objects (used by wallhack to see models through walls at all distances, REDUCES FPS)
	if (pQueryDesc->Query == D3D11_QUERY_OCCLUSION)
	{
		D3D11_QUERY_DESC oqueryDesc = CD3D11_QUERY_DESC();
		(&oqueryDesc)->MiscFlags = pQueryDesc->MiscFlags;
		(&oqueryDesc)->Query = D3D11_QUERY_TIMESTAMP;

		return phookD3D11CreateQuery(pDevice, &oqueryDesc, ppQuery);
	}

	return phookD3D11CreateQuery(pDevice, pQueryDesc, ppQuery);
}


//==========================================================================================================================

const int MultisampleCount = 1; // Set to 1 to disable multisampling
LRESULT CALLBACK DXGIMsgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) { return DefWindowProc(hwnd, uMsg, wParam, lParam); }
DWORD __stdcall InitializeHook(LPVOID)
{
	HMODULE hDXGIDLL = 0;
	do
	{
		hDXGIDLL = GetModuleHandle("dxgi.dll");
		if(!hDXGIDLL)
			Sleep(8000);
	} while (!hDXGIDLL);
	Sleep(100);
	IDXGISwapChain* pSwapChain;

	WNDCLASSEXA wc = { sizeof(WNDCLASSEX), CS_CLASSDC, DXGIMsgProc, 0L, 0L, GetModuleHandleA(NULL), NULL, NULL, NULL, NULL, "DX", NULL };
	RegisterClassExA(&wc);
	HWND hWnd = CreateWindowA("DX", NULL, WS_OVERLAPPEDWINDOW, 100, 100, 300, 300, NULL, NULL, wc.hInstance, NULL);

	D3D_FEATURE_LEVEL requestedLevels[] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_1 };
	D3D_FEATURE_LEVEL obtainedLevel;
	ID3D11Device* d3dDevice = nullptr;
	ID3D11DeviceContext* d3dContext = nullptr;

	DXGI_SWAP_CHAIN_DESC scd;
	ZeroMemory(&scd, sizeof(scd));
	scd.BufferCount = 1;
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	scd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	scd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

	scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	scd.OutputWindow = hWnd;
	scd.SampleDesc.Count = MultisampleCount;
	scd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	scd.Windowed = ((GetWindowLongPtr(hWnd, GWL_STYLE) & WS_POPUP) != 0) ? false : true;

	// LibOVR 0.4.3 requires that the width and height for the backbuffer is set even if
	// you use windowed mode, despite being optional according to the D3D11 documentation.
	scd.BufferDesc.Width = 1;
	scd.BufferDesc.Height = 1;
	scd.BufferDesc.RefreshRate.Numerator = 0;
	scd.BufferDesc.RefreshRate.Denominator = 1;

	UINT createFlags = 0;
	IDXGISwapChain* d3dSwapChain = 0;

	if (FAILED(D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		createFlags,
		requestedLevels,
		sizeof(requestedLevels) / sizeof(D3D_FEATURE_LEVEL),
		D3D11_SDK_VERSION,
		&scd,
		&pSwapChain,
		&pDevice,
		&obtainedLevel,
		&pContext)))
	{
		MessageBox(hWnd, "Failed to create directX device and swapchain!", "Error", MB_ICONERROR);
		return NULL;
	}

	pSwapChainVtable = (DWORD_PTR*)pSwapChain;
	pSwapChainVtable = (DWORD_PTR*)pSwapChainVtable[0];

	pContextVTable = (DWORD_PTR*)pContext;
	pContextVTable = (DWORD_PTR*)pContextVTable[0];

	pDeviceVTable = (DWORD_PTR*)pDevice;
	pDeviceVTable = (DWORD_PTR*)pDeviceVTable[0];

	if (MH_Initialize() != MH_OK) { return 1; }
	if (MH_CreateHook((DWORD_PTR*)pSwapChainVtable[8], hookD3D11Present, reinterpret_cast<void**>(&phookD3D11Present)) != MH_OK) { return 1; }
	if (MH_EnableHook((DWORD_PTR*)pSwapChainVtable[8]) != MH_OK) { return 1; }
	if (MH_CreateHook((DWORD_PTR*)pContextVTable[12], hookD3D11DrawIndexed, reinterpret_cast<void**>(&phookD3D11DrawIndexed)) != MH_OK) { return 1; }
	if (MH_EnableHook((DWORD_PTR*)pContextVTable[12]) != MH_OK) { return 1; }
	if (MH_CreateHook((DWORD_PTR*)pDeviceVTable[24], hookD3D11CreateQuery, reinterpret_cast<void**>(&phookD3D11CreateQuery)) != MH_OK) { return 1; }
	if (MH_EnableHook((DWORD_PTR*)pDeviceVTable[24]) != MH_OK) { return 1; }

	DWORD dwOld;
	VirtualProtect(phookD3D11Present, 2, PAGE_EXECUTE_READWRITE, &dwOld);

	while (true) {
		Sleep(10);
	}

	pDevice->Release();
	pContext->Release();
	pSwapChain->Release();

	return NULL;
}

//==========================================================================================================================
BOOL __stdcall DllMain(HINSTANCE hModule, DWORD dwReason, LPVOID lpReserved)
{
#ifdef HeightFeature
	mainModule = GetModuleHandle(NULL);
	wall_addr = (int*)FindMemory(mainModule, (char*)wall_bytes, sizeof(wall_bytes));
	if (wall_addr != NULL) {
		wall_addr = (int*)((int)wall_addr - 0x9);
	}
	speed_addr = (float*)FindMemory(mainModule, (char*)speed_bytes, sizeof(speed_bytes));
	bullet_addr = (int*)(((int)mainModule) + 0x13E0244);
	house_addr = (int*)(((int)mainModule) + 0x61E6D6);
	big_addr = (int*)(((int)mainModule) + 0x167C480);
	dundi_addr = (float*)(((int)mainModule) + 0x167C4A0);

#endif // HeightFeature
	switch (dwReason)
	{
	case DLL_PROCESS_ATTACH: // A process is loading the DLL.
		DisableThreadLibraryCalls(hModule);
		GetModuleFileName(hModule, dlldir, 512);
		for (size_t i = strlen(dlldir); i > 0; i--) { if (dlldir[i] == '\\') { dlldir[i + 1] = 0; break; } }
		CreateThread(NULL, 0, InitializeHook, NULL, 0, NULL);
		break;

	case DLL_PROCESS_DETACH: // A process unloads the DLL.
		if (MH_Uninitialize() != MH_OK) { return 1; }
		if (MH_DisableHook((DWORD_PTR*)pSwapChainVtable[8]) != MH_OK) { return 1; }
		if (MH_DisableHook((DWORD_PTR*)pContextVTable[12]) != MH_OK) { return 1; }
		if (MH_DisableHook((DWORD_PTR*)pDeviceVTable[24]) != MH_OK) { return 1; }
		if (MH_DisableHook((DWORD_PTR*)pContextVTable[39]) != MH_OK) { return 1; }
		if (MH_DisableHook((DWORD_PTR*)pContextVTable[20]) != MH_OK) { return 1; }
		if (MH_DisableHook((DWORD_PTR*)pContextVTable[21]) != MH_OK) { return 1; }
		if (MH_DisableHook((DWORD_PTR*)pContextVTable[40]) != MH_OK) { return 1; }
		if (MH_DisableHook((DWORD_PTR*)pContextVTable[13]) != MH_OK) { return 1; }
		if (MH_DisableHook((DWORD_PTR*)pContextVTable[8]) != MH_OK) { return 1; }
		break;
	}
	return TRUE;
}