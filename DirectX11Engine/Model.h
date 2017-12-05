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
#include <string>

using namespace std;
using namespace DirectX;

/////////////////////
// CUSTOM INCLUDES //
/////////////////////
#include "texture.h"
#include "GfxUtil.h"

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
		XMFLOAT3 tangent;
		XMFLOAT3 binormal;
	};

	// Contains position, texture coords, and normal vectors corresponding to model file to be loaded
	struct ModelType
	{
		float x, y, z;
		float tu, tv;
		float nx, ny, nz;
		float tx, ty, tz;
		float bx, by, bz;
	};

	// Used to calculate tangent and binormal
	struct TempVertexType
	{
		float x, y, z;
		float tu, tv;
		float nx, ny, nz;
	};

	struct VectorType
	{
		float x, y, z;
	};

public:
	Model();
	Model(const Model&);
	~Model();

	bool Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, char* modelFilename, char* textureFilename1, char* textureFilename2, char* textureFilename3, char* textureFilename4, char* textureFilename5, char* textureFilename6);
	bool InitializeDDS(ID3D11Device * device, ID3D11DeviceContext * deviceContext, char * modelFilename, char * textureFilename1, char * textureFilename2, char * lightMapFileName3, char * alphaFileName4, char * normalMapFilename5, char * specMapFilename6);
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

	bool LoadTextures(ID3D11Device*, ID3D11DeviceContext*, char* fileName1, char* fileName2, char* fileName3, char* fileName4, char* textureFileName5, char* texFileName6);
	bool LoadTexturesDDS(ID3D11Device * device, ID3D11DeviceContext * deviceContext, char * fileName1, char * fileName2, char * fileName3, char * fileName4, char * normalMapFileName, char * specMapFilename6);
	void ReleaseTextures();

	//functions for calculating the tangent and binormal vectors for the model.
	void CalculateModelVectors();
	void CalculateTangentBinormal(TempVertexType, TempVertexType, TempVertexType, VectorType&, VectorType&);
	void CalculateNormal(VectorType, VectorType, VectorType&);

private:
	ID3D11Buffer *_vertexBuffer, *_indexBuffer;
	int _vertexCount, _indexCount;
	ModelType* _model; // used to read in and hold the model data before it is placed in the vertex buffer.
	//TextureClass* _Texture;
	TextureClass* _TextureArray;
};

