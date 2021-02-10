#include "d3dUtility.h"

IDirect3DDevice9* device = nullptr;
const int Width = 800;
const int Height = 600;

ID3DXMesh* Teapot = nullptr;
D3DMATERIAL9 TeapotMtrl;
IDirect3DVertexBuffer9* BkGndQuad = nullptr;
IDirect3DTexture9* BkGndTex = nullptr;
D3DMATERIAL9 BkGndMtrl;

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
	TeapotMtrl = d3d::RED_MTRL;
	TeapotMtrl.Diffuse.a = 0.5f;
	BkGndMtrl = d3d::WHITE_MTRL;
	//create the teapot
	D3DXCreateTeapot(device, &Teapot, 0);
	//create the background quad
	device->CreateVertexBuffer(
		6 * sizeof(Vertex),
		D3DUSAGE_WRITEONLY,
		Vertex::FVF,
		D3DPOOL_MANAGED,
		&BkGndQuad,
		0
	);
	Vertex* v;
	BkGndQuad->Lock(0, 0, (void**)&v, 0);
	v[0] = Vertex(-10.0f, -10.0f, 5.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
	v[1] = Vertex(-10.0f, 10.0f, 5.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f);
	v[2] = Vertex(10.0f, 10.0f, 5.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);

	v[3] = Vertex(-10.0f, -10.0f, 5.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
	v[4] = Vertex(10.0f, 10.0f, 5.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);
	v[5] = Vertex(10.0f, -10.0f, 5.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f);

	BkGndQuad->Unlock();

	//setup a directional light
	D3DLIGHT9 dir;
	ZeroMemory(&dir, sizeof(dir));
	dir.Type = D3DLIGHT_DIRECTIONAL;
	dir.Diffuse = d3d::WHITE;
	dir.Specular = d3d::WHITE * 0.2f;
	dir.Ambient = d3d::WHITE * 0.6f;
	dir.Direction = D3DXVECTOR3(0.707f, 0.0f, 0.707f);
	device->SetLight(0, &dir);
	device->LightEnable(0, true);
	device->SetRenderState(D3DRS_NORMALIZENORMALS, true);
	device->SetRenderState(D3DRS_SPECULARENABLE, true);

	//create texture and set texture filters
	D3DXCreateTextureFromFile(device, TEXT("crate.jpg"), &BkGndTex);
	device->SetTexture(0, BkGndTex);
	device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT);

	//set alpha bliending states
	//use alpha in material's diffuse component for alpha
	device->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE);
	device->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);

	//set blending factors so that alpha component determines transparency
	device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	//set camera
	D3DXVECTOR3 pos(0.0f, 0.0f, -3.0f);
	D3DXVECTOR3 target(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);
	D3DXMATRIX V;
	D3DXMatrixLookAtLH(&V, &pos, &target, &up);
	device->SetTransform(D3DTS_VIEW, &V);

	//set projection matrix
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
	d3d::Release<IDirect3DVertexBuffer9*>(BkGndQuad);
	d3d::Release<ID3DXMesh*>(Teapot);
	d3d::Release<IDirect3DTexture9*>(BkGndTex);
}

bool Display(float timeDelta) {
	if (device) {
		Sleep(50);
		if (GetAsyncKeyState('A') & 0x8000)
			TeapotMtrl.Diffuse.a += 0.1f;
		if (GetAsyncKeyState('S') & 0x8000)
			TeapotMtrl.Diffuse.a -= 0.1f;
		if (TeapotMtrl.Diffuse.a > 1.0f)
			TeapotMtrl.Diffuse.a = 1.0f;
		if (TeapotMtrl.Diffuse.a < 0.0f)
			TeapotMtrl.Diffuse.a = 0.0f;

		device->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xffffffff, 1.0f, 0);
		device->BeginScene();
		D3DXMATRIX W;
		D3DXMatrixIdentity(&W);
		device->SetTransform(D3DTS_WORLD, &W);
		device->SetFVF(Vertex::FVF);
		device->SetStreamSource(0, BkGndQuad, 0, sizeof(Vertex));
		device->SetMaterial(&BkGndMtrl);
		device->SetTexture(0, BkGndTex);
		device->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 2);

		device->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
		D3DXMatrixScaling(&W, 1.5f, 1.5f, 1.5f);
		device->SetMaterial(&TeapotMtrl);
		device->SetTexture(0, 0);
		Teapot->DrawSubset(0);
		device->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
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