////////////////////////////////////////////////////////////////////////////////
// Filename: textureclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "textureclassALT.h"
#include "DDSTextureLoader.h"

using namespace DirectX;

textureClassALT::textureClassALT()
{
	//m_texture = 0;
}


textureClassALT::textureClassALT(const textureClassALT& other)
{
}


textureClassALT::~textureClassALT()
{
}


bool textureClassALT::Initialize(ID3D11Device* device, ID3D11DeviceContext* d3dContext, WCHAR* filename)
{
	HRESULT result;


	// Load the texture in.
	//result = D3DX11CreateShaderResourceViewFromFile(device, filename, NULL, NULL, &m_texture, NULL);

	result = CreateDDSTextureFromFile(device, d3dContext, filename, &m_texture, &m_textureView);
	if (FAILED(result))
	{
		return false;
	}

	return true;
}


void textureClassALT::Shutdown()
{
	//// Release the texture resource.
	//if(m_texture)
	//{
	//	m_texture->Release();
	//	m_texture = 0;
	//}

	return;
}


ID3D11ShaderResourceView* textureClassALT::GetTexture()
{
	return m_textureView.Get();
}