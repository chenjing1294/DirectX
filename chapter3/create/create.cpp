#include "d3dUtility.h"

IDirect3DDevice9* device = nullptr;

ID3DXMesh* Objects[5] = { 0 };
D3DXMATRIX ObjWorldMatrices[5];
const int Width = 800;
const int Height = 600;

bool Setup() {
	D3DXCreateTeapot(device, &Objects[0], 0);
	D3DXCreateBox(device, 2.0f, 2.0f, 2.0f, &Objects[1], 0);
	D3DXCreateCylinder(device, 1.0f, 1.0f, 3.0f, 10, 10, &Objects[2], 0);
	D3DXCreateTorus(device, 1.0f, 3.0f, 10, 10, &Objects[3], 0);
	D3DXCreateSphere(device, 1.0f, 10, 10, &Objects[4], 0);
	D3DXMatrixTranslation(&ObjWorldMatrices[0], 0.0f, 0.0f, 0.0f);
	D3DXMatrixTranslation(&ObjWorldMatrices[1], -5.0f, 0.0f, 5.0f);
	D3DXMatrixTranslation(&ObjWorldMatrices[2], 5.0f, 0.0f, 5.0f);
	D3DXMatrixTranslation(&ObjWorldMatrices[3], -5.0f, 0.0f, -5.0f);
	D3DXMatrixTranslation(&ObjWorldMatrices[4], 5.0f, 0.0f, -5.0f);

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
	for (int i = 0; i < 5; i++)
		d3d::Release<ID3DXMesh*>(Objects[i]);
}

bool Display(float timeDelta) {
	if (device) {
		Sleep(50);
		static float angle = (3.0f * D3DX_PI) / 2.0f;
		static float cameraHeight = 0.0f;
		static float cameraHeightDirection = 5.0f;

		D3DXVECTOR3 position(cosf(angle) * 10.f, cameraHeight, sinf(angle) * 10.0f);
		D3DXVECTOR3 target(0.0f, 0.0f, 0.0f);
		D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);
		D3DXMATRIX V;
		D3DXMatrixLookAtLH(&V, &position, &target, &up);
		device->SetTransform(D3DTS_VIEW, &V);
		angle += timeDelta;
		if (angle >= 6.28f)
			angle = 0.0f;
		cameraHeight += cameraHeightDirection * timeDelta;
		if (cameraHeight >= 10.0f)
			cameraHeight = -5.0f;
		if (cameraHeight <= -10.f)
			cameraHeight = 5.0f;

		device->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
			0XFFFFFFFF, 1.0f, 0);
		device->BeginScene();
		for (int i = 0; i < 5; i++) {
			device->SetTransform(D3DTS_WORLD, &ObjWorldMatrices[i]);
			Objects[i]->DrawSubset(0);
		}
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