/////////////
// DEFINES //
/////////////
#define NUM_LIGHTS 3

/////////////
// GLOBALS //
/////////////
// texture resource that will be used for rendering the texture on the model
Texture2D shaderTextures[7];// NOTE - we only use one render target for drawing all the shadows here!
// allows modifying how pixels are written to the polygon face, for example choosing which to draw. 
SamplerState SampleType;

///////////////////
// SAMPLE STATES //
///////////////////
SamplerState SampleTypeClamp : register(s0);
SamplerState SampleTypeWrap  : register(s1);

///////////////////
// TYPEDEFS //
///////////////////

// This structure is used to describe the lights properties
struct LightTemplate_PS
{
	int type;
	float3 padding;
	float4 diffuseColor;
    float3 lightDirection; //(lookat?) //@TODO pass from VS BUFFER?
    float specularPower;
    float4 specularColor;
};

//////////////////////
// CONSTANT BUFFERS //
//////////////////////
cbuffer SceneLightBuffer:register(b0)
{
	float4 cb_ambientColor;
	LightTemplate_PS cb_lights[NUM_LIGHTS];
}

//////////////////////
// CONSTANT BUFFERS //
//////////////////////
// value set here will be between 0 and 1.
cbuffer TranslationBuffer:register(b1)
{
    float textureTranslation; //@NOTE = hlsl automatically pads floats for you
};

// for alpha blending textures
cbuffer TransparentBuffer:register(b2)
{
    float blendAmount;
};

// All in one tex param type
cbuffer TexParamBufferType:register(b3)
{
	float cb_texTrans;
	float cb_blendAmt;
	float cb_gamma;
	int cb_bBlendTex; // NOTE - 0 is false, 1 is true!
};

///////////////////////////
// INPUT FROM VERTEX SHADER
///////////////////////////

struct PixelInputType
{
	float4 vertex_ModelSpace : SV_POSITION;
    float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
    float3 binormal : BINORMAL;
	float3 viewDirection : TEXCOORD1;
    float3 lightPos_LS[NUM_LIGHTS] : TEXCOORD2;
    float4 vertex_ScrnSpace : TEXCOORD5;
};

float4 main(PixelInputType input) : SV_TARGET
{
	bool bInsideSpotlight = true;
	float2 projectTexCoord;
	float depthValue;
	float lightDepthValue;
	float4 textureColor;

	/////////////////// NORMAL MAPPING //////////////////
	float4 bumpMap = shaderTextures[4].Sample(SampleType, input.tex);

	// Sample the shadow value from the shadow texture using the sampler at the projected texture coordinate location.
	projectTexCoord.x =  input.vertex_ScrnSpace.x / input.vertex_ScrnSpace.w / 2.0f + 0.5f;
	projectTexCoord.y = -input.vertex_ScrnSpace.y / input.vertex_ScrnSpace.w / 2.0f + 0.5f;
	float shadowValue = shaderTextures[6].Sample(SampleTypeClamp, projectTexCoord).r;

	// Expand the range of the normal value from (0, +1) to (-1, +1).
    bumpMap = (bumpMap * 2.0f) - 1.0f;

	// Change the COORDINATE BASIS of the normal into the space represented by basis vectors tangent, binormal, and normal!
    float3 bumpNormal = normalize((bumpMap.x * input.tangent) + (bumpMap.y * input.binormal) + (bumpMap.z * input.normal));

	//////////////// AMBIENT BASE COLOR ////////////////
	// Set the default output color to the ambient light value for all pixels.
    float4 lightColor = cb_ambientColor * saturate(dot(bumpNormal, input.normal) + .2);

	// Calculate the amount of light on this pixel.
	for(int i = 0; i < NUM_LIGHTS; ++i)
	{
		float lightIntensity = saturate(dot(bumpNormal, normalize(input.lightPos_LS[i])));
		if(lightIntensity > 0.0f)
		{
		lightColor += (cb_lights[i].diffuseColor * lightIntensity) * 0.3;
		}
	}

    // Saturate the final light color.
    lightColor = saturate(lightColor);

    //lightColor = lightMapAddition;

	// TEXTURE ANIMATION -  Sample pixel color from texture at this texture coordinate location.
    input.tex.x += textureTranslation;

	// BLENDING
	float4 color1 = shaderTextures[0].Sample(SampleTypeWrap, input.tex);
    float4 color2 = shaderTextures[1].Sample(SampleTypeWrap, input.tex);
	float4 alphaValue = shaderTextures[3].Sample(SampleTypeWrap, input.tex);
	//textureColor = saturate((alphaValue * color1) + ((1.0f - alphaValue) * color2));
	textureColor = color1;

	// Add extra light map ontop of shadow value
	float4 lightMapValue = shaderTextures[2].Sample(SampleType, input.tex);
	float gammaToApply = (cb_gamma * lightMapValue.r) + 1.f;
	//float gammaToApply = (cb_gamma * (lightMapValue.r + 1.f)) - 1.f;

	// Combine the light and texture color.
	float4 finalColor = lightColor * textureColor * shadowValue * gammaToApply;//cb_gamma;

	//if(lightColor.x == 0)
	//{
	//	finalColor =  cb_ambientColor * saturate(dot(bumpNormal, input.normal) + .2) * textureColor;
	//}

	// Transparency
	finalColor.a = cb_blendAmt;

	return finalColor;
}