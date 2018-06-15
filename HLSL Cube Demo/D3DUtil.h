#pragma once

#include <d3dx9.h>
#include <string>
#include <DxErr.h>

#include <vector>

#pragma comment(lib, "d3d9")
#pragma comment(lib, "d3dx9")
#pragma comment(lib, "DxErr")

#if defined (DEBUG) | defined(__DEBUG)
#define HR(x) { HRESULT hr = x; if (FAILED(hr)) DXTrace(__FILE__, __LINE__, hr, #x, true); }
#else
#define HR(x) { x; }
#endif

#define ReleaseCOM(x) { if (x) x->Release(); x = NULL; }

#define HLSL(x) #x
