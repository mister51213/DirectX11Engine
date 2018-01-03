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
	//result = _TextureShader->Initialize(device, hwnd, "../DirectX11Engine/texture_vs.hlsl", "../DirectX11Engine/texture_ps.hlsl");
	//result = _TextureShader->Initialize(device, hwnd, "../x64/Debug/texture_vs.cso", "../x64/Debug/texture_ps.cso");
	result = _TextureShader->Initialize(device, hwnd, "texture_vs.cso", "texture_ps.cso");
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
	//result = _LightShader->Initialize(device, hwnd, "../DirectX11Engine/Light_vs.hlsl", "../DirectX11Engine/Light_ps.hlsl");
	//result = _LightShader->Initialize(device, hwnd, "../x64/Debug/Light_vs.cso", "../x64/Debug/Light_ps.cso");
	result = _LightShader->Initialize(device, hwnd, "Light_vs.cso", "Light_ps.cso");
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
	//result = _FontShader->Initialize(device, hwnd, "../DirectX11Engine/font.vs", "../DirectX11Engine/font.ps");
	//result = _FontShader->Initialize(device, hwnd, "../x64/Debug/font_vs.cso", "../x64/Debug/font_ps.cso");
	result = _FontShader->Initialize(device, hwnd, "font_vs.cso", "font_ps.cso");
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
	//result = _ReflectionShader->Initialize(device, hwnd, "../DirectX11Engine/Reflection_vs.hlsl", "../DirectX11Engine/Reflection_ps.hlsl");
	//result = _ReflectionShader->Initialize(device, hwnd, "../x64/Debug/Reflection_vs.cso", "../x64/Debug/Reflection_ps.cso");
	result = _ReflectionShader->Initialize(device, hwnd, "Reflection_vs.cso", "Reflection_ps.cso");
	if (!result)
	{
		throw std::runtime_error("Could not initialize the reflection shader object. - " + to_string(__LINE__));

		return false;
	}

	_WaterShader.reset(new WaterShaderClass);
	if (!_WaterShader)
	{
		return false;
	}

	//result = _WaterShader->Initialize(device, hwnd, "../DirectX11Engine/WaterShader_vs.hlsl", "../DirectX11Engine/WaterShader_ps.hlsl");
	//result = _WaterShader->Initialize(device, hwnd, "../x64/Debug/WaterShader_vs.cso", "../x64/Debug/WaterShader_ps.cso");
	result = _WaterShader->Initialize(device, hwnd, "WaterShader_vs.cso", "WaterShader_ps.cso");
	if (!result)
	{
		throw std::runtime_error("Could not initialize the water shader object. - " + to_string(__LINE__));

		return false;
	}

	_RefractionShader.reset(new RefractionShaderClass);
	if (!_RefractionShader)
	{
		return false;
	}

	//result = _RefractionShader->Initialize(device, hwnd, "../DirectX11Engine/Refraction_vs.hlsl", "../DirectX11Engine/Refraction_ps.hlsl");
	//result = _RefractionShader->Initialize(device, hwnd, "../x64/Debug/Refraction_vs.cso", "../x64/Debug/Refraction_ps.cso");
	result = _RefractionShader->Initialize(device, hwnd, "Refraction_vs.cso", "Refraction_ps.cso");
	if (!result)
	{
		throw std::runtime_error("Could not initialize the refraction shader object. - " + to_string(__LINE__));

		return false;
	}

	_DepthShader.reset(new DepthShaderClass);
	if (!_DepthShader)
	{
		return false;
	}

	//result = _DepthShader->Initialize(device, hwnd, "../DirectX11Engine/depth.vs", "../DirectX11Engine/depth.ps");
	//result = _DepthShader->Initialize(device, hwnd, "../x64/Debug/depth_vs.cso", "../x64/Debug/depth_ps.cso");
	result = _DepthShader->Initialize(device, hwnd, "depth_vs.cso", "depth_ps.cso");
	if (!result)
	{
		throw std::runtime_error("Could not initialize the depth shader object. - " + to_string(__LINE__));

		return false;
	}

	return true;
}

