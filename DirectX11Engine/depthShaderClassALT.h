//////////////////////////////////////////////////////////////////////////////////
//// Filename: depthShaderClassALT.h
//////////////////////////////////////////////////////////////////////////////////
//#ifndef _depthShaderClassALT_H_
//#define _depthShaderClassALT_H_
//
//#pragma comment ( lib, "d3dcompiler.lib" )
//#pragma comment ( lib, "d3d11.lib" )
//
////////////////
//// INCLUDES //
////////////////
//#include <d3dcompiler.h>
//#include <d3d11.h>
//#include <DirectXMath.h>
////#include <d3dx11async.h>
//#include <fstream>
//using namespace std;
//using namespace DirectX;
//
//////////////////////////////////////////////////////////////////////////////////
//// Class name: depthShaderClassALT
//////////////////////////////////////////////////////////////////////////////////
//class depthShaderClassALT
//{
//private:
//	struct MatrixBufferType
//	{
//		XMMATRIX world;
//		XMMATRIX view;
//		XMMATRIX projection;
//	};
//
//public:
//	depthShaderClassALT();
//	depthShaderClassALT(const depthShaderClassALT&);
//	~depthShaderClassALT();
//
//	bool Initialize(ID3D11Device*, HWND);
//	void Shutdown();
//	bool Render(ID3D11DeviceContext*, int, XMMATRIX, XMMATRIX, XMMATRIX);
//
//private:
//	bool InitializeShader(ID3D11Device*, HWND, WCHAR*, WCHAR*);
//	void ShutdownShader();
//	void OutputShaderErrorMessage(ID3D10Blob*, HWND, WCHAR*);
//
//	bool SetShaderParameters(ID3D11DeviceContext*, XMMATRIX, XMMATRIX, XMMATRIX);
//	void RenderShader(ID3D11DeviceContext*, int);
//
//private:
//	ID3D11VertexShader* m_vertexShader;
//	ID3D11PixelShader* m_pixelShader;
//	ID3D11InputLayout* m_layout;
//	ID3D11Buffer* m_matrixBuffer;
//};
//
//#endif