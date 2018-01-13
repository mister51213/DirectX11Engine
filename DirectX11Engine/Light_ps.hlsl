////////////////////////////////////////////////////////////////////////////////
// Filename: light_ps
// The pixel shader gets its input from the vertex shader output.
// Msdn note: Pixel shaders can only write to parameters with the 
// SV_Depth and SV_Target system-value semantics.
////////////////////////////////////////////////////////////////////////////////

/////////////
// DEFINES //
/////////////
#define NUM_LIGHTS 3

/////////////
// GLOBALS //
/////////////
// texture resource that will be used for rendering the texture on the model
Texture2D shaderTextures[9];
// allows modifying how pixels are written to the polygon face, for example choosing which to draw. 
SamplerState SampleType;

///////////////////
// SAMPLE STATES //
///////////////////
SamplerState SampleTypeClamp : register(s0);
SamplerState SampleTypeWrap  : register(s1);
SamplerComparisonState SampleTypeComp  : register(s2);

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
	//int cb_lightCount;
	//float3 cb_padding;
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

//////////////
// TYPEDEFS //
//////////////
struct PixelInputType
{
	float4 vertPos_ModelSpace : TEXCOORD12;
	float4 vertPos_ScrnSpace : SV_POSITION;
	float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
	float3 tangent : TANGENT;
    float3 binormal : BINORMAL;
    float3 viewDirection : TEXCOORD1;
    float4 vertex_ProjLightSpace[NUM_LIGHTS] : TEXCOORD6;
    float3 lightPos_LS[NUM_LIGHTS] : TEXCOORD9;
	float fogFactor : FOG;
};

float4 CalculateNormalMapIntensity(PixelInputType input, float4 diffuseColor, float3 lightDirection)
{
    float3 bumpNormal;

	// Get normal value from the bump map
	float4 bumpMap = shaderTextures[4].Sample(SampleType, input.tex);

	/////////////////// NORMAL MAPPING //////////////////
	// Expand the range of the normal value from (0, +1) to (-1, +1).
    bumpMap = (bumpMap * 2.0f) - 1.0f;

	// Change the COORDINATE BASIS of the normal into the space represented by basis vectors tangent, binormal, and normal!
    bumpNormal = normalize((bumpMap.x * input.tangent) + (bumpMap.y * input.binormal) + (bumpMap.z * input.normal));


    // Set the default output color to the ambient light value for all pixels.
    float lightColor = cb_ambientColor;

    // Initialize the specular color.
    float4 specular = float4(0.0f, 0.0f, 0.0f, 0.0f);

	// Invert the light direction for calculations.
    float3 lightDir = -lightDirection;

    // Calculate the amount of light on this pixel.
    float lightIntensity = saturate(dot(bumpNormal, lightDir));

	if(lightIntensity > 0.0f)
    {
        // Determine the final diffuse color based on the diffuse color and the amount of light intensity.
        lightColor = saturate(lightColor + (diffuseColor * lightIntensity));

		// Calculate the reflection vector based on the light intensity, normal vector, and light direction.
        float3 reflection = normalize(2 * lightIntensity * bumpNormal - lightDir); 

        // Determine the amount of specular light based on the reflection vector, viewing direction, and specular power.
        specular = pow(saturate(dot(reflection, input.viewDirection)), 5.f);
    }

    // Saturate the final light color.
   return saturate(lightColor + specular);
  // return lightColor;
}

float4 sphereMask(float2 tex)
{
	float alpha = 0;

	float radius = .4f;
	float2 offsetFromCenter = tex - float2(0.5f,0.5f);
	float dist = length(offsetFromCenter);
	
	if(dist <= radius)
	{
		alpha = (1-dist)*(1-dist);
	}

	return alpha;
}

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

	//float3 lightToVertex = normalize(SurfacePosition - LightPos_VertexSpace);
	float3 lightToVertex_WS = -LightPos_VertexSpace;
	
	float dotProduct = saturate(dot(normalize(lightToVertex_WS), normalize(LightDirection_WS)));

	// METALLIC EFFECT (deactivate for now)
	float metalEffect = saturate(dot(SurfaceNormal_WS, normalize(LightPos_VertexSpace)));

	//float dpCutOff = .77f;
	float dpCutOff = .95f;
	if(dotProduct > dpCutOff /*&& metalEffect > .55*/)
	{
		float expandedRange = (dotProduct - dpCutOff)/(1.f - dpCutOff);
		return saturate(dot(SurfaceNormal_WS, normalize(LightPos_VertexSpace))* expandedRange/**attenuation*/);
		//return saturate(dot(SurfaceNormal_WS, normalize(LightPos_VertexSpace)));
		//return dotProduct;
	}
	else
	{
		return 0;
	}
}

