////////////////////////////////////////////////////////////////////////////////
// Filename: model.h
////////////////////////////////////////////////////////////////////////////////

#pragma once

//////////////
// INCLUDES //
//////////////
#include <d3d11.h>
#include <directxmath.h>
using namespace DirectX;

////////////////////////////////////////////////////////////////////////////////
// Class name: ModelClass
////////////////////////////////////////////////////////////////////////////////
class Model
{
private:
// this typedef must match the layout in the ColorShaderClass 
	struct VertexType
	{
		XMFLOAT3 position;
		XMFLOAT4 color;
	};

public:
	Model();
	Model(const Model&);
	~Model();

	bool Initialize(ID3D11Device*);
	void Shutdown();
	void Render(ID3D11DeviceContext*);

	/** Needed by the shader to draw this model */
	int GetIndexCount();

private:
	bool InitializeBuffers(ID3D11Device* device);
	void ShutdownBuffers();
	/* Once the GPU has an active vertex buffer it can use the shader to render that buffer. */
	void RenderBuffers(ID3D11DeviceContext* deviceContext);

private:
	ID3D11Buffer *_vertexBuffer, *_indexBuffer;
	int _vertexCount, _indexCount;
};

