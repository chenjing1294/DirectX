#include "d3dUtility.h"

D3DXMATRIX World;
IDirect3DDevice9* device = nullptr;
IDirect3DVertexBuffer9* Triangle = nullptr;	//¶¥µã»º´æ
const int Width = 800;
const int Height = 600;
struct ColorVertex {
	float _x, _y, _z;
	D3DCOLOR _color;
	ColorVertex(float x, float y, float z, D3DCOLOR c) :_x(x), _y(y), _z(z), _color(c) {}
	static const DWORD FVF;
};
const DWORD ColorVertex::FVF = D3DFVF_XYZ | D3DFVF_DIFFUSE;

bool Setup() {
	//´´½¨¶¥µã»º´æºÍË÷Òý»º´æ
	device->CreateVertexBuffer(
		3 * sizeof(ColorVertex),
		D3DUSAGE_WRITEONLY,
		ColorVertex::FVF,
		D3DPOOL_MANAGED,
		&Triangle,
		0
	);

	//Ìî³ä»º´æ
	ColorVertex* v;
	Triangle->Lock(
		0,
		0,
		(void**)&v,
		0
	);
	v[0] = ColorVertex(-1.0f, 0.0f, 2.0f, D3DCOLOR_XRGB(255,0,0));
	v[1] = ColorVertex(0.0f, 1.0f, 2.0f, D3DCOLOR_XRGB(0, 255, 0));
	v[2] = ColorVertex(1.0f, 0.0f, 2.0f, D3DCOLOR_XRGB(0, 0, 255));
	Triangle->Unlock();

	D3DXMATRIX proj;
	D3DXMatrixPerspectiveFovLH(
		&proj,
		D3DX_PI * 0.5f,
		(float)Width / (float)Height,
		1.0f,
		1000.0f
	);
	device->SetTransform(D3DTS_PROJECTION, &proj);
	//turn off lighting
	device->SetRenderState(D3DRS_LIGHTING, false);
	return true;
}

void Cleanup() {
	d3d::Release<IDirect3DVertexBuffer9*>(Triangle);
}

bool Display(float timeDelta) {
	if (device) {
		Sleep(50);
		device->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
			0xFFFFFFFF, 1.0f, 0);
		device->BeginScene();
		device->SetFVF(ColorVertex::FVF);
		device->SetStreamSource(0, Triangle, 0, sizeof(ColorVertex));
		//draw the triangle to the left with flat shading
		D3DXMatrixTranslation(&World, -1.25f, 0.0f, 0.0f);
		device->SetTransform(D3DTS_WORLD, &World);
		device->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_FLAT);
		device->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 1);

		//draw the triangle to the right with gouraud shading
		D3DXMatrixTranslation(&World, 1.25f, 0.0f, 0.0f);
		device->SetTransform(D3DTS_WORLD, &World);
		device->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_GOURAUD);
		device->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 1);
		device->EndScene();
		device->Present(0, 0, 0, 0);
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