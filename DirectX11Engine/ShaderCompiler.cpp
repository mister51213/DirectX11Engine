#include "ShaderCompiler.h"
#include <d3dcompiler.h>
#include <fstream>

#pragma comment ( lib, "d3dcompiler.lib" )

ComPtr<ID3DBlob> ShaderCompiler::CompileShader( 
	std::string EntryPoint,
	std::string TargetShaderVersion,
	std::string Filename,
	ID3D11Device* pDevice,
	ID3D10Blob* defaultMessage) const
{
	const auto sFilenameW = ConvertTo<std::string, std::wstring>( Filename );	
	ComPtr<ID3D10Blob> sBlob, errorMessage;

	HRESULT result = D3DCompileFromFile(
		sFilenameW.c_str(),
		nullptr,
		nullptr,
		EntryPoint.c_str(),
		TargetShaderVersion.c_str(),
		D3DCOMPILE_ENABLE_STRICTNESS,
		0,
		&sBlob,
		&errorMessage );
	if( FAILED( result ) )
	{
		// If the shader failed to compile it should have writen something to the error message.
		if( errorMessage )
		{
			OutputErrorMessage( Filename, errorMessage );
		}
		// If there was nothing in the error message then it simply could not find the shader file itself.
		else
		{
			throw MissingShaderException( Filename );
		}
	}

	return sBlob;
}

void ShaderCompiler::OutputErrorMessage( const std::string & Filename, ComPtr<ID3DBlob>& ErrorMessage ) const
{
	constexpr auto errorFilename = "shader-error.txt";

	// Get a pointer to the error message text buffer.
	const std::string compileErrors = ( char* )( ErrorMessage->GetBufferPointer() );

	// Open a file to write the error message to.
	ofstream fout( errorFilename );

	// Write out the error message.
	fout << compileErrors;

	// Close the file.
	fout.close();

	// Pop a message up on the screen to notify the user to check the text file for compile errors.
	throw CompileShaderException( Filename, compileErrors, errorFilename );
}
