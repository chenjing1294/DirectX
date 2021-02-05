#include "d3dUtility.h"

IDirect3DDevice9* device = nullptr;

IDirect3DVertexBuffer9* Triangle = nullptr;	//¶¥µã»º´æ
const int Width = 800;
const int Height = 600;
struct Vertex {
	Vertex() {}
	Vertex(float x, float y, float z) {
		_x = x;
		_y = y;
		_z = z;
	}
	float _x, _y, _z;
	static const DWORD FVF;
};
const DWORD Vertex::FVF = D3DFVF_XYZ;

bool Setup() {
	//´´½¨¶¥µã»º´æºÍË÷Òý»º´æ
	device->CreateVertexBuffer(
		3 * sizeof(Vertex),
		D3DUSAGE_WRITEONLY,
		Vertex::FVF,
		D3DPOOL_MANAGED,
		&Triangle,
		0
	);

	//Ìî³ä»º´æ
	Vertex* vertices;
	Triangle->Lock(
		0,
		0,
		(void**)&vertices,
		0
	);
	vertices[0] = Vertex(-1.0f, 0.0f, 2.0f);
	vertices[1] = Vertex(0.0f, 1.0f, 2.0f);
	vertices[2] = Vertex(1.0f, 0.0f, 2.0f);
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
	device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	return true;
}

void Cleanup() {
	d3d::Release<IDirect3DVertexBuffer9*>(Triangle);
}

bool Display(float timeDelta) {
	if (device) {
		Sleep(50);
		device->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
			0XFFFFFFFF, 1.0f, 0);
		device->BeginScene();
		device->SetStreamSource(0, Triangle, 0, sizeof(Vertex));
		device->SetFVF(Vertex::FVF);
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