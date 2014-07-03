// Example low level rendering Unity plugin
#include "RenderingPlugin.h"
#include "Interface.h"

#include <math.h>
#include <stdio.h>
#include "vx_driver.h"

#include "CommonVar.h"

using namespace std;
// --------------------------------------------------------------------------
// Include headers for the graphics APIs we support

#if SUPPORT_D3D9
	#include <d3d9.h>
#endif
#if SUPPORT_D3D11
	#include <d3d11.h>
#endif
#if SUPPORT_OPENGL
	#if UNITY_WIN
		#include <gl/GL.h>
	#else
		#include <OpenGL/OpenGL.h>
	#endif
#endif

// --------------------------------------------------------------------------
// Helper utilities


// Prints a string
static void DebugLog (const char* str)
{
	char buf[512];

	sprintf(buf,"vx :: %s",str);

	#if UNITY_WIN
	OutputDebugStringA (buf);
	#else
	printf ("%s", buf);
	#endif
}

// COM-like Release macro
#ifndef SAFE_RELEASE
#define SAFE_RELEASE(a) if (a) { a->Release(); a = NULL; }
#endif


//----------------------------------------------------
//				System Area Function Layer
//----------------------------------------------------	

// UnitySetGraphicsDevice

static int g_DeviceType = -1;


// Actual setup/teardown functions defined below
#if SUPPORT_D3D9
static void SetGraphicsDeviceD3D9 (IDirect3DDevice9* device, GfxDeviceEventType eventType);
#endif
#if SUPPORT_D3D11
static void SetGraphicsDeviceD3D11 (ID3D11Device* device, GfxDeviceEventType eventType);
#endif

vector<vx_mon_state*> mon_lst;
vx_state *vx;

// 시작될떄
void EventCreate(void)
{
	int i,max;
	vx = vx_init();
	max = vx_get_devices(vx);
	for(i=0;i<max;i++)
	{
		vx_mon_state *vmon = vx_get_desc(vx,i);

		if(vmon)	//가상 모니터임
			mon_lst.push_back(vmon);
	}
	
	char buf[256];
	sprintf(buf,"vx inited // the number of detected virtual monitor %d\n",mon_lst.size());
	DebugLog(buf);
}

// 끝날떄
void EventShutdown(void)
{
	g_inf_RenderList.clear();
	vx_release(vx);
	DebugLog("vx released\n");
}

extern "C" void EXPORT_API UnitySetGraphicsDevice (void* device, int deviceType, int eventType)
{
	// Set device type to -1, i.e. "not recognized by our plugin"
	g_DeviceType = -1;
	

	/* Common Event Handler  */
	if (eventType == kGfxDeviceEventInitialize)
		EventCreate();
	if (eventType == kGfxDeviceEventShutdown)
		EventShutdown();

	#if SUPPORT_D3D9
	// D3D9 device, remember device pointer and device type.
	// The pointer we get is IDirect3DDevice9.
	if (deviceType == kGfxRendererD3D9)
	{
		DebugLog ("Set D3D9 graphics device\n");
		g_DeviceType = deviceType;
		SetGraphicsDeviceD3D9 ((IDirect3DDevice9*)device, (GfxDeviceEventType)eventType);
	}
	#endif

	#if SUPPORT_D3D11
	// D3D11 device, remember device pointer and device type.
	// The pointer we get is ID3D11Device.
	if (deviceType == kGfxRendererD3D11)
	{
		DebugLog ("Set D3D11 graphics device\n");
		g_DeviceType = deviceType;
		SetGraphicsDeviceD3D11 ((ID3D11Device*)device, (GfxDeviceEventType)eventType);
	}
	#endif

	#if SUPPORT_OPENGL
	// If we've got an OpenGL device, remember device type. There's no OpenGL
	// "device pointer" to remember since OpenGL always operates on a currently set
	// global context.
	if (deviceType == kGfxRendererOpenGL)
	{
		DebugLog ("Set OpenGL graphics device\n");
		g_DeviceType = deviceType;
	}
	#endif
}



