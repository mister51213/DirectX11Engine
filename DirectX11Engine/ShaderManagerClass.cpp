////////////////////////////////////////////////////////////////////////////////
// Filename: shadermanagerclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "shadermanagerclass.h"

ShaderManagerClass::ShaderManagerClass()
	:
	_TextureShader(0),
	_LightShader(0),
	_FontShader(0)
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
	_TextureShader = new TextureShaderClass;
	if (!_TextureShader)
	{
		return false;
	}

	// Initialize the texture shader object.
	result = _TextureShader->Initialize(device, hwnd, L"../DirectX11Engine/texture_vs.hlsl", L"../DirectX11Engine/texture_ps.hlsl");
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the texture shader object.", L"Error", MB_OK);
		return false;
	}

	// Create the light shader object.
	_LightShader = new LightShaderClass;
	if (!_LightShader)
	{
		return false;
	}

	// Initialize the light shader object.
	result = _LightShader->Initialize(device, hwnd, L"../DirectX11Engine/Light_vs.hlsl", L"../DirectX11Engine/Light_ps.hlsl");
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the light shader object.", L"Error", MB_OK);
		return false;
	}

	// Create the bump map shader object.
	_FontShader = new FontShaderClass;
	if (!_FontShader)
	{
		return false;
	}

	// Initialize the bump map shader object.
	result = _FontShader->Initialize(device, hwnd, L"../DirectX11Engine/font.vs", L"../DirectX11Engine/font.ps");
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the bump map shader object.", L"Error", MB_OK);
		return false;
	}

	// Create the bump map shader object.
	_ReflectionShader = new ReflectionShaderClass;
	if (!_FontShader)
	{
		return false;
	}

	// Initialize the bump map shader object.
	result = _ReflectionShader->Initialize(device, hwnd, L"../DirectX11Engine/Reflection_vs.hlsl", L"../DirectX11Engine/Reflection_ps.hlsl");
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the bump map shader object.", L"Error", MB_OK);
		return false;
	}

	_WaterShader = new WaterShaderClass;

	result = _WaterShader->Initialize(device, hwnd, L"../DirectX11Engine/WaterShader_vs.hlsl", L"../DirectX11Engine/WaterShader_ps.hlsl");
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the bump map shader object.", L"Error", MB_OK);
		return false;
	}

	_RefractionShader = new RefractionShaderClass;

	result = _RefractionShader->Initialize(device, hwnd, L"../DirectX11Engine/Refraction_vs.hlsl", L"../DirectX11Engine/Refraction_ps.hlsl");
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the bump map shader object.", L"Error", MB_OK);
		return false;
	}

	return true;
}

void ShaderManagerClass::Shutdown()
{
	if (_RefractionShader)
	{
		_RefractionShader->Shutdown();
		delete _RefractionShader;
		_RefractionShader = 0;
	}

	// Release water shader
	if (_WaterShader)
	{
		_WaterShader->Shutdown();
		delete _WaterShader;
		_WaterShader = 0;
	}

	// Release reflection shader
	if (_ReflectionShader)
	{
		_ReflectionShader->Shutdown();
		delete _ReflectionShader;
		_ReflectionShader = 0;
	}

	// Release the bump map shader object.
	if (_FontShader)
	{
		_FontShader->Shutdown();
		delete _FontShader;
		_FontShader = 0;
	}

	// Release the light shader object.
	if (_LightShader)
	{
		_LightShader->Shutdown();
		delete _LightShader;
		_LightShader = 0;
	}

	// Release the texture shader object.
	if (_TextureShader)
	{
		_TextureShader->Shutdown();
		delete _TextureShader;
		_TextureShader = 0;
	}

	return;
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
	XMMATRIX projectionMatrix, ID3D11ShaderResourceView** textureArray, XMFLOAT3 lightDirection, XMFLOAT4 ambientColor, XMFLOAT4 diffuseColor,
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

bool ShaderManagerClass::RenderWaterShader(ID3D11DeviceContext * deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, XMMATRIX reflectionMatrix, ID3D11ShaderResourceView * reflectionTexture, ID3D11ShaderResourceView * refractionTexture, ID3D11ShaderResourceView * normalTexture, float waterTranslation, float reflectRefractScale)
{
	bool result;

	// Render the model using the reflection shader. 
	result = _WaterShader->Render(deviceContext, indexCount, worldMatrix, viewMatrix, projectionMatrix, reflectionMatrix, reflectionTexture, refractionTexture, normalTexture, waterTranslation, reflectRefractScale);
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
	return _FontShader;

	return nullptr;
}
