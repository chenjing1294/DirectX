#include "d3dUtility.h"

IDirect3DDevice9* device = nullptr;

bool Setup() {
	return true;
}

void Cleanup() {
}

bool Display(float timeDelta) {
	if (device) {
		Sleep(50);
		device->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
			0x00000000, 1.0f, 0);
		device->Present(0, 0, 0, 0);	//present backbuffer
	}
	return true;
}


LRESULT CALLBACK d3d::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE) {
			DestroyWindow(hwnd);
			break;
		}
	}
	return DefWindowProc(hwnd, msg, wParam, lParam);
}

int WINAPI	WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmsLine, int showCmd) {
	if (!d3d::InitD3D(hInstance, 800, 600, true, D3DDEVTYPE_HAL, &device)) {
		MessageBox(0, TEXT("InitD3D() - FAILED"), 0, 0);
		return 0;
	}
	if (!Setup()) {
		MessageBox(0, TEXT("Setup() - FAILED"), 0, 0);
		return 0;
	}
	d3d::EnterMsgLoop(Display);
	Cleanup();
	device->Release();
	return 0;
}