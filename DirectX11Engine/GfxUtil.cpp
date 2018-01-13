#include "GfxUtil.h"

GfxUtil::Material::Material()
	{
		shaderType = ETEXTURE;
		texArraySize = 1;
	}

GfxUtil::Material::Material(EShaderType inShaderType, int numRenderTex = 0)
{
	shaderType = inShaderType;
	texArraySize = 1;
	numRenderTextures = numRenderTex;
}

bool GfxUtil::Material::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, EShaderType inShaderType, vector<string> texFileNames)
{
	texArraySize = texFileNames.size();

	shaderType = inShaderType;

	_TextureArray.reset(new TextureClass);
	if (!_TextureArray)
	{
		return false;
	}

	bool result = _TextureArray->InitializeArray(device, deviceContext, texFileNames);
	if (!result)
	{
		return false;
	}

	return true;
}

ID3D11ShaderResourceView* GfxUtil::Material::AddRenderTexture(ID3D11Device* const device, const int textureWidth, const int textureHeight, const float screenDepth, const float screenNear)
{
	// Add a new EMPTY resource view to the texture array object
	_TextureArray->_textureViews.push_back(Microsoft::WRL::ComPtr <ID3D11ShaderResourceView>());

	// Create a new render texture and push it back into the render texture array in this material
	_RenderTextures.push_back(unique_ptr<RenderTextureClass>(new RenderTextureClass(device, textureWidth, textureHeight, screenDepth, screenNear)));
	
	// Point the newly added resource view inside the Texture Array owned by this material at the render texture we just created
	_TextureArray->GetTextureArray()[_TextureArray->_textureViews.size()-1] = _RenderTextures[_RenderTextures.size() - 1]->GetShaderResourceView();

	return _RenderTextures[_RenderTextures.size() - 1]->GetShaderResourceView();
}


TextureClass * GfxUtil::Material::GetTextureObject()
{
	return _TextureArray.get();
}

ID3D11ShaderResourceView** GfxUtil::Material::GetResourceArray()
{
	if (_TextureArray)
	{
		return _TextureArray->GetTextureArray();
	}
}

void GfxUtil::Material::Animate(float deltaTime)
{
	if (bAnimated)
	{
		translation += 0.001f * deltaTime;
		if (translation > 1.0f)
		{
			translation -= 1.0f;
		}
	}
}

/////////// MATRIX MATH /////////////////
XMMATRIX GfxUtil::ComputeWorldTransform(XMFLOAT3& rotation, XMFLOAT3& scale, XMFLOAT3& translation)
{
	XMMATRIX rot = XMMatrixRotationRollPitchYaw(rotation.x*DEGTORAD, rotation.y*DEGTORAD, rotation.z*DEGTORAD);
	XMMATRIX scal = XMMatrixScaling(scale.x, scale.y, scale.z);
	XMMATRIX trans = XMMatrixTranslation(translation.x, translation.y, translation.z);
	return rot*scal*trans;
}