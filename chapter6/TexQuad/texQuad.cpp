#include "d3dUtility.h"

IDirect3DDevice9* device = nullptr;
const int Width = 800;
const int Height = 600;
IDirect3DVertexBuffer9* Quad = nullptr;
IDirect3DTexture9* Tex = nullptr;

struct Vertex
{
	float _x, _y, _z;
	float _nx, _ny, _nz;
	float _u, _v;
	static const DWORD FVF;
	Vertex(float x, float y, float z, float nx, float ny, float nz, float u, float v) {
		_x = x; _y = y; _z = z;
		_nx = nx; _ny = ny; _nz = nz;
		_u = u; _v = v;
	}
};
const DWORD Vertex::FVF = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1;
bool Setup() {
	device->CreateVertexBuffer(
		6 * sizeof(Vertex),
		D3DUSAGE_WRITEONLY,
		Vertex::FVF,
		D3DPOOL_MANAGED,
		&Quad,
		0
	);
	Vertex* v;
	Quad->Lock(0, 0, (void**)&v, 0);
	v[0] = Vertex(-1.0f, -1.0f, 1.25f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
	v[1] = Vertex(-1.0f, 1.0f, 1.25f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f);
	v[2] = Vertex(1.0f, 1.0f, 1.25f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);

	v[3] = Vertex(-1.0f, -1.0f, 1.25f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
	v[4] = Vertex(1.0f, 1.0f, 1.25f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);
	v[5] = Vertex(1.0f, -1.0f, 1.25f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f);

	Quad->Unlock();

	D3DXCreateTextureFromFile(device, TEXT("dx5_logo.bmp"), &Tex);
	device->SetTexture(0, Tex);
	device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT);

	device->SetRenderState(D3DRS_LIGHTING, false);
	D3DXMATRIX proj;
	D3DXMatrixPerspectiveFovLH(
		&proj,
		D3DX_PI * 0.5f,
		(float)Width / (float)Height,
		1.0f,
		1000.0f
	);
	device->SetTransform(D3DTS_PROJECTION, &proj);
	return true;
}

void Cleanup() {
	d3d::Release<IDirect3DVertexBuffer9*>(Quad);
	d3d::Release<IDirect3DTexture9*>(Tex);
}

bool Display(float timeDelta) {
	if (device) {
		Sleep(50);
		device->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
			0xffffffff, 1.0f, 0);
		device->BeginScene();
		device->SetStreamSource(0, Quad, 0, sizeof(Vertex));
		device->SetFVF(Vertex::FVF);
		device->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 2);
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