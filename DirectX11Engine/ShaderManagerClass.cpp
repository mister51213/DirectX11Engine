////////////////////////////////////////////////////////////////////////////////
// Filename: shadermanagerclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "shadermanagerclass.h"

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

	_DiffuseShader.reset(new DiffuseShaderClass);
	if (!_DiffuseShader)
	{
		return false;
	}

	//result = _RefractionShader->Initialize(device, hwnd, "../DirectX11Engine/Refraction_vs.hlsl", "../DirectX11Engine/Refraction_ps.hlsl");
	//result = _RefractionShader->Initialize(device, hwnd, "../x64/Debug/Refraction_vs.cso", "../x64/Debug/Refraction_ps.cso");
	result = _DiffuseShader->Initialize(device, hwnd, "Diffuse_vs.cso", "Diffuse_ps.cso");
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

	result = _DepthShader->Initialize(device, hwnd, "depth_vs.cso", "depth_ps.cso");
	if (!result)
	{
		throw std::runtime_error("Could not initialize the depth shader object. - " + to_string(__LINE__));

		return false;
	}

	// Multi Shadow Shader
	_ShadowShaderMulti.reset(new ShadowShaderClass_Multi);
	if (!_ShadowShaderMulti)
	{
		return false;
	}

	result = _ShadowShaderMulti->InitializeShader(device, hwnd, L"MultiShadows_vs.cso", L"MultiShadows_ps.cso");
	if (!result)
	{
		throw std::runtime_error("Could not initialize the multi shadow shader object. - " + to_string(__LINE__));

		return false;
	}

	// Soft Shadow Shader
	_ShadowShaderSoft.reset(new SoftShadowShaderClass_Multi);
	if (!_ShadowShaderSoft)
	{
		return false;
	}

	result = _ShadowShaderSoft->InitializeShader(device, hwnd, L"Light_SoftShadows_vs.cso", L"Light_SoftShadows_ps.cso");
	if (!result)
	{
		throw std::runtime_error("Could not initialize the soft shadow shader object. - " + to_string(__LINE__));

		return false;
	}

	return true;
}

// @TODO the params at the end need to be encapsulated
bool ShaderManagerClass::Render(
	ID3D11DeviceContext * device, int indexCount, 
	MatrixBufferType& transforms,
	Material * material,
	LightClass* lights[], 
	SceneEffects& effects, 
	XMFLOAT3 cameraPos, 
	EShaderType inShaderType,
	XMMATRIX reflectionMatrix)
{
	bool result;

	// Allow caller of this function to specify an override shader type to use
	EShaderType shaderToUse = inShaderType;
	if (shaderToUse == EShaderType::EMATERIAL_DEFAULT)
	{
		shaderToUse = material->shaderType;
	}

	switch (shaderToUse)
	{
	case EShaderType::ETEXTURE:
		if(!_TextureShader->SetShaderParameters(device, transforms, material->GetTextureObject()->_textureViews, material->translation2D, material->textureScale, material->gamma))
		ThrowRuntime("Could not render the texture shader.");
		_TextureShader->RenderShader(device, indexCount);

		break;

	case EShaderType::ELIGHT_SPECULAR:
		ThrowRuntime("UNDER CONSTRUCTION! ~ currently calling render functions directly for light shader");

		if(!_LightShader->SetShaderParameters(device, transforms, material->GetResourceArray(), material->GetTextureObject()->_textureViews,
			effects.ambientColor, lights, cameraPos, effects.fogStart, effects.fogEnd, material->translation, material->transparency))
		ThrowRuntime("Could not render the light shader.");
		_LightShader->RenderShader(device, indexCount);

		break;

	case EShaderType::EREFLECTION:
		ThrowRuntime("UNDER CONSTRUCTION ~ reflection shader currently not working. Need to refactor and pass its reflection texture in the texture array now");

		//if(!_ReflectionShader->SetShaderParameters(device, worldMatrix, viewMatrix, projectionMatrix, material->GetResourceArray()[0], material->GetTextureObject()->_textureViews, reflectionTexture, reflectionMatrix))
		//ThrowRuntime("Could not render the reflection shader.");
		//_ReflectionShader->RenderShader(device, indexCount);
		break;

	case EShaderType::ELIGHT_DIFFUSE:
		if(!_DiffuseShader->SetShaderParameters(device, transforms, material->GetResourceArray()[0], material->GetTextureObject()->_textureViews,
			lights[0]->GetDirection(), effects.ambientColor, lights[0]->GetDiffuseColor(), effects.clipPlane))
		ThrowRuntime("Could not render the diffuse shader.");
		_DiffuseShader->RenderShader(device, indexCount);

		break;

	case EShaderType::EWATER:
		if(!_WaterShader->SetShaderParameters(device, transforms, reflectionMatrix, material->GetResourceArray(),
			material->GetTextureObject()->_textureViews, material->translation, material->reflectRefractScale, material->waterLerpRatio))
		ThrowRuntime("Could not render the water shader.");
		_WaterShader->RenderShader(device, indexCount);

		break;

	case EShaderType::EFONT:
		if(!_FontShader->SetShaderParameters(device, transforms, material->GetResourceArray()[0], material->GetTextureObject()->_textureViews, material->pixelColor))
		ThrowRuntime("Could not render the font shader.");
		_FontShader->RenderShader(device, indexCount);

		break;

	case EShaderType::EDEPTH:
		if(!_DepthShader->SetShaderParameters(device, transforms))
		ThrowRuntime("Could not render the depth shader.");
		_DepthShader->RenderShader(device, indexCount);

		break;

	default:
		if(!_TextureShader->SetShaderParameters(device, transforms,	material->GetTextureObject()->_textureViews, material->translation2D, material->textureScale))
		ThrowRuntime("Could not render the texture shader.");
		_TextureShader->RenderShader(device, indexCount);

		break;
	}
	return true;
}

FontShaderClass * ShaderManagerClass::GetFontShader() const
{
	if (_FontShader)
	{
		return _FontShader.get();
	}

	return nullptr;
}

bool ShaderManagerClass::RenderFontShader(ID3D11DeviceContext* deviceContext, int indexCount, MatrixBufferType transforms, ID3D11ShaderResourceView* texture, 
	vector<Microsoft::WRL::ComPtr <ID3D11ShaderResourceView>>& texViews, XMFLOAT4 pixelColor)
{
	if (!_FontShader->/*Render*/SetShaderParameters(deviceContext, /*indexCount, */transforms, texture, texViews, pixelColor))
	{
		ThrowRuntime("Could not initialize the font shader object.");
	}

	_FontShader->RenderShader(deviceContext, indexCount);

	return true;
}