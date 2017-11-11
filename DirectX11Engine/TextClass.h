////////////////////////////////////////////////////////////////////////////////
// Filename: textclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _TEXTCLASS_H_
#define _TEXTCLASS_H_


//////////////
// INCLUDES //
//////////////
#include "fontclass.h"
#include "fontshaderclass.h"

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
	void Shutdown();
	void Render(ID3D11DeviceContext*, XMMATRIX, XMMATRIX, XMMATRIX, ID3D11ShaderResourceView*);

	bool UpdateSentence(ID3D11DeviceContext*, FontClass*, char*, int, int, float, float, float);

private:
	bool InitializeSentence(ID3D11Device*, ID3D11DeviceContext*, FontClass*, char*, int, int, float, float, float);
	void RenderSentence(ID3D11DeviceContext*, XMMATRIX, XMMATRIX, XMMATRIX, ID3D11ShaderResourceView*);

private:
	FontShaderClass* _FontShader;
	ID3D11Buffer *m_vertexBuffer, *m_indexBuffer, *m_vertexBuffer2, *m_indexBuffer2;
	int m_screenWidth, m_screenHeight, m_maxLength, m_vertexCount, m_indexCount;
	//bool m_shadow;
	XMFLOAT4 m_pixelColor;
};

#endif

//////////////////////////////////////////////////////////////////////////////////
//// Filename: textclass.h
//////////////////////////////////////////////////////////////////////////////////
//#pragma once
//
/////////////////////////
//// MY CLASS INCLUDES //
/////////////////////////
//#include <DirectXMath.h>
//#include "fontclass.h"
//#include "fontshaderclass.h"
//
//using namespace DirectX;
//
//////////////////////////////////////////////////////////////////////////////////
//// Class name: TextClass
//////////////////////////////////////////////////////////////////////////////////
//class TextClass
//{
//private:
//	struct SentenceType
//	{
//		ID3D11Buffer *vertexBuffer, *indexBuffer;
//		int vertexCount, indexCount, maxLength;
//		float red, green, blue;
//	};
//
//	struct VertexType
//	{
//		XMFLOAT3 position;
//		XMFLOAT2 texture;
//	};
//
//public:
//	TextClass();
//	TextClass(const TextClass&);
//	~TextClass();
//
//	bool Initialize(ID3D11Device*, ID3D11DeviceContext*, HWND, int, int, XMMATRIX);
//	void Shutdown();
//	bool Render(ID3D11DeviceContext*, XMMATRIX, XMMATRIX);
//
//	bool SetRenderCount(int, ID3D11DeviceContext*);
//	bool SetMousePosition(int, int, ID3D11DeviceContext*);
//	bool SetFps(int, ID3D11DeviceContext*);
//	bool SetCpu(int, ID3D11DeviceContext*);
//
//private:
//	bool InitializeSentence(SentenceType**, int, ID3D11Device*);
//	bool UpdateSentence(SentenceType*, char*, int, int, float, float, float, ID3D11DeviceContext*);
//	void ReleaseSentence(SentenceType**);
//	bool RenderSentence(ID3D11DeviceContext*, SentenceType*, XMMATRIX, XMMATRIX);
//
//private:
//	FontClass* _Font;
//	FontShaderClass* _FontShader;
//	int _screenWidth, _screenHeight;
//	XMMATRIX _baseViewMatrix;
//	SentenceType* _sentence1;
//	SentenceType* _sentence2;
//};