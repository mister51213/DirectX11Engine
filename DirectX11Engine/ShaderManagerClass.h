////////////////////////////////////////////////////////////////////////////////
// Filename: shadermanagerclass.h
////////////////////////////////////////////////////////////////////////////////
#pragma once

///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "d3dclass.h"

#include "TextureShaderClass.h"
#include "LightShaderClass.h"
#include "FontShaderClass.h"
#include "ReflectionShaderClass.h"
#include "WaterShaderClass.h"
//#include "RefractionShaderClass.h"
#include "DiffuseShaderClass.h"
#include "DepthShaderClass.h"

#include "GfxUtil.h"

#include "LightClass.h"


using namespace GfxUtil;

////////////////////////////////////////////////////////////////////////////////
// Class name: ShaderManagerClass
////////////////////////////////////////////////////////////////////////////////
class ShaderManagerClass
{
public:
	ShaderManagerClass();
	ShaderManagerClass(const ShaderManagerClass&);
	~ShaderManagerClass();

	bool Initialize(ID3D11Device*, HWND);

	bool Render(ID3D11DeviceContext* device, int indexCount, 
		MatrixBufferType& transforms,
		XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, Material* material, LightClass* lights[], SceneEffects& effects,
		XMFLOAT3 cameraPos = XMFLOAT3(0,0,0), EShaderType shaderType = EShaderType::EMATERIAL_DEFAULT, XMMATRIX reflectionMatrix = XMMatrixIdentity());

	bool RenderFontShader(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix,
		XMMATRIX projectionMatrix, ID3D11ShaderResourceView* texture, vector<Microsoft::WRL::ComPtr <ID3D11ShaderResourceView>>& texViews, XMFLOAT4 pixelColor);

	FontShaderClass* GetFontShader();

public:
	unique_ptr<LightShaderClass> _LightShader;
	unique_ptr<DepthShaderClass> _DepthShader;
	unique_ptr<ReflectionShaderClass> _ReflectionShader;
	//unique_ptr<RefractionShaderClass> _RefractionShader;
	unique_ptr<DiffuseShaderClass> _DiffuseShader;
	unique_ptr<TextureShaderClass> _TextureShader;
	unique_ptr<FontShaderClass> _FontShader;
	unique_ptr<WaterShaderClass> _WaterShader;
};