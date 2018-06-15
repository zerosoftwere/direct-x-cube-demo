#include "Vertex.h"
#include "D3DUtil.h"
#include "D3DApp.h"

LPDIRECT3DVERTEXDECLARATION9 VertexPos::Decl = 0;
LPDIRECT3DVERTEXDECLARATION9 VertexCol::Decl = 0;

void InitAllVertexDeclarations()
{
	D3DVERTEXELEMENT9 vertexPosElem[] = 
	{
		{0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
		D3DDECL_END()
	};
	HR(gd3dDevice->CreateVertexDeclaration(vertexPosElem, &VertexPos::Decl));

	D3DVERTEXELEMENT9 vertexColElem[] = 
	{
		{0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
		{0, 12, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0},
		D3DDECL_END()
	};
	HR(gd3dDevice->CreateVertexDeclaration(vertexColElem, &VertexCol::Decl));
}

void DestroyAllVertexDeclarations()
{
	ReleaseCOM(VertexPos::Decl);
	ReleaseCOM(VertexCol::Decl);
}