// --------------------------------------------------------------------------
// UnityRenderEvent
// This will be called for GL.IssuePluginEvent script calls; eventID will
// be the integer passed to IssuePluginEvent. In this example, we just ignore
// that value.

static void DoRendering (int mon,void *TexturePointer);


extern "C" void EXPORT_API UnityRenderEvent (int eventID)
{
	int updateCnt=0;
	char str[256];

	// Unknown graphics device type? Do nothing.
	if (g_DeviceType == -1)
		return;
	
	MON_UPDATE_PARAM::iterator it;
	for(it = g_inf_RenderList.begin();it != g_inf_RenderList.end();++it)
	{
		vx_mon_state *vmon = NULL;
		int targ_mon = (*it).targMon;
		
		if(mon_lst.size() < targ_mon)
			continue;

		vmon = mon_lst[targ_mon];
		if((*it).token != vx_mon_tokenGet(vmon))	//해당 모니터가 업데이트 되었나 ?
		{
			DoRendering (targ_mon,(*it).texPtr);
			(*it).token = vx_mon_tokenGet(vmon);
			updateCnt++;
		}
	}

#if _DEBUG
	if(updateCnt)
	{
		sprintf(str,"UpdateCnt %d",updateCnt);
		DebugLog(str);
	}
#endif
}

// -------------------------------------------------------------------
//  Direct3D 9 setup/teardown code


#if SUPPORT_D3D9

static IDirect3DDevice9* g_D3D9Device;

// A dynamic vertex buffer just to demonstrate how to handle D3D9 device resets.
static IDirect3DVertexBuffer9* g_D3D9DynamicVB;

static void SetGraphicsDeviceD3D9 (IDirect3DDevice9* device, GfxDeviceEventType eventType)
{
	g_D3D9Device = device;

	// Create or release a small dynamic vertex buffer depending on the event type.
	switch (eventType) {
	case kGfxDeviceEventInitialize:
	case kGfxDeviceEventAfterReset:
		// After device is initialized or was just reset, create the VB.
		if (!g_D3D9DynamicVB)
			g_D3D9Device->CreateVertexBuffer (1024, D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, 0, D3DPOOL_DEFAULT, &g_D3D9DynamicVB, NULL);
		break;
	case kGfxDeviceEventBeforeReset:
	case kGfxDeviceEventShutdown:
		// Before device is reset or being shut down, release the VB.
		SAFE_RELEASE(g_D3D9DynamicVB);
		break;
	}
}

#endif // #if SUPPORT_D3D9



// -------------------------------------------------------------------
//  Direct3D 11 setup/teardown code


#if SUPPORT_D3D11

static ID3D11Device* g_D3D11Device;
static ID3D11Buffer* g_D3D11VB; // vertex buffer
static ID3D11Buffer* g_D3D11CB; // constant buffer
static ID3D11VertexShader* g_D3D11VertexShader;
static ID3D11PixelShader* g_D3D11PixelShader;
static ID3D11InputLayout* g_D3D11InputLayout;
static ID3D11RasterizerState* g_D3D11RasterState;
static ID3D11BlendState* g_D3D11BlendState;
static ID3D11DepthStencilState* g_D3D11DepthState;

typedef HRESULT (WINAPI *D3DCompileFunc)(
	const void* pSrcData,
	unsigned long SrcDataSize,
	const char* pFileName,
	const D3D10_SHADER_MACRO* pDefines,
	ID3D10Include* pInclude,
	const char* pEntrypoint,
	const char* pTarget,
	unsigned int Flags1,
	unsigned int Flags2,
	ID3D10Blob** ppCode,
	ID3D10Blob** ppErrorMsgs);

static const char* kD3D11ShaderText =
"cbuffer MyCB : register(b0) {\n"
"	float4x4 worldMatrix;\n"
"}\n"
"void VS (float3 pos : POSITION, float4 color : COLOR, out float4 ocolor : COLOR, out float4 opos : SV_Position) {\n"
"	opos = mul (worldMatrix, float4(pos,1));\n"
"	ocolor = color;\n"
"}\n"
"float4 PS (float4 color : COLOR) : SV_TARGET {\n"
"	return color;\n"
"}\n";


