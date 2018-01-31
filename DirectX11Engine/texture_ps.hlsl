////////////////////////////////////////////////////////////////////////////////
// Filename: texture.ps
// The pixel shader gets its input from the vertex shader output.
// Msdn note: Pixel shaders can only write to parameters with the 
// SV_Depth and SV_Target system-value semantics.
////////////////////////////////////////////////////////////////////////////////

/////////////
// GLOBALS //
/////////////
// texture resource that will be used for rendering the texture on the model
//Texture2D shaderTexture;
Texture2D shaderTextures[2];
// allows modifying how pixels are written to the polygon face, for example choosing which to draw. 
SamplerState SampleType;

//////////////
// CBUFFERS //
//////////////
cbuffer TextureParams
{
	float2 translation;
	float scale;
	float padding;
};

//////////////
// TYPEDEFS //
//////////////
struct PixelInputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
};

// git test change

////////////////////////////////////////////////////////////////////////////////
// Pixel Shader
////////////////////////////////////////////////////////////////////////////////
float4 TexturePixelShader(PixelInputType input) : SV_TARGET
{
    float4 textureColor;
	float gamma = .5;

	// Move the position the texture is sampled from.	
    input.tex += translation;
	input.tex *= scale;

    // Sample the pixel color from the texture using the sampler at this texture coordinate location.
    textureColor = shaderTextures[0].Sample(SampleType, input.tex);
    //textureColor = shaderTextures[0].Sample(SampleType, input.tex*.5f) * gamma;

	float4 alpha = shaderTextures[1].Sample(SampleType, input.tex);
	textureColor.a = 1.f - alpha.x;
	 
    return textureColor;
}

//TODO: possibly remove hlsl extensions?