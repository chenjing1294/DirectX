#pragma once
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")
#pragma comment(lib, "winmm.lib")
#include <d3d9.h>
#include <d3dx9math.h>

namespace d3d {
	bool InitD3D(
		HINSTANCE hInstance,		//�����ʵ�����
		int width, int height,		//��̨�����б���Ŀ��
		bool windowed,				//ȫ�����Ǵ���
		D3DDEVTYPE deviceType,		//ʹ��Ӳ���豸��������豸
		IDirect3DDevice9** device	//���ܴ������豸
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
}