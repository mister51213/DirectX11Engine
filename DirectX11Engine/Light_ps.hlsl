////////////////////////////////////////////////////////////////////////////////
// Filename: light_ps
// The pixel shader gets its input from the vertex shader output.
// Msdn note: Pixel shaders can only write to parameters with the 
// SV_Depth and SV_Target system-value semantics.
////////////////////////////////////////////////////////////////////////////////

/////////////
// GLOBALS //
/////////////
// texture resource that will be used for rendering the texture on the model
//Texture2D shaderTexture;
Texture2D shaderTextures[6];
// allows modifying how pixels are written to the polygon face, for example choosing which to draw. 
SamplerState SampleType;

Texture2D reflectionTexture;

//////////////////////
// CONSTANT BUFFERS //
//////////////////////
// hold diffuse color and direction of light
cbuffer LightBuffer //@TODO: register w same number as in class
{
    float4 ambientColor;
    float4 diffuseColor;
    float3 lightDirection;
    float specularPower;
    float4 specularColor;
};

// value set here will be between 0 and 1.
cbuffer TranslationBuffer //@TODO: register w same number as in class
{
    float textureTranslation; //@NOTE = hlsl automatically pads floats for you
};

// for alpha blending textures
cbuffer TransparentBuffer:register(b4)
{
    float blendAmount;
};

//////////////
// TYPEDEFS //
//////////////
struct PixelInputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
	float3 tangent : TANGENT;
    float3 binormal : BINORMAL;
    float3 viewDirection : TEXCOORD1;
	float fogFactor : FOG;
	float clip : SV_ClipDistance0;
    float4 reflectionPosition : TEXCOORD2;
};

////////////////////////////////////////////////////////////////////////////////
// Pixel Shader
////////////////////////////////////////////////////////////////////////////////
float4 LightPixelShader(PixelInputType input) : SV_TARGET
{
	//////////// INIT VALUES ////////////////
	// multi texturing data
    float4 color1;
    float4 color2;
    float4 lightColor; //light map
	float4 alphaValue;
    float4 fogColor = float4(0.5f, 0.3f, 0.3f, 1.0f);
    
	// computed blend of textures
	float4 blendColor;

	// lighting data
    float4 textureColor;
    float3 lightDir;
    float lightIntensity;
    float3 reflection;
    float4 specular;
	float4 specularIntensity;
    float4 color;

    float4 bumpMap;
    float3 bumpNormal;

	float gamma = 1.5f;

	//////////// REFLECTION /////////////////////
	float2 reflectTexCoord;
    float4 reflectionColor;

	// Translate the position where we sample the pixel from.
    input.tex.x += textureTranslation;

    // Sample the pixel color from the textures using the sampler at this texture coordinate location.
	color1 = shaderTextures[0].Sample(SampleType, input.tex);

    // Get the pixel color from the second texture.
    color2 = shaderTextures[1].Sample(SampleType, input.tex);

	// Apply lightmap
	lightColor = shaderTextures[2].Sample(SampleType, input.tex);

	// Apply alpha
	alphaValue = shaderTextures[3].Sample(SampleType, input.tex);

	// Get normal value from the bump map
	bumpMap = shaderTextures[4].Sample(SampleType, input.tex);


	/////////////////// NORMAL MAPPING //////////////////
	// Expand the range of the normal value from (0, +1) to (-1, +1).
    bumpMap = (bumpMap * 2.0f) - 1.0f;

	// Change the COORDINATE BASIS of the normal into the space represented by basis vectors tangent, binormal, and normal!
    bumpNormal = (bumpMap.x * input.tangent) + (bumpMap.y * input.binormal) + (bumpMap.z * input.normal);

	// Normalize the resulting bump normal.
    bumpNormal = normalize(bumpNormal);


	/////////////////// BLENDING /////////////////////////
    // Blend the two pixels together and multiply by the gamma value.
    blendColor = (alphaValue * color1) + ((1.0f - alphaValue) * color2) * /*lightColor **/ gamma;
    
    // Saturate the final color.
    blendColor = saturate(blendColor);

    // Set the default output color to the ambient light value for all pixels.
    color = ambientColor;

    // Initialize the specular color.
    specular = float4(0.0f, 0.0f, 0.0f, 0.0f);

	// Invert the light direction for calculations.
    lightDir = -lightDirection;

    // Calculate the amount of light on this pixel.
    //lightIntensity = saturate(dot(input.normal, lightDir));
    lightIntensity = saturate(dot(bumpNormal, lightDir));

	if(lightIntensity > 0.0f)
    {
	    // Sample the pixel from the specular map texture.
        specularIntensity = shaderTextures[5].Sample(SampleType, input.tex);

        // Determine the final diffuse color based on the diffuse color and the amount of light intensity.
        color += (diffuseColor * lightIntensity);

        // Saturate the ambient and diffuse color.
        color = saturate(color);

		// Calculate the reflection vector based on the light intensity, normal vector, and light direction.
        reflection = normalize(2 * lightIntensity * /*input.normal*/bumpNormal - lightDir); 

        // Determine the amount of specular light based on the reflection vector, viewing direction, and specular power.
        specular = pow(saturate(dot(reflection, input.viewDirection)), specularPower);

		// Use the specular map to determine the intensity of specular light at this pixel.
        specular = specular * specularIntensity;
    }

    // Multiply the texture pixel and the final diffuse color to get the final pixel color result.
    color = color * blendColor; //textureColor;

	/////////////////////////////////////////////////////////////////////
	//@TODO: FOG FOG FOG FOG FOG FOG - experiment with calling this in different places
	// Calculate the final color using the fog effect equation.
    //color = input.fogFactor * color + (1.0 - input.fogFactor) * fogColor;
	//@TODO:  FOG FOG FOG FOG FOG FOG
	/////////////////////////////////////////////////////////////////////////

    // Saturate the final light color.
    color = saturate(color + specular);
	//float shiny = color + specular;
	//float matte = color;
    //color = saturate((alphaValue * matte) + ((1.0f - alphaValue) * shiny));

	/////////////// TRANSPARENCY /////////////////////
    // Set the alpha value of this pixel to the blending amount to create the alpha blending effect.
    color.a = blendAmount;

	/////////////// REFLECTION ///////////////////////
	// Calculate the projected reflection texture coordinates.
    reflectTexCoord.x = input.reflectionPosition.x / input.reflectionPosition.w / 2.0f + 0.5f;
    reflectTexCoord.y = -input.reflectionPosition.y / input.reflectionPosition.w / 2.0f + 0.5f;

	// Sample the texture pixel from the reflection texture using the projected texture coordinates.
    reflectionColor = reflectionTexture.Sample(SampleType, reflectTexCoord);

	// Do a linear interpolation between the two textures for a blend effect.
    color = lerp(color, reflectionColor, 0.15f);

    return color;
}