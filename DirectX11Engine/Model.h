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
#include <vector>

using namespace std;
using namespace DirectX;

/////////////////////
// CUSTOM INCLUDES //
/////////////////////
#include "texture.h"
#include "GfxUtil.h"

using namespace GfxUtil;

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
	Model(XMFLOAT3 scale, XMFLOAT3 orientation, XMFLOAT3 translation, string name);
	Model(const Model&);
	//~Model();

	bool Initialize(ID3D11Device* const device, ID3D11DeviceContext* const deviceContext, const string modelFilename, vector<string> texFileNames, const EShaderType shaderType);

	/** Needed by the shader to draw this model */
	int GetIndexCount();

	Material* GetMaterial();
	void SetMaterial(Material* const mat);
	//ID3D11ShaderResourceView** GetTextureArray(); // TODO: replace w material

	void SetResourceView(const int index, ID3D11ShaderResourceView* const view);

	bool LoadModel(const string name);

	/* Once the GPU has an active vertex buffer it can use the shader to render that buffer. */
	void Draw(ID3D11DeviceContext* const deviceContext);

	inline XMFLOAT3 GetPosition() const
	{
		return _position;
	}

	inline XMFLOAT3 GetOrientation() const
	{
		return _orientation;
	}

	inline void SetPosition(XMFLOAT3 pos)
	{
		_position = pos;
	}

	inline void SetOrientation(XMFLOAT3 or)
	{
		_orientation = or ;
	}

	inline XMFLOAT3 GetScale() { return _scale; }

	inline void SetScale(XMFLOAT3 scale) { _scale = scale; }

public:
	string Name;

private:
	bool InitializeBuffers(ID3D11Device* const device);

	// Functions for calculating the tangent and binormal vectors for the model.
	void CalculateModelVectors();
	void CalculateTangentBinormal(TempVertexType, TempVertexType, TempVertexType, VectorType&, VectorType&);
	void CalculateNormal(VectorType, VectorType, VectorType&);

private:
	ComPtr<ID3D11Buffer> _vertexBuffer, _indexBuffer;
	int _vertexCount, _indexCount;
	unique_ptr<ModelType> _model; // used to read in and hold the model data before it is placed in the vertex buffer.
	unique_ptr<Material> _material;

	// Relative position and orientation (TODO: tie to parent actor)
	XMFLOAT3 _position;
	XMFLOAT3 _orientation;
	XMFLOAT3 _scale;
};

