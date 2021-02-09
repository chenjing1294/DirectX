#include "d3dUtility.h"
#include "cube.h"
#include "vertex.h"

IDirect3DDevice9* device = nullptr;
const int Width = 800;
const int Height = 600;
Cube* Box = nullptr;
IDirect3DTexture9* Tex = nullptr;

bool Setup() {
	Box = new Cube(device);
	D3DLIGHT9 light;
	ZeroMemory(&light, sizeof(light));
	light.Type = D3DLIGHT_DIRECTIONAL;
	light.Ambient = D3DXCOLOR(0.8f, 0.8f, 0.8f, 1.0f);
	light.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	light.Specular = D3DXCOLOR(0.2f, 0.2f, 0.2f, 1.0f);
	light.Direction = D3DXVECTOR3(1.0f, -1.0f, 0.0f);
	device->SetLight(0, &light);
	device->LightEnable(0, true);
	device->SetRenderState(D3DRS_NORMALIZENORMALS, true);
	device->SetRenderState(D3DRS_SPECULARENABLE, true);

	D3DXCreateTextureFromFile(
		device,
		TEXT("crate.jpg"),
		&Tex
	);

	device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

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
	d3d::Delete<Cube*>(Box);
	d3d::Release<IDirect3DTexture9*>(Tex);
}

bool Display(float timeDelta) {
	if (device) {
		Sleep(50);
		static float angle = (3.0f * D3DX_PI) / 2.0f;
		static float height = 2.0f;
		if (GetAsyncKeyState(VK_LEFT) & 0x8000)
			angle -= 0.5f * timeDelta;
		if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
			angle += 0.5f * timeDelta;
		if (GetAsyncKeyState(VK_UP) & 0x8000)
			height += 5.0f * timeDelta;
		if (GetAsyncKeyState(VK_DOWN) & 0x8000)
			height -= 5.0f * timeDelta;

		D3DXVECTOR3 position(cosf(angle) * 3.0f, height, sinf(angle) * 3.0f);
		D3DXVECTOR3 target(0.0f, 0.0f, 0.0f);
		D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);
		D3DXMATRIX V;
		D3DXMatrixLookAtLH(&V, &position, &target, &up);
		device->SetTransform(D3DTS_VIEW, &V);

		device->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
			0xffffffff, 1.0f, 0);
		device->BeginScene();
		device->SetMaterial(&d3d::WHITE_MTRL);
		device->SetTexture(0, Tex);
		Box->draw(0, 0, 0);
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