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

// Calculates spot lights, based on position and direction of the light and of the direction of the surface.
float CalculateSpotLightIntensity(
	float3 LightPos_VertexSpace, 
	float3 LightDirection_WS, 
	float3 SurfaceNormal_WS)
{
	// CALCULATE SPOTLIGHT ATTENUATION
	float maxLightRange = 50.f;
	float dist = length(LightPos_VertexSpace);
	float attenuation = 1.f - (maxLightRange - dist) / maxLightRange;

	float3 lightToVertex_WS = -LightPos_VertexSpace;
	
	float dotProduct = saturate(dot(normalize(lightToVertex_WS), normalize(LightDirection_WS)));

	// METALLIC EFFECT (deactivate for now)
	float metalEffect = saturate(dot(SurfaceNormal_WS, normalize(LightPos_VertexSpace)));

	float dpCutOff = .95f;
	if(dotProduct > dpCutOff /*&& metalEffect > .55*/)
	{
		float expandedRange = (dotProduct - dpCutOff)/(1.f - dpCutOff);
		return saturate(dot(SurfaceNormal_WS, normalize(LightPos_VertexSpace))* expandedRange/**attenuation*/);
	}
	else
	{
		return 0;
	}
}

float4 main(PixelInputType input) : SV_TARGET
{
	float2 projectTexCoord;
	float depthValue;
	float lightDepthValue;
	float4 textureColor;

	/////////////////// NORMAL MAPPING //////////////////
	float4 bumpMap = shaderTextures[4].Sample(SampleType, input.tex);

	// Expand the range of the normal value from (0, +1) to (-1, +1).
    bumpMap = (bumpMap * 2.0f) - 1.0f;

	// Change the COORDINATE BASIS of the normal into the space represented by basis vectors tangent, binormal, and normal!
    float3 bumpNormal = normalize((bumpMap.x * input.tangent) + (bumpMap.y * input.binormal) + (bumpMap.z * input.normal));

	//////////////// AMBIENT BASE COLOR ////////////////
	// Set the default output color to the ambient light value for all pixels.
    //float4 lightColor = cb_ambientColor * saturate(dot(bumpNormal, input.normal) + .2);
    float4 lightColor = float4(0,0,0,0);

	// Calculate the amount of light on this pixel.
	for(int i = 0; i < NUM_LIGHTS; ++i)
	{
		float lightIntensity = saturate(dot(bumpNormal, normalize(input.lightPos_LS[i])));
		if(lightIntensity > 0.0f)
		{
		// Determine the final diffuse color based on the diffuse color and the amount of light intensity.
		lightIntensity = 
		CalculateSpotLightIntensity(input.lightPos_LS[i], cb_lights[i].lightDirection, bumpNormal);

		lightColor += (cb_lights[i].diffuseColor * lightIntensity) * 0.3;
		}
	}

    // Saturate the final light color.
    lightColor = saturate(lightColor);

	// Calculate the projected texture coordinates.
	projectTexCoord.x =  input.vertex_ScrnSpace.x / input.vertex_ScrnSpace.w / 2.0f + 0.5f;
	projectTexCoord.y = -input.vertex_ScrnSpace.y / input.vertex_ScrnSpace.w / 2.0f + 0.5f;

	// Sample the shadow value from the shadow texture using the sampler at the projected texture coordinate location.
	float shadowValue = shaderTextures[6].Sample(SampleTypeClamp, projectTexCoord).r;

	// TEXTURE ANIMATION -  Sample pixel color from texture at this texture coordinate location.
    input.tex.x += textureTranslation;

	// BLENDING
	float4 color1 = shaderTextures[0].Sample(SampleTypeWrap, input.tex);
    float4 color2 = shaderTextures[1].Sample(SampleTypeWrap, input.tex);
	float4 alphaValue = shaderTextures[3].Sample(SampleTypeWrap, input.tex);
	textureColor = saturate((alphaValue * color1) + ((1.0f - alphaValue) * color2));

	// Combine the light and texture color.
	float4 finalColor = lightColor * textureColor * shadowValue;
	//if(lightColor.x == 0)
	//{
	//	finalColor =  cb_ambientColor * saturate(dot(bumpNormal, input.normal) + .2) * textureColor;
	//}


	return finalColor;
}