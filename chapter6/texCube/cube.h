#pragma once
#include <d3dx9.h>

class Cube {
private:
	IDirect3DDevice9* _device;
	IDirect3DVertexBuffer9* _vb;
	IDirect3DIndexBuffer9* _ib;
public:
	Cube(IDirect3DDevice9* device);
	~Cube();
	bool draw(D3DXMATRIX* world, D3DMATERIAL9* mtrl, IDirect3DTexture9* tex);
};