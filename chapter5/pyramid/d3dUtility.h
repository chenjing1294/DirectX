#pragma once
#pragma once
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")
#pragma comment(lib, "winmm.lib")
#include <d3d9.h>
#include <d3dx9math.h>

namespace d3d {
	bool InitD3D(
		HINSTANCE hInstance,		//程序的实例句柄
		int width, int height,		//后台缓存中表面的宽高
		bool windowed,				//全屏还是窗口
		D3DDEVTYPE deviceType,		//使用硬件设备还是软件设备
		IDirect3DDevice9** device	//接受创建的设备
	);

	int EnterMsgLoop(bool (*ptr_display)(float timeDelta));

	LRESULT CALLBACK WndProc(
		HWND hwnd,
		UINT msg,
		WPARAM wParam,
		LPARAM lParam
	);

	template<typename T> void Release(T t) {
		if (t) {
			t->Release();
			t = nullptr;
		}
	}

	template<typename T> void Delete(T t) {
		if (t) {
			delete t;
			t = nullptr;
		}
	}

	const D3DXCOLOR WHITE(D3DCOLOR_XRGB(255, 255, 255));
	const D3DXCOLOR BLACK(D3DCOLOR_XRGB(0, 0, 0));
	const D3DXCOLOR RED(D3DCOLOR_XRGB(255, 0, 0));
	const D3DXCOLOR GREEN(D3DCOLOR_XRGB(0, 255, 0));
	const D3DXCOLOR BLUE(D3DCOLOR_XRGB(0, 0, 255));
	const D3DXCOLOR YELLOW(D3DCOLOR_XRGB(255, 255, 0));
	const D3DXCOLOR CYAN(D3DCOLOR_XRGB(0, 255, 255));
	const D3DXCOLOR MAGENTA(D3DCOLOR_XRGB(255, 0, 255));
}