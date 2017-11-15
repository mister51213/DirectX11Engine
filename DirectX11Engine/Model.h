////////////////////////////////////////////////////////////////////////////////
// Filename: model.h
////////////////////////////////////////////////////////////////////////////////
#pragma once

//////////////
// DX INCLUDES //
//////////////
#include <d3d11.h>
#include <directxmath.h>
#include <fstream>

using namespace std;
using namespace DirectX;

/////////////////////
// CUSTOM INCLUDES //
/////////////////////
#include "texture.h"

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
		XMFLOAT2 texture;
		XMFLOAT3 normal;
	};

	// Contains position, texture coords, and normal vectors corresponding to model file to be loaded
	struct ModelType
	{
		float x, y, z;
		float tu, tv;
		float nx, ny, nz;
	};

public:
	Model();
	Model(const Model&);
	~Model();

	bool Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, char* modelFilename, char* textureFilename1, char* textureFilename2, char* textureFilename3);
	void Shutdown();
	void Render(ID3D11DeviceContext*);

	/** Needed by the shader to draw this model */
	int GetIndexCount();
	//ID3D11ShaderResourceView* GetTexture();
	ID3D11ShaderResourceView** GetTextureArray();

	bool LoadModel(char*);
	void ReleaseModel();

private:
	bool InitializeBuffers(ID3D11Device* device);
	void ShutdownBuffers();

	/* Once the GPU has an active vertex buffer it can use the shader to render that buffer. */
	void RenderBuffers(ID3D11DeviceContext* deviceContext);

	bool LoadTextures(ID3D11Device*, ID3D11DeviceContext*, char* fileName1, char* fileName2, char* filename3);
	void ReleaseTextures();

private:
	ID3D11Buffer *_vertexBuffer, *_indexBuffer;
	int _vertexCount, _indexCount;
	ModelType* _model; // used to read in and hold the model data before it is placed in the vertex buffer.
	//TextureClass* _Texture;
	TextureClass* _TextureArray;
};

