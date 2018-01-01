////////////////////////////////////////////////////////////////////////////////
// Filename: debugwindowclass.h
////////////////////////////////////////////////////////////////////////////////
#pragma once

//////////////
// INCLUDES //
//////////////
#include <d3d11.h>
#include <DirectXMath.h>
#include <wrl/client.h>

using namespace DirectX;
using namespace Microsoft::WRL;

////////////////////////////////////////////////////////////////////////////////
// Class name: DebugWindowClass
////////////////////////////////////////////////////////////////////////////////
class DebugWindowClass
{
private:
	struct VertexType
	{
		XMFLOAT3 position;
		XMFLOAT2 texture;
	};

public:
	DebugWindowClass();
	DebugWindowClass(const DebugWindowClass&);
	~DebugWindowClass();

	bool Initialize(ID3D11Device* device, int screenWidth, int screenHeight, int bitmapWidth, int bitmapHeight);
	bool Render(ID3D11DeviceContext*, int, int);

	int GetIndexCount();

private:
	bool InitializeBuffers(ID3D11Device*);
	bool UpdateBuffers(ID3D11DeviceContext*, int, int);
	void LoadVertices(ID3D11DeviceContext*);

private:
	ComPtr<ID3D11Buffer> _vertexBuffer, _indexBuffer;
	int _vertexCount, _indexCount;
	int _screenWidth, _screenHeight;
	int _bitmapWidth, _bitmapHeight;
	int _previousPosX, _previousPosY;
};