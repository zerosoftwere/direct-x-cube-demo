#include "D3DApp.h"
#include "D3DUtil.h"
#include "Vertex.h"

const char* shader = HLSL(
	uniform extern float4x4 gWVP;

	void ColorVS(in float3 iPositionL : POSITION0, in float4 iColor : COLOR0, 
				out float4 dColor : COLOR0, out float4 oPositionH : POSITION0)
	{
		oPositionH = mul(float4(iPositionL, 1.0f), gWVP);
		dColor = iColor;
	}

	float4 ColorPS(in float4 dColor : COLOR0) : COLOR0
	{
		return dColor;
	}

	technique ColorTech
	{
		pass P0
		{
			vertexshader = compile vs_2_0 ColorVS();
			pixelshader  = compile ps_2_0 ColorPS();
		
			fillmode = solid;
		}
	}
);

class CubeDemo : public D3DApp
{
public:
	CubeDemo(HINSTANCE hInstance, std::string winCaption, DWORD requestedVP, D3DDEVTYPE deviceType);
	~CubeDemo();

	void OnLostDevice() override;
	void OnResetDevice() override;

	void UpdateScene(float dt) override;
	void DrawScene() override;

	void BuildFX();
private:
	LPDIRECT3DVERTEXBUFFER9 mCubeVB;
	LPDIRECT3DINDEXBUFFER9  mCubeIB;

	D3DXMATRIX mView;
	D3DXMATRIX mProj;

	LPD3DXEFFECT mFX;
	D3DXHANDLE   mhWVP;
	D3DXHANDLE   mhTech;

	float mRot;
};

INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR szCmdLine, INT nCmdShow)
{
	CubeDemo app(hInstance, "HLSL Cube Demo", D3DCREATE_HARDWARE_VERTEXPROCESSING, D3DDEVTYPE_HAL);
	gd3dApp = &app;
	return gd3dApp->Run();
}

CubeDemo::CubeDemo(HINSTANCE hInstance, std::string winCaption, DWORD reqestedVP, D3DDEVTYPE devType)
	: D3DApp(hInstance, winCaption, reqestedVP, devType), mRot(0.0f)
{
	InitAllVertexDeclarations();

	const VertexCol cubeVertices[] = {
		VertexCol( 1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f),
		VertexCol( 1.0f,  1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 1.0f),
		VertexCol(-1.0f,  1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f),
		VertexCol(-1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f),

		VertexCol( 1.0f, -1.0f,  1.0f, 1.0f, 0.0f, 0.0f, 1.0f),
		VertexCol( 1.0f,  1.0f,  1.0f, 0.0f, 1.0f, 0.0f, 1.0f),
		VertexCol(-1.0f,  1.0f,  1.0f, 0.0f, 0.0f, 1.0f, 1.0f),
		VertexCol(-1.0f, -1.0f,  1.0f, 1.0f, 0.0f, 0.0f, 1.0f)
	};

	const WORD cubeIndices[] = {
		0, 3, 1,	3, 2, 1, // Front
		0, 1, 4,	4, 1, 5, // Right
		1, 2, 5,	2, 6, 5, // Top
		3, 7, 2,	2, 7, 6, // Left
		4, 5, 7,	7, 5, 6, // Back
		0, 4, 3,	4, 7, 3  // Bottom
	};

	HR(gd3dDevice->CreateVertexBuffer(8 * sizeof(VertexCol), D3DUSAGE_WRITEONLY, NULL, D3DPOOL_MANAGED, &mCubeVB, 0));
	VertexCol* verts = 0;
	HR(mCubeVB->Lock(0, 0, (void**) &verts, 0));
	for (UINT8 i = 0; i < 8; i++) verts[i] = cubeVertices[i];
	HR(mCubeVB->Unlock());

	HR(gd3dDevice->CreateIndexBuffer(36 * sizeof(WORD), D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_MANAGED, &mCubeIB, 0));
	WORD* indices = 0;
	HR(mCubeIB->Lock(0, 0, (void**) &indices, 0));
	for (INT8 i = 0; i < 36; i++) indices[i] = cubeIndices[i];
	HR(mCubeIB->Unlock());

	float aspectRatio = (float) md3dPP.BackBufferWidth / (float) md3dPP.BackBufferHeight;
	D3DXMatrixPerspectiveFovLH(&mProj, D3DX_PI * 0.25f, aspectRatio, 0.1f, 100.0f);
	D3DXMatrixLookAtLH(&mView, &D3DXVECTOR3(0.0f, -2.0f, -6.0f), &D3DXVECTOR3(0.0f, 0.0f, 0.0f), &D3DXVECTOR3(0.0f, 1.0f, 0.0f));

	BuildFX();
}

CubeDemo::~CubeDemo()
{
	DestroyAllVertexDeclarations();

	ReleaseCOM(mCubeVB);
	ReleaseCOM(mCubeIB);
	ReleaseCOM(mFX);
}

void CubeDemo::UpdateScene(float dt)
{
	mRot += 2.0f * dt;
	if (mRot > D3DX_PI * 2) mRot = 0.0f;
}

void CubeDemo::DrawScene()
{
	HR(gd3dDevice->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DXCOLOR(0.8f, 0.8f, 1.0f, 1.0f), 1.0f, 0));
	HR(gd3dDevice->BeginScene());

	D3DXMATRIX world;
	D3DXMatrixRotationY(&world, mRot);
	HR(mFX->SetMatrix(mhWVP, &(world * mView * mProj)));
	HR(mFX->SetTechnique(mhTech));
	HR(mFX->CommitChanges());

	UINT numPasses = 0;
	HR(mFX->Begin(&numPasses, 0));
	for (UINT i = 0; i < numPasses; i++)
	{
		HR(mFX->BeginPass(i));

		HR(gd3dDevice->SetStreamSource(0, mCubeVB, 0, sizeof(VertexCol)));
		HR(gd3dDevice->SetIndices(mCubeIB));
		HR(gd3dDevice->SetVertexDeclaration(VertexCol::Decl));
		HR(gd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 0, 0, 16));

		HR(mFX->EndPass());
	}
	HR(mFX->End());

	HR(gd3dDevice->EndScene());
	HR(gd3dDevice->Present(0, 0, 0, 0));
}

void CubeDemo::BuildFX()
{
	LPD3DXBUFFER errors = 0;
	HR(D3DXCreateEffect(gd3dDevice, shader, strlen(shader), 0, 0, D3DXSHADER_DEBUG, 0, &mFX, &errors));
	if (errors) MessageBox(NULL, (char*) errors->GetBufferPointer(), "Effect Error", MB_OK);

	mhTech = mFX->GetTechniqueByName("ColorTech");
	mhWVP  = mFX->GetParameterByName(NULL, "gWVP");
}

void CubeDemo::OnLostDevice()
{
	HR(mFX->OnLostDevice());
}

void CubeDemo::OnResetDevice()
{
	HR(mFX->OnResetDevice());
}