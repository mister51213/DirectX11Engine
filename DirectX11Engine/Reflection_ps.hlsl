////////////////////////////////////////////////////////////////////////////////
// Filename: reflection.ps
////////////////////////////////////////////////////////////////////////////////

/////////////
// GLOBALS //
/////////////
Texture2D shaderTexture;
SamplerState SampleType;

Texture2D reflectionTexture;

//////////////
// TYPEDEFS //
//////////////
struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float4 reflectionPosition : TEXCOORD1;
};


////////////////////////////////////////////////////////////////////////////////
// Pixel Shader
////////////////////////////////////////////////////////////////////////////////
float4 ReflectionPixelShader(PixelInputType input) : SV_TARGET
{
    float4 textureColor;
    float2 reflectTexCoord;
    float4 reflectionColor;
    float4 color;
	
    // Sample the texture pixel at this location.
    textureColor = shaderTexture.Sample(SampleType, input.tex);

	// To convert input reflection position homogenous coordinates to proper texture coodrinates, first divide by the W coordinate. 
	// This leaves us with tu and tv coordinates in the -1 to +1 range. To fix it to map to a 0 to +1 range just divide by 2 and add 0.5.
    reflectTexCoord.x = input.reflectionPosition.x / input.reflectionPosition.w / 2.0f + 0.5f;
    reflectTexCoord.y = -input.reflectionPosition.y / input.reflectionPosition.w / 2.0f + 0.5f;

    // Sample the texture pixel from the reflection texture using the projected texture coordinates.
    reflectionColor = reflectionTexture.Sample(SampleType, reflectTexCoord);

    // Do a linear interpolation between floor and reflection texture.
    color = lerp(textureColor, reflectionColor, 0.15f);

    return color;
}
