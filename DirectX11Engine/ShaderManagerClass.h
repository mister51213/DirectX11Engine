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
#include "RefractionShaderClass.h"

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

	bool RenderAll(ID3D11DeviceContext* device, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, Material* material, LightClass* light, SceneEffects effects, 
		XMFLOAT3 cameraPos = XMFLOAT3(0,0,0), XMMATRIX reflectionMatrix = XMMatrixIdentity(), ID3D11ShaderResourceView * reflectionTexture = nullptr, ID3D11ShaderResourceView * refractionTexture = nullptr, ID3D11ShaderResourceView * normalTexture = nullptr);

	bool RenderTextureShader(ID3D11DeviceContext*, int, XMMATRIX, XMMATRIX, XMMATRIX, ID3D11ShaderResourceView*);

	bool RenderLightShader(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix,
		XMMATRIX projectionMatrix, Material* material, ID3D11ShaderResourceView** textureArray, XMFLOAT3 lightDirection, XMFLOAT4 ambientColor, XMFLOAT4 diffuseColor,
		XMFLOAT3 cameraPosition, XMFLOAT4 specularColor, float specularPower, float fogStart, float fogEnd, XMFLOAT4 clipPlane, float translation, float transparency);

	bool RenderReflectionShader(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix, 
		XMMATRIX projectionMatrix, ID3D11ShaderResourceView* texture, ID3D11ShaderResourceView* reflectionTexture, XMMATRIX reflectionMatrix);

	bool RenderWaterShader(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix,
		XMMATRIX projectionMatrix, XMMATRIX reflectionMatrix,
		ID3D11ShaderResourceView* reflectionTexture, ID3D11ShaderResourceView* refractionTexture,
		ID3D11ShaderResourceView* normalTexture, float waterTranslation, float reflectRefractScale);

	bool RenderRefractionShader(ID3D11DeviceContext * deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, ID3D11ShaderResourceView * texture, XMFLOAT3 lightDirection, XMFLOAT4 ambientColor, XMFLOAT4 diffuseColor, XMFLOAT4 clipPlane);

	bool RenderFontShader(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix,
		XMMATRIX projectionMatrix, ID3D11ShaderResourceView* texture, XMFLOAT4 pixelColor);

	FontShaderClass* GetFontShader();

private:
	// Make a private class object for each shader type the application will be using.
	unique_ptr<TextureShaderClass> _TextureShader;
	unique_ptr<LightShaderClass> _LightShader;
	unique_ptr<FontShaderClass> _FontShader;
	unique_ptr<ReflectionShaderClass> _ReflectionShader;
	unique_ptr<WaterShaderClass> _WaterShader;
	unique_ptr<RefractionShaderClass> _RefractionShader;
};