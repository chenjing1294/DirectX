#include "d3dUtility.h"

IDirect3DDevice9* device = nullptr;
const int Width = 800;
const int Height = 600;

IDirect3DVertexBuffer9* VB = nullptr;
IDirect3DTexture9* FloorTex = nullptr;
IDirect3DTexture9* WallTex = nullptr;
IDirect3DTexture9* MirrorTex = nullptr;

D3DMATERIAL9 FloorMtrl = d3d::WHITE_MTRL;
D3DMATERIAL9 WallMtrl = d3d::WHITE_MTRL;
D3DMATERIAL9 MirrorMtrl = d3d::WHITE_MTRL;

ID3DXMesh* Teapot = nullptr;
D3DXVECTOR3 TeapotPosition(0.0f, 3.0f, -7.5f);
D3DMATERIAL9 TeapotMtrl = d3d::YELLOW_MTRL;

void RenderScene();
void RenderMirror();

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
	//make walls have low specular reflectance: 20%
	WallMtrl.Specular = d3d::WHITE * 0.2f;
	//create the teapot
	D3DXCreateTeapot(device, &Teapot, 0);

	//
	// Create and specify geometry.  For this sample we draw a floor
	// and a wall with a mirror on it.  We put the floor, wall, and
	// mirror geometry in one vertex buffer.
	//
	//   |----|----|----|
	//   |Wall|Mirr|Wall|
	//   |    | or |    |
	//   /--------------/
	//  /   Floor      /
	// /--------------/
	//
	device->CreateVertexBuffer(
		24 * sizeof(Vertex),
		0,
		Vertex::FVF,
		D3DPOOL_MANAGED,
		&VB,
		0
	);
	Vertex* v;
	VB->Lock(0, 0, (void**)&v, 0);
	// floor
	v[0] = Vertex(-7.5f, 0.0f, -10.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f);
	v[1] = Vertex(-7.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f);
	v[2] = Vertex(7.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f);

	v[3] = Vertex(-7.5f, 0.0f, -10.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f);
	v[4] = Vertex(7.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f);
	v[5] = Vertex(7.5f, 0.0f, -10.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f);

	// wall
	v[6] = Vertex(-7.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
	v[7] = Vertex(-7.5f, 5.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f);
	v[8] = Vertex(-2.5f, 5.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);

	v[9] = Vertex(-7.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
	v[10] = Vertex(-2.5f, 5.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);
	v[11] = Vertex(-2.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f);

	// Note: We leave gap in middle of walls for mirror

	v[12] = Vertex(2.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
	v[13] = Vertex(2.5f, 5.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f);
	v[14] = Vertex(7.5f, 5.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);

	v[15] = Vertex(2.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
	v[16] = Vertex(7.5f, 5.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);
	v[17] = Vertex(7.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f);

	// mirror
	v[18] = Vertex(-2.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
	v[19] = Vertex(-2.5f, 5.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f);
	v[20] = Vertex(2.5f, 5.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);

	v[21] = Vertex(-2.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
	v[22] = Vertex(2.5f, 5.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);
	v[23] = Vertex(2.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f);

	VB->Unlock();

	// Load Textures, set filters.

	D3DXCreateTextureFromFile(device, TEXT("checker.jpg"), &FloorTex);
	D3DXCreateTextureFromFile(device, TEXT("brick0.jpg"), &WallTex);
	D3DXCreateTextureFromFile(device, TEXT("ice.bmp"), &MirrorTex);

	device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

	// Lights.
	D3DXVECTOR3 lightDir(0.707f, -0.707f, 0.707f);
	D3DXCOLOR color(1.0f, 1.0f, 1.0f, 1.0f);
	D3DLIGHT9 light = d3d::InitDirectionalLight(&lightDir, &color);

	device->SetLight(0, &light);
	device->LightEnable(0, true);

	device->SetRenderState(D3DRS_NORMALIZENORMALS, true);
	device->SetRenderState(D3DRS_SPECULARENABLE, true);

	//
	// Set Camera.
	//

	D3DXVECTOR3    pos(-10.0f, 3.0f, -15.0f);
	D3DXVECTOR3 target(0.0, 0.0f, 0.0f);
	D3DXVECTOR3     up(0.0f, 1.0f, 0.0f);

	D3DXMATRIX V;
	D3DXMatrixLookAtLH(&V, &pos, &target, &up);

	device->SetTransform(D3DTS_VIEW, &V);

	//
	// Set projection matrix.
	//
	D3DXMATRIX proj;
	D3DXMatrixPerspectiveFovLH(
		&proj,
		D3DX_PI / 4.0f, // 45 - degree
		(float)Width / (float)Height,
		1.0f,
		1000.0f);
	device->SetTransform(D3DTS_PROJECTION, &proj);
	return true;
}

void Cleanup() {
	d3d::Release<IDirect3DVertexBuffer9*>(VB);
	d3d::Release<IDirect3DTexture9*>(FloorTex);
	d3d::Release<IDirect3DTexture9*>(WallTex);
	d3d::Release<IDirect3DTexture9*>(MirrorTex);
	d3d::Release<ID3DXMesh*>(Teapot);
}

bool Display(float timeDelta) {
	if (device) {
		Sleep(50);
		static float radius = 20.0f;
		if (GetAsyncKeyState(VK_LEFT) & 0x8000f)
			TeapotPosition.x -= 3.0f * timeDelta;

		if (GetAsyncKeyState(VK_RIGHT) & 0x8000f)
			TeapotPosition.x += 3.0f * timeDelta;

		if (GetAsyncKeyState(VK_UP) & 0x8000f)
			radius -= 2.0f * timeDelta;

		if (GetAsyncKeyState(VK_DOWN) & 0x8000f)
			radius += 2.0f * timeDelta;


		static float angle = (3.0f * D3DX_PI) / 2.0f;

		if (GetAsyncKeyState('A') & 0x8000f)
			angle -= 0.5f * timeDelta;

		if (GetAsyncKeyState('S') & 0x8000f)
			angle += 0.5f * timeDelta;

		D3DXVECTOR3 position(cosf(angle) * radius, 3.0f, sinf(angle) * radius);
		//D3DXVECTOR3 position(-3.0f, 3.0f, -10.0f);
		D3DXVECTOR3 target(0.0f, 0.0f, 0.0f);
		D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);
		D3DXMATRIX V;
		D3DXMatrixLookAtLH(&V, &position, &target, &up);
		device->SetTransform(D3DTS_VIEW, &V);

		//
		// Draw the scene:
		//
		device->Clear(0, 0,
			D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL,
			0xff000000, 1.0f, 0L);
		device->BeginScene();
		RenderScene();
		RenderMirror();
		device->EndScene();
		device->Present(0, 0, 0, 0);
	}
	return true;
}

void RenderScene()
{
	// draw teapot
	device->SetMaterial(&TeapotMtrl);
	device->SetTexture(0, 0);
	D3DXMATRIX W;
	D3DXMatrixTranslation(&W,
		TeapotPosition.x,
		TeapotPosition.y,
		TeapotPosition.z);

	device->SetTransform(D3DTS_WORLD, &W);
	Teapot->DrawSubset(0);

	D3DXMATRIX I;
	D3DXMatrixIdentity(&I);
	device->SetTransform(D3DTS_WORLD, &I);

	device->SetStreamSource(0, VB, 0, sizeof(Vertex));
	device->SetFVF(Vertex::FVF);

	// draw the floor
	device->SetMaterial(&FloorMtrl);
	device->SetTexture(0, FloorTex);
	device->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 2);

	// draw the walls
	device->SetMaterial(&WallMtrl);
	device->SetTexture(0, WallTex);
	device->DrawPrimitive(D3DPT_TRIANGLELIST, 6, 4);

	// draw the mirror
	device->SetMaterial(&MirrorMtrl);
	device->SetTexture(0, MirrorTex);
	device->DrawPrimitive(D3DPT_TRIANGLELIST, 18, 2);
}

void RenderMirror()
{
	//
	// Draw Mirror quad to stencil buffer ONLY.  In this way
	// only the stencil bits that correspond to the mirror will
	// be on.  Therefore, the reflected teapot can only be rendered
	// where the stencil bits are turned on, and thus on the mirror 
	// only.
	//

	device->SetRenderState(D3DRS_STENCILENABLE, true);
	device->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_ALWAYS);
	device->SetRenderState(D3DRS_STENCILREF, 0x1);
	device->SetRenderState(D3DRS_STENCILMASK, 0xffffffff);
	device->SetRenderState(D3DRS_STENCILWRITEMASK, 0xffffffff);
	device->SetRenderState(D3DRS_STENCILZFAIL, D3DSTENCILOP_KEEP);
	device->SetRenderState(D3DRS_STENCILFAIL, D3DSTENCILOP_KEEP);
	device->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_REPLACE);

	// disable writes to the depth and back buffers
	device->SetRenderState(D3DRS_ZWRITEENABLE, false);
	device->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
	device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ZERO);
	device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

	// draw the mirror to the stencil buffer
	device->SetStreamSource(0, VB, 0, sizeof(Vertex));
	device->SetFVF(Vertex::FVF);
	device->SetMaterial(&MirrorMtrl);
	device->SetTexture(0, MirrorTex);
	D3DXMATRIX I;
	D3DXMatrixIdentity(&I);
	device->SetTransform(D3DTS_WORLD, &I);
	device->DrawPrimitive(D3DPT_TRIANGLELIST, 18, 2);

	// re-enable depth writes
	device->SetRenderState(D3DRS_ZWRITEENABLE, true);

	// only draw reflected teapot to the pixels where the mirror
	// was drawn to.
	device->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_EQUAL);
	device->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_KEEP);

	// position reflection
	D3DXMATRIX W, T, R;
	D3DXPLANE plane(0.0f, 0.0f, 1.0f, 0.0f); // xy plane
	D3DXMatrixReflect(&R, &plane);

	D3DXMatrixTranslation(&T,
		TeapotPosition.x,
		TeapotPosition.y,
		TeapotPosition.z);

	W = T * R;

	// clear depth buffer and blend the reflected teapot with the mirror
	device->Clear(0, 0, D3DCLEAR_ZBUFFER, 0, 1.0f, 0);
	device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_DESTCOLOR);
	device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ZERO);

	// Finally, draw the reflected teapot
	device->SetTransform(D3DTS_WORLD, &W);
	device->SetMaterial(&TeapotMtrl);
	device->SetTexture(0, 0);

	device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
	Teapot->DrawSubset(0);

	// Restore render states.
	device->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
	device->SetRenderState(D3DRS_STENCILENABLE, false);
	device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
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