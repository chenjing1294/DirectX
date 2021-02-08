#include "d3dUtility.h"

IDirect3DDevice9* device = nullptr;
IDirect3DVertexBuffer9* Pyramid = nullptr;	//¶¥µã»º´æ
const int Width = 800;
const int Height = 600;
struct Vertex {
	float _x, _y, _z;
	float _nx, _ny, _nz;
	static const DWORD FVF;
	Vertex(float x, float y, float z, float nx, float ny, float nz) :_x(x), _y(y), _z(z), _nx(nx), _ny(ny), _nz(nz) {}
};
const DWORD Vertex::FVF = D3DFVF_XYZ | D3DFVF_NORMAL;

bool Setup() {
	device->SetRenderState(D3DRS_LIGHTING, true);
	device->CreateVertexBuffer(
		12 * sizeof(Vertex),
		D3DUSAGE_WRITEONLY,
		Vertex::FVF,
		D3DPOOL_MANAGED,
		&Pyramid,
		0
	);
	//fill the vertex buffer with pyramid data
	Vertex* v;
	Pyramid->Lock(0, 0, (void**)&v, 0);
	//front face
	v[0] = Vertex(-1.0f, 0.0f, -1.0f, 0.0f, 0.707f, -0.707f);
	v[1] = Vertex(0.0f, 1.0f, 0.0f, 0.0f, 0.707f, -0.707f);
	v[2] = Vertex(1.0f, 0.0f, -1.0f, 0.0f, 0.707f, -0.707f);

	//left face
	v[3] = Vertex(-1.0f, 0.0f, 1.0f, -0.707f, 0.707f, 0.0f);
	v[4] = Vertex(0.0f, 1.0f, 0.0f, -0.707f, 0.707f, 0.0f);
	v[5] = Vertex(-1.0f, 0.0f, -1.0f, -0.707f, 0.707f, 0.0f);

	//right face
	v[6] = Vertex(1.0f, 0.0f, -1.0f, 0.707f, 0.707f, 0.0f);
	v[7] = Vertex(0.0f, 1.0f, 0.0f, 0.707f, 0.707f, 0.0f);
	v[8] = Vertex(1.0f, 0.0f, 1.0f, 0.707f, 0.707f, 0.0f);

	//back face
	v[9] = Vertex(1.0f, 0.0f, 1.0f, 0.0f, 0.707f, 0.707f);
	v[10] = Vertex(0.0f, 1.0f, 0.0f, 0.0f, 0.707f, 0.707f);
	v[11] = Vertex(-1.0f, 0.0f, 1.0f, 0.0f, 0.707f, 0.707f);
	Pyramid->Unlock();

	//create and set the meterial
	D3DMATERIAL9 mtrl;
	mtrl.Ambient = d3d::WHITE;
	mtrl.Diffuse = d3d::WHITE;
	mtrl.Specular = d3d::WHITE;
	mtrl.Emissive = d3d::BLACK;
	mtrl.Power = 5.0f;
	device->SetMaterial(&mtrl);

	//setup a directional light
	D3DLIGHT9 dir;
	ZeroMemory(&dir, sizeof(dir));
	dir.Type = D3DLIGHT_DIRECTIONAL;
	dir.Diffuse = d3d::WHITE;
	dir.Specular = d3d::WHITE * 0.3f;
	dir.Ambient = d3d::WHITE * 0.6f;
	dir.Direction = D3DXVECTOR3(1.0f, 0.0f, 0.0f);
	device->SetLight(0, &dir);
	device->LightEnable(0, true);

	//turn on specular lighting and instruct Direct3D
	//to renormalize normals.
	device->SetRenderState(D3DRS_NORMALIZENORMALS, true);
	device->SetRenderState(D3DRS_SPECULARENABLE, true);

	//position and aim the camera
	D3DXVECTOR3 pos(0.0f, 1.0f, -3.0f);
	D3DXVECTOR3 target(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);
	D3DXMATRIX V;
	D3DXMatrixLookAtLH(&V, &pos, &target, &up);
	device->SetTransform(D3DTS_VIEW, &V);

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
	d3d::Release<IDirect3DVertexBuffer9*>(Pyramid);
}

bool Display(float timeDelta) {
	if (device) {
		Sleep(50);
		D3DXMATRIX yRot;
		static float y = 0.0f;
		D3DXMatrixRotationY(&yRot, y);
		y += timeDelta;
		if (y >= 6.28f)
			y = 0.0f;
		device->SetTransform(D3DTS_WORLD, &yRot);
		device->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
			0x00000000, 1.0f, 0);
		device->BeginScene();
		device->SetStreamSource(0, Pyramid, 0, sizeof(Vertex));
		device->SetFVF(Vertex::FVF);
		device->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 4);
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