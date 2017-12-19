////////////////////////////////////////////////////////////////////////////////
// Filename: shadermanagerclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "shadermanagerclass.h"

ShaderManagerClass::ShaderManagerClass()
{}

ShaderManagerClass::ShaderManagerClass(const ShaderManagerClass& other)
{}

ShaderManagerClass::~ShaderManagerClass()
{
}

bool ShaderManagerClass::Initialize(ID3D11Device* device, HWND hwnd)
{
	bool result;

		// Create the texture shader object.
	_TextureShader.reset(new TextureShaderClass);
	if (!_TextureShader)
	{
		return false;
	}

	// Initialize the texture shader object.
	result = _TextureShader->Initialize(device, hwnd, "../DirectX11Engine/texture_vs.hlsl", "../DirectX11Engine/texture_ps.hlsl");
	if (!result)
	{
		throw std::runtime_error("Could not initialize the texture shader object. - " + to_string(__LINE__));

		return false;
	}

	// Create the light shader object.
	_LightShader.reset(new LightShaderClass);
	if (!_LightShader)
	{
		return false;
	}

	// Initialize the light shader object.
	result = _LightShader->Initialize(device, hwnd, "../DirectX11Engine/Light_vs.hlsl", "../DirectX11Engine/Light_ps.hlsl");
	if (!result)
	{
		throw std::runtime_error("Could not initialize the light shader object. - " + to_string(__LINE__));

		return false;
	}

	// Create the bump map shader object.
	_FontShader.reset(new FontShaderClass);
	if (!_FontShader)
	{
		return false;
	}

	// Initialize the bump map shader object.
	result = _FontShader->Initialize(device, hwnd, "../DirectX11Engine/font.vs", "../DirectX11Engine/font.ps");
	if (!result)
	{
		throw std::runtime_error("Could not initialize the bump map shader object. - " + to_string(__LINE__));

		return false;
	}

	// Create the bump map shader object.
	_ReflectionShader.reset(new ReflectionShaderClass);
	if (!_FontShader)
	{
		return false;
	}

	// Initialize the bump map shader object.
	result = _ReflectionShader->Initialize(device, hwnd, "../DirectX11Engine/Reflection_vs.hlsl", "../DirectX11Engine/Reflection_ps.hlsl");
	if (!result)
	{
		throw std::runtime_error("Could not initialize the reflection shader object. - " + to_string(__LINE__));

		return false;
	}

	_WaterShader.reset(new WaterShaderClass);

	result = _WaterShader->Initialize(device, hwnd, "../DirectX11Engine/WaterShader_vs.hlsl", "../DirectX11Engine/WaterShader_ps.hlsl");
	if (!result)
	{
		throw std::runtime_error("Could not initialize the water shader object. - " + to_string(__LINE__));

		return false;
	}

	_RefractionShader.reset(new RefractionShaderClass);

	result = _RefractionShader->Initialize(device, hwnd, "../DirectX11Engine/Refraction_vs.hlsl", "../DirectX11Engine/Refraction_ps.hlsl");
	if (!result)
	{
		throw std::runtime_error("Could not initialize the refraction shader object. - " + to_string(__LINE__));

		return false;
	}

	return true;
}

// @TODO the params at teh end need to be encapsulated
bool ShaderManagerClass::RenderAll(ID3D11DeviceContext * device, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, Material * material, LightClass * light, SceneEffects effects, XMFLOAT3 cameraPos, XMMATRIX reflectionMatrix, ID3D11ShaderResourceView * reflectionTexture, ID3D11ShaderResourceView * refractionTexture, ID3D11ShaderResourceView * normalTexture)
{
	//@TODO: GO THROUGH EVERY SINGLE FUCKING CALL IN GRAPHICS.CPP AND MAKE SURE THEY MATCH WITH THE CALLS BELOW
	// (may have passed in wrong textures in water or reflection shader)
	bool result;

	switch (material->shaderType)
	{
	case EShaderType::ETEXTURE:
		result = _TextureShader->Render(device, indexCount, worldMatrix, viewMatrix, projectionMatrix, material->GetResourceArray()[0]);
		if (!result) return false;
		break;

	case EShaderType::ELIGHT_SPECULAR:
		result = _LightShader->Render(device, indexCount, worldMatrix, viewMatrix, projectionMatrix,
			material->GetResourceArray(), light->GetDirection(), light->GetAmbientColor(), light->GetDiffuseColor(),
			cameraPos, light->GetSpecularColor(), light->GetSpecularPower(), effects.fogStart, effects.fogEnd, effects.clipPlane, material->translation, material->transparency);
		if (!result) return false;
		break;

	case EShaderType::EREFLECTION:
		result = _ReflectionShader->Render(device, indexCount, worldMatrix, viewMatrix, projectionMatrix,
			material->GetResourceArray()[0], reflectionTexture, reflectionMatrix);
		if (!result) return false;
		break;

	case EShaderType::EREFRACTION:
		result = _RefractionShader->Render(device, indexCount, worldMatrix, viewMatrix, projectionMatrix,
			material->GetResourceArray()[0], light->GetDirection(), light->GetAmbientColor(), light->GetDiffuseColor(), effects.clipPlane);
		if (!result) return false;
		break;

	case EShaderType::EWATER:
		result = _WaterShader->Render(device, indexCount, worldMatrix, viewMatrix, projectionMatrix, reflectionMatrix,
			material->GetResourceArray(), /*reflectionTexture, refractionTexture, normalTexture, */material->translation, material->reflectRefractScale);
		if (!result) return false;
		break;

	case EShaderType::EFONT:
		result = _FontShader->Render(device, indexCount, worldMatrix, viewMatrix, projectionMatrix,
			material->GetResourceArray()[0], material->pixelColor);
		if (!result) return false;
		break;

	default:
		result = _TextureShader->Render(device, indexCount, worldMatrix, viewMatrix, projectionMatrix, material->GetResourceArray()[0]);
		if (!result) return false;
		break;
	}

	return result;
}

