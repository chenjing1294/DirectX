#include "d3dUtility.h"

IDirect3DDevice9* device = nullptr;

IDirect3DVertexBuffer9* VB = nullptr;	//顶点缓存
IDirect3DIndexBuffer9* IB = nullptr;	//索引缓存
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
	//创建顶点缓存和索引缓存
	device->CreateVertexBuffer(
		8 * sizeof(Vertex),
		D3DUSAGE_WRITEONLY,
		Vertex::FVF,
		D3DPOOL_MANAGED,
		&VB,
		0
	);
	device->CreateIndexBuffer(
		36 * sizeof(WORD),
		D3DUSAGE_WRITEONLY,
		D3DFMT_INDEX16,
		D3DPOOL_MANAGED,
		&IB,
		0
	);

	//填充缓存
	Vertex* vertices;
	VB->Lock(
		0,
		0,
		(void**)&vertices,
		0
	);
	vertices[0] = Vertex(-1.0f, -1.0f, -1.0f);
	vertices[1] = Vertex(-1.0f, 1.0f, -1.0f);
	vertices[2] = Vertex(1.0f, 1.0f, -1.0f);
	vertices[3] = Vertex(1.0f, -1.0f, -1.0f);
	vertices[4] = Vertex(-1.0f, -1.0f, 1.0f);
	vertices[5] = Vertex(-1.0f, 1.0f, 1.0f);
	vertices[6] = Vertex(1.0f, 1.0f, 1.0f);
	vertices[7] = Vertex(1.0f, -1.0f, 1.0f);
	VB->Unlock();

	WORD* indices = 0;
	IB->Lock(
		0,
		0,
		(void**)&indices,
		0
	);
	//前面
	indices[0] = 0; indices[1] = 1; indices[2] = 2;
	indices[3] = 0; indices[4] = 2; indices[5] = 3;

	//后面
	indices[6] = 4; indices[7] = 6; indices[8] = 5;
	indices[9] = 4; indices[10] = 7; indices[11] = 6;

	//左面
	indices[12] = 4; indices[13] = 5; indices[14] = 1;
	indices[15] = 4; indices[16] = 1; indices[17] = 0;

	//右面
	indices[18] = 3; indices[19] = 2; indices[20] = 6;
	indices[21] = 3; indices[22] = 6; indices[23] = 7;

	//上面
	indices[24] = 1; indices[25] = 5; indices[26] = 6;
	indices[27] = 1; indices[28] = 6; indices[29] = 2;

	//下面
	indices[30] = 4; indices[31] = 0; indices[32] = 3;
	indices[33] = 4; indices[34] = 3; indices[35] = 7;
	IB->Unlock();

	//取景变换
	D3DXVECTOR3 position(0.0f, 0.0f, -5.0f);
	D3DXVECTOR3 target(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);
	D3DXMATRIX V;
	D3DXMatrixLookAtLH(&V, &position, &target, &up);
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
	device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	return true;
}

void Cleanup() {
	d3d::Release<IDirect3DVertexBuffer9*>(VB);
	d3d::Release<IDirect3DIndexBuffer9*>(IB);
}

bool Display(float timeDelta) {
	if (device) {
		Sleep(50);
		//旋转立方体
		D3DXMATRIX Rx, Ry;
		//沿x轴旋转45度
		D3DXMatrixRotationX(&Rx, 3.14f / 4.0f);
		static float y = 0.0f;
		D3DXMatrixRotationY(&Ry, y);
		y += timeDelta;
		if (y >= 6.28f)
			y = 0.0f;
		D3DMATRIX p = Rx * Ry;
		device->SetTransform(D3DTS_WORLD, &p);
		device->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
			0XFFFFFFFF, 1.0f, 0);
		device->BeginScene();

		device->SetStreamSource(0, VB, 0, sizeof(Vertex));
		device->SetIndices(IB);
		device->SetFVF(Vertex::FVF);
		device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 8, 0, 12);
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