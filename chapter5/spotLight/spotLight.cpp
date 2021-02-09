#include "d3dUtility.h"

IDirect3DDevice9* device = nullptr;
const int Width = 800;
const int Height = 600;
ID3DXMesh* Objects[4] = { 0 };
D3DXMATRIX Worlds[4];
D3DMATERIAL9 Mtrls[4];
D3DLIGHT9 spotLight;

bool Setup() {
	//create objects
	D3DXCreateTeapot(device, &Objects[0], 0);
	D3DXCreateSphere(device, 1.0f, 20, 20, &Objects[1], 0);
	D3DXCreateTorus(device, 0.5f, 1.0f, 20, 20, &Objects[2], 0);
	D3DXCreateCylinder(device, 0.5f, 0.5f, 2.0f, 20, 20, &Objects[3], 0);

	//build world matrices - position the objects in world space
	D3DXMatrixTranslation(&Worlds[0], 0.0f, 2.0f, 0.0f);
	D3DXMatrixTranslation(&Worlds[1], 0.0f, -2.0f, 0.0f);
	D3DXMatrixTranslation(&Worlds[2], -3.0f, 0.0f, 0.0f);
	D3DXMatrixTranslation(&Worlds[3], 3.0f, 2.0f, 0.0f);

	//setup the object's materials
	Mtrls[0] = d3d::RED_MTRL;
	Mtrls[1] = d3d::BLUE_MTRL;
	Mtrls[2] = d3d::GREEN_MTRL;
	Mtrls[3] = d3d::YELLOW_MTRL;
	for (int i = 0; i < 4; i++)
		Mtrls[i].Power = 20.0f;

	//setup a point light
	D3DXVECTOR3 pos(0.0f, 0.0f, -5.0f);
	D3DXVECTOR3 dir(0.0f, 0.0f, 1.0f);
	D3DXCOLOR c = d3d::WHITE;
	spotLight = d3d::InitSpotLight(&pos, &dir, &c);

	//set lighting related render states.
	device->SetRenderState(D3DRS_NORMALIZENORMALS, true);
	device->SetRenderState(D3DRS_SPECULARENABLE, true);

	//position and aim the camera
	D3DXVECTOR3 position(0.0f, 0.0f, -5.0f);
	D3DXVECTOR3 target(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);
	D3DXMATRIX V;
	D3DXMatrixLookAtLH(&V, &position, &target, &up);
	device->SetTransform(D3DTS_VIEW, &V);

	D3DXMATRIX proj;
	D3DXMatrixPerspectiveFovLH(
		&proj,
		D3DX_PI * 0.25f,
		(float)Width / (float)Height,
		1.0f,
		1000.0f
	);
	device->SetTransform(D3DTS_PROJECTION, &proj);
	return true;
}

void Cleanup() {
	for (int i = 0; i < 4; i++)
		d3d::Release<ID3DXMesh*>(Objects[i]);
}

bool Display(float timeDelta) {
	if (device) {
		Sleep(50);
		static float angle = (3.0f * D3DX_PI) / 2.0f;
		if (GetAsyncKeyState(VK_LEFT) & 0x8000)
			spotLight.Direction.x -= 0.5f * timeDelta;
		if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
			spotLight.Direction.x += 0.5f * timeDelta;
		if(GetAsyncKeyState(VK_UP) & 0x8000)
			spotLight.Direction.y += 5.0f * timeDelta;
		if (GetAsyncKeyState(VK_DOWN) & 0x8000)
			spotLight.Direction.y -= 5.0f * timeDelta;

		//setup and enable the light
		device->SetLight(0, &spotLight);
		device->LightEnable(0, true);

		device->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x00000000, 1.0f, 0);
		device->BeginScene();
		for (int i = 0; i < 4; i++) {
			device->SetMaterial(&Mtrls[i]);
			device->SetTransform(D3DTS_WORLD, &Worlds[i]);
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