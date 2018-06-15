#pragma once

#include <d3dx9.h>

struct VertexPos
{
public:
	VertexPos(float x, float y, float z) : pos(x, y, z) {};
	VertexPos(const D3DXVECTOR3& pos) : pos(pos) {};

	D3DXVECTOR3 pos;
	static LPDIRECT3DVERTEXDECLARATION9 Decl;
};

struct VertexCol
{
	VertexCol() : pos(0.0f, 0.0f, 0.0f), col(1.0f, 0.0f, 0.0f, 1.0f) { }
	VertexCol(float x, float y, float z, float r, float g, float b, float a) : pos(x, y, z), col(r, g, b, a) { }
	VertexCol(const D3DXVECTOR3& pos, const D3DXCOLOR& col) : pos(pos), col(col) { }

	D3DXVECTOR3 pos;
	D3DXCOLOR	col;

	static LPDIRECT3DVERTEXDECLARATION9 Decl;
};

void InitAllVertexDeclarations();
void DestroyAllVertexDeclarations();