static void CreateD3D11Resources()
{
	D3D11_BUFFER_DESC desc;
	memset (&desc, 0, sizeof(desc));

	// vertex buffer
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.ByteWidth = 1024;
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	g_D3D11Device->CreateBuffer (&desc, NULL, &g_D3D11VB);

	// constant buffer
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.ByteWidth = 64; // hold 1 matrix
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc.CPUAccessFlags = 0;
	g_D3D11Device->CreateBuffer (&desc, NULL, &g_D3D11CB);

	// shaders
	HMODULE compiler = LoadLibraryA("D3DCompiler_43.dll");

	if (compiler == NULL)
	{
		// Try compiler from Windows 8 SDK
		compiler = LoadLibraryA("D3DCompiler_46.dll");
	}
	if (compiler)
	{
		ID3D10Blob* vsBlob = NULL;
		ID3D10Blob* psBlob = NULL;

		D3DCompileFunc compileFunc = (D3DCompileFunc)GetProcAddress (compiler, "D3DCompile");
		if (compileFunc)
		{
			HRESULT hr;
			hr = compileFunc(kD3D11ShaderText, strlen(kD3D11ShaderText), NULL, NULL, NULL, "VS", "vs_4_0", 0, 0, &vsBlob, NULL);
			if (SUCCEEDED(hr))
			{
				g_D3D11Device->CreateVertexShader (vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), NULL, &g_D3D11VertexShader);
			}

			hr = compileFunc(kD3D11ShaderText, strlen(kD3D11ShaderText), NULL, NULL, NULL, "PS", "ps_4_0", 0, 0, &psBlob, NULL);
			if (SUCCEEDED(hr))
			{
				g_D3D11Device->CreatePixelShader (psBlob->GetBufferPointer(), psBlob->GetBufferSize(), NULL, &g_D3D11PixelShader);
			}
		}

		// input layout
		if (g_D3D11VertexShader && vsBlob)
		{
			D3D11_INPUT_ELEMENT_DESC layout[] = {
				{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "COLOR", 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			};

			g_D3D11Device->CreateInputLayout (layout, 2, vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &g_D3D11InputLayout);
		}

		SAFE_RELEASE(vsBlob);
		SAFE_RELEASE(psBlob);

		FreeLibrary (compiler);
	}
	else
	{
		DebugLog ("D3D11: HLSL shader compiler not found, will not render anything\n");
	}

	// render states
	D3D11_RASTERIZER_DESC rsdesc;
	memset (&rsdesc, 0, sizeof(rsdesc));
	rsdesc.FillMode = D3D11_FILL_SOLID;
	rsdesc.CullMode = D3D11_CULL_NONE;
	rsdesc.DepthClipEnable = TRUE;
	g_D3D11Device->CreateRasterizerState (&rsdesc, &g_D3D11RasterState);

	D3D11_DEPTH_STENCIL_DESC dsdesc;
	memset (&dsdesc, 0, sizeof(dsdesc));
	dsdesc.DepthEnable = TRUE;
	dsdesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	dsdesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	g_D3D11Device->CreateDepthStencilState (&dsdesc, &g_D3D11DepthState);

	D3D11_BLEND_DESC bdesc;
	memset (&bdesc, 0, sizeof(bdesc));
	bdesc.RenderTarget[0].BlendEnable = FALSE;
	bdesc.RenderTarget[0].RenderTargetWriteMask = 0xF;
	g_D3D11Device->CreateBlendState (&bdesc, &g_D3D11BlendState);
}

static void ReleaseD3D11Resources()
{
	SAFE_RELEASE(g_D3D11VB);
	SAFE_RELEASE(g_D3D11CB);
	SAFE_RELEASE(g_D3D11VertexShader);
	SAFE_RELEASE(g_D3D11PixelShader);
	SAFE_RELEASE(g_D3D11InputLayout);
	SAFE_RELEASE(g_D3D11RasterState);
	SAFE_RELEASE(g_D3D11BlendState);
	SAFE_RELEASE(g_D3D11DepthState);
}

