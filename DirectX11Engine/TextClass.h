////////////////////////////////////////////////////////////////////////////////
// Filename: textclass.h
////////////////////////////////////////////////////////////////////////////////
#pragma once

//////////////
// INCLUDES //
//////////////
#include "fontclass.h"
#include "ShaderManagerClass.h"
#include "ShaderManagerClass.h"
#include <wrl/client.h>

using namespace Microsoft::WRL;

////////////////////////////////////////////////////////////////////////////////
// Class name: TextClass
////////////////////////////////////////////////////////////////////////////////
class TextClass
{
private:
	struct VertexType
	{
		XMFLOAT3 position;
		XMFLOAT2 texture;
	};

public:
	TextClass();
	TextClass(const TextClass&);
	~TextClass();

	bool Initialize(ID3D11Device*, ID3D11DeviceContext*, int, int, int, bool, FontClass*, char*, int, int, float, float, float);
	void Render(ID3D11DeviceContext*, ShaderManagerClass* pShaderManager, MatrixBufferType transforms,/* XMMATRIX, XMMATRIX, XMMATRIX,*/ ID3D11ShaderResourceView*, vector<Microsoft::WRL::ComPtr <ID3D11ShaderResourceView>>& texViews);

	bool UpdateSentence(ID3D11DeviceContext*, FontClass*, char*, int, int, float, float, float);

private:
	bool InitializeSentence(ID3D11Device*, ID3D11DeviceContext*, FontClass*, char*, int, int, float, float, float);
	void RenderSentence(ID3D11DeviceContext*, ShaderManagerClass* shaderManager, MatrixBufferType transforms, /*XMMATRIX, XMMATRIX, XMMATRIX, */ID3D11ShaderResourceView*, vector<Microsoft::WRL::ComPtr <ID3D11ShaderResourceView>>& texViews);

private:
	unique_ptr<ShaderManagerClass> _ShaderManager;

	ComPtr<ID3D11Buffer> _vertexBuffer, _indexBuffer, _vertexBuffer2, _indexBuffer2;
	int _screenWidth, _screenHeight, _maxLength, _vertexCount, _indexCount;
	XMFLOAT4 _pixelColor;
};