float4 LightPixelShader(PixelInputType input) : SV_TARGET
{
	float2 projectTexCoord;
	float depthValue;
	float lightDepthValue;
	float4 textureColor;

	// Set the bias value for fixing the floating point precision issues.
	float bias = 0.001f;

	/////////////////// NORMAL MAPPING //////////////////
	float4 bumpMap = shaderTextures[4].Sample(SampleType, input.tex);

	// Expand the range of the normal value from (0, +1) to (-1, +1).
    bumpMap = (bumpMap * 2.0f) - 1.0f;

	// Change the COORDINATE BASIS of the normal into the space represented by basis vectors tangent, binormal, and normal!
    float3 bumpNormal = normalize((bumpMap.x * input.tangent) + (bumpMap.y * input.binormal) + (bumpMap.z * input.normal));

	//////////////// AMBIENT BASE COLOR ////////////////
	// Set the default output color to the ambient light value for all pixels.
    float4 lightColor = cb_ambientColor* saturate(dot(bumpNormal, input.normal) + .35);

	//////////////// SHADOWING LOOP ////////////////
	for(int i = 0; i < NUM_LIGHTS; ++i)
	{
	// Calculate the projected texture coordinates.
	projectTexCoord.x =  input.vertex_ProjLightSpace[i].x / input.vertex_ProjLightSpace[i].w / 2.0f + 0.5f;
	projectTexCoord.y = -input.vertex_ProjLightSpace[i].y / input.vertex_ProjLightSpace[i].w / 2.0f + 0.5f;

	if((saturate(projectTexCoord.x) == projectTexCoord.x) && (saturate(projectTexCoord.y) == projectTexCoord.y))
	{
		// Sample the shadow map depth value from the depth texture using the sampler at the projected texture coordinate location.
		depthValue = shaderTextures[6 + i].Sample(SampleTypeClamp, projectTexCoord).r;

		// Calculate the depth of the light.
		lightDepthValue = input.vertex_ProjLightSpace[i].z / input.vertex_ProjLightSpace[i].w;

		// Subtract the bias from the lightDepthValue.
		lightDepthValue = lightDepthValue - bias;

		float lightVisibility = shaderTextures[6 + i].SampleCmp(SampleTypeComp, projectTexCoord, lightDepthValue );

		// Compare the depth of the shadow map value and the depth of the light to determine whether to shadow or to light this pixel.
		// If the light is in front of the object then light the pixel, if not then shadow this pixel since an object (occluder) is casting a shadow on it.
			if(lightDepthValue < depthValue)
			{
				// Calculate the amount of light on this pixel.
				float lightIntensity = saturate(dot(bumpNormal, normalize(input.lightPos_LS[i])));

				if(lightIntensity > 0.0f)
				{
					// Determine the final diffuse color based on the diffuse color and the amount of light intensity.
					float spotLightIntensity = CalculateSpotLightIntensity(
						input.lightPos_LS[i], // NOTE - this is NOT NORMALIZED!!!
						cb_lights[i].lightDirection, 
						bumpNormal/*input.normal*/);

					lightColor += cb_lights[i].diffuseColor*spotLightIntensity* .3f; // spotlight
					//lightColor += cb_lights[i].diffuseColor*lightIntensity* .2f; // square light
				}
			}
		}
	}

    // Saturate the final light color.
    lightColor = saturate(lightColor);
   // lightColor = saturate( CalculateNormalMapIntensity(input, lightColor, cb_lights[0].lightDirection));

	// TEXTURE ANIMATION -  Sample pixel color from texture at this texture coordinate location.
    input.tex.x += textureTranslation;

	// BLENDING
	float4 color1 = shaderTextures[0].Sample(SampleTypeWrap, input.tex);
    float4 color2 = shaderTextures[1].Sample(SampleTypeWrap, input.tex);
	float4 alphaValue = shaderTextures[3].Sample(SampleTypeWrap, input.tex);
	textureColor = saturate((alphaValue * color1) + ((1.0f - alphaValue) * color2));

	// Combine the light and texture color.
	float4 finalColor = lightColor * textureColor;

	/////// TRANSPARENCY /////////
	//finalColor.a = 0.2f;
	return finalColor;
}