// @TODO the params at teh end need to be encapsulated
bool ShaderManagerClass::Render(ID3D11DeviceContext * device, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, Material * material,
	LightClass* lights[], SceneEffects& effects, XMFLOAT3 cameraPos, XMMATRIX reflectionMatrix, ID3D11ShaderResourceView * reflectionTexture, ID3D11ShaderResourceView * refractionTexture/*, ID3D11ShaderResourceView * normalTexture*/)
{
	bool result;

	switch (material->shaderType)
	{
	case EShaderType::ETEXTURE:
		result = _TextureShader->Render(device, indexCount, worldMatrix, viewMatrix, projectionMatrix, material->GetResourceArray()[0]);
		if (!result) return false;
		//_TextureShader->RenderShader(device, indexCount);
		break;

	case EShaderType::ELIGHT_SPECULAR:
		ThrowRuntime("UNDER CONSTRUCTION! ~ this switch case not yet working; currently calling _LightShader->Render() directly from Graphics.cpp!");
		//@TODO - not passing in right values - need to REWORK ShaderManagerClass::Render() params to do so!
		/*result = _LightShader->Render(device, indexCount, worldMatrix, viewMatrix, projectionMatrix,
			&light->GetViewMatrix(), 
			&light->GetProjectionMatrix(),
			material->GetResourceArray(), light->GetDirection(), light->GetAmbientColor(), light->GetDiffuseColor(), light->GetDiffuseColor(),
			lights, 
			cameraPos, light->GetSpecularColor(), light->GetSpecularPower(), effects.fogStart, effects.fogEnd, material->translation, material->transparency);*/	
		if (!result) return false;
		//_LightShader->RenderShader(device, indexCount);
		break;

	case EShaderType::EREFLECTION:
		result = _ReflectionShader->Render(device, indexCount, worldMatrix, viewMatrix, projectionMatrix,
			material->GetResourceArray()[0], reflectionTexture, reflectionMatrix);
		if (!result) return false;
		//_ReflectionShader->RenderShader(device, indexCount);
		break;

	case EShaderType::EREFRACTION:
		result = _RefractionShader->Render(device, indexCount, worldMatrix, viewMatrix, projectionMatrix,
			material->GetResourceArray()[0], lights[0]->GetDirection(), effects.ambientColor, lights[0]->GetDiffuseColor(), effects.clipPlane);
		if (!result) return false;
		//_RefractionShader->RenderShader(device, indexCount);
		break;

	case EShaderType::EWATER:
		result = _WaterShader->Render(device, indexCount, worldMatrix, viewMatrix, projectionMatrix, reflectionMatrix,
			material->GetResourceArray(), material->translation, material->reflectRefractScale);
		if (!result) return false;
	//	_WaterShader->RenderShader(device, indexCount);
		break;

	case EShaderType::EFONT:
		result = _FontShader->Render(device, indexCount, worldMatrix, viewMatrix, projectionMatrix,
			material->GetResourceArray()[0], material->pixelColor);
		if (!result) return false;
		//_FontShader->RenderShader(device, indexCount);
		break;

	case EShaderType::EDEPTH:
		result = _DepthShader->Render(device, indexCount, worldMatrix, viewMatrix, projectionMatrix);
		if (!result) return false;
		break;

	default:
		result = _TextureShader->Render(device, indexCount, worldMatrix, viewMatrix, projectionMatrix, material->GetResourceArray()[0]);
		if (!result) return false;
		//_TextureShader->RenderShader(device, indexCount);
		break;
	}
	return true;
}

FontShaderClass * ShaderManagerClass::GetFontShader()
{
	if (_FontShader)
	return _FontShader.get();

	return nullptr;
}

bool ShaderManagerClass::RenderFontShader(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix,
	XMMATRIX projectionMatrix, ID3D11ShaderResourceView* texture, XMFLOAT4 pixelColor)
{
	if (!_FontShader->Render(deviceContext, indexCount, worldMatrix, viewMatrix, projectionMatrix, texture, pixelColor))
	{
		ThrowRuntime("Could not initialize the font shader object.");
	}

	return true;
}