static void SetGraphicsDeviceD3D11 (ID3D11Device* device, GfxDeviceEventType eventType)
{
	g_D3D11Device = device;

	if (eventType == kGfxDeviceEventInitialize)
		CreateD3D11Resources();
	if (eventType == kGfxDeviceEventShutdown)
		ReleaseD3D11Resources();
}

#endif // #if SUPPORT_D3D11

/*

BOOL vx_mon_isUpdate(vx_mon_state *mon);	//해당 모니터가 업데이트 되었나 ?
void vx_mon_tokenUpdate(vx_mon_state *mon);
PCHAR vx_mon_getFramebuffer(vx_mon_state *mon);	//framebuffer의 포인터를 얻어옴
POINT vx_mon_getResolution(vx_mon_state *mon);	//해상도 정보를 얻어옴
vx_mon_state *vx_get_desc(vx_state *vs,int idx);		//해당 드라이버의 디스크립터를 얻어옴
*/

static void FillTextureFromCode (int targ_mon,int width, int height, int stride, unsigned char* dst)
{
	int vv;

	vx_mon_state *vmon = NULL;
	if(mon_lst.size() > targ_mon)
		vmon = mon_lst[targ_mon];

	if(vmon)
	{
		char *ptr = vx_mon_getFramebuffer(vmon);
		if(ptr)
			memcpy(dst,ptr,width*height*4);
		else
		{
			memset(dst,0xff0000ff,width*height*4);
			DebugLog("vx_mon_getFramebuffer failed\n");
		}
	}
	else
	{
		memset(dst,0x00ff00ff,width*height*4);
		DebugLog("vx_get_desc(vx,0) failed\n");
	}
}

static void DoRendering (int mon,void *TexturePointer)
{
	#if SUPPORT_D3D9
	// D3D9 case
	if (g_DeviceType == kGfxRendererD3D9)
	{
		// Update native texture from code
		if (TexturePointer)
		{
			IDirect3DTexture9* d3dtex = (IDirect3DTexture9*)TexturePointer;
			D3DSURFACE_DESC desc;
			d3dtex->GetLevelDesc (0, &desc);
			D3DLOCKED_RECT lr;
			d3dtex->LockRect (0, &lr, NULL, 0);
			FillTextureFromCode (mon,desc.Width, desc.Height, lr.Pitch, (unsigned char*)lr.pBits);
			d3dtex->UnlockRect (0);
		}
	}
	#endif


	#if SUPPORT_D3D11
	// D3D11 case
	if (g_DeviceType == kGfxRendererD3D11 && g_D3D11VertexShader)
	{
		ID3D11DeviceContext* ctx = NULL;
		g_D3D11Device->GetImmediateContext (&ctx);

		// update native texture from code
		if (TexturePointer)
		{
			ID3D11Texture2D* d3dtex = (ID3D11Texture2D*)TexturePointer;
			D3D11_TEXTURE2D_DESC desc;
			d3dtex->GetDesc (&desc);

			unsigned char* data = new unsigned char[desc.Width*desc.Height*4];
			FillTextureFromCode (mon,desc.Width, desc.Height, desc.Width*4, data);
			ctx->UpdateSubresource (d3dtex, 0, NULL, data, desc.Width*4, 0);
			delete[] data;
		}

		ctx->Release();
	}
	#endif


	#if SUPPORT_OPENGL
	// OpenGL case
	if (g_DeviceType == kGfxRendererOpenGL)
	{
		// update native texture from code
		if (TexturePointer)
		{
			GLuint gltex = (GLuint)(size_t)(TexturePointer);
			glBindTexture (GL_TEXTURE_2D, gltex);
			int texWidth, texHeight;
			glGetTexLevelParameteriv (GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &texWidth);
			glGetTexLevelParameteriv (GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &texHeight);

			unsigned char* data = new unsigned char[texWidth*texHeight*4];
			FillTextureFromCode (mon,texWidth, texHeight, texHeight*4, data);
			glTexSubImage2D (GL_TEXTURE_2D, 0, 0, 0, texWidth, texHeight, GL_RGBA, GL_UNSIGNED_BYTE, data);
			delete[] data;
		}
	}
	#endif
}