bool ShaderManagerClass::RenderTextureShader(ID3D11DeviceContext* device, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix,
	ID3D11ShaderResourceView* texture)
{
	bool result;
	
	// Render the model using the texture shader.
	result = _TextureShader->Render(device, indexCount, worldMatrix, viewMatrix, projectionMatrix, texture);
	if (!result)
	{
		return false;
	}

	return true;
}

bool ShaderManagerClass::RenderLightShader(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix,
	XMMATRIX projectionMatrix, Material* material, ID3D11ShaderResourceView** textureArray, XMFLOAT3 lightDirection, XMFLOAT4 ambientColor, XMFLOAT4 diffuseColor,
	XMFLOAT3 cameraPosition, XMFLOAT4 specularColor, float specularPower, float fogStart, float fogEnd, XMFLOAT4 clipPlane, float translation, float transparency/*,
	ID3D11ShaderResourceView* reflectionTexture, XMMATRIX reflectionMatrix*/)
{
	// Render the model using the light shader.
	bool result = _LightShader->Render(deviceContext, indexCount, worldMatrix, viewMatrix,
		projectionMatrix, textureArray, lightDirection, ambientColor, diffuseColor,
		cameraPosition, specularColor, specularPower, fogStart, fogEnd, clipPlane, translation, transparency/*,
		reflectionTexture, reflectionMatrix*/); // @TODO: REMOVE REFLECTION FROM INSIDE HERE!!!!!! This is just for lighting effects
	if (!result)
	{
		return false;
	}

	return true;
}

bool ShaderManagerClass::RenderFontShader(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix,
	XMMATRIX projectionMatrix, ID3D11ShaderResourceView* texture, XMFLOAT4 pixelColor)
{
	bool result;

	// Render the model using the bump map shader. // @TODO: also make inheritance hierarchy for SHADER CLASSES
	result = _FontShader->Render(deviceContext, indexCount, worldMatrix, viewMatrix, projectionMatrix, texture, pixelColor);
	if (!result)
	{
		return false;
	}

	return true;
}

bool ShaderManagerClass::RenderReflectionShader(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix,
	XMMATRIX projectionMatrix, ID3D11ShaderResourceView* texture, ID3D11ShaderResourceView* reflectionTexture, XMMATRIX reflectionMatrix)
{
	bool result;

	// Render the model using the reflection shader. 
	result = _ReflectionShader->Render(deviceContext, indexCount, worldMatrix, viewMatrix, projectionMatrix, texture, reflectionTexture, reflectionMatrix);
	if (!result)
	{
		return false;
	}

	return true;
}

bool ShaderManagerClass::RenderWaterShader(ID3D11DeviceContext * deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, XMMATRIX reflectionMatrix, ID3D11ShaderResourceView** textureArray, /*ID3D11ShaderResourceView * reflectionTexture, ID3D11ShaderResourceView * refractionTexture, ID3D11ShaderResourceView * normalTexture, */float waterTranslation, float reflectRefractScale)
{
	bool result;

	// Render the model using the reflection shader. 
	result = _WaterShader->Render(deviceContext, indexCount, worldMatrix, viewMatrix, projectionMatrix, reflectionMatrix, textureArray, /*reflectionTexture, refractionTexture, normalTexture, */waterTranslation, reflectRefractScale);
	if (!result)
	{
		return false;
	}

	return true;
}

bool ShaderManagerClass::RenderRefractionShader(ID3D11DeviceContext * deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, ID3D11ShaderResourceView * texture, XMFLOAT3 lightDirection, XMFLOAT4 ambientColor, XMFLOAT4 diffuseColor, XMFLOAT4 clipPlane)
{
	bool result;

	// Render the model using the reflection shader. 
	result = _RefractionShader->Render(deviceContext, indexCount, worldMatrix, viewMatrix, projectionMatrix, texture, lightDirection, ambientColor, diffuseColor, clipPlane);
	if (!result)
	{
		return false;
	}

	return true;
}

FontShaderClass * ShaderManagerClass::GetFontShader()
{
	if (_FontShader)
	return _FontShader.get();

	return nullptr;
}
