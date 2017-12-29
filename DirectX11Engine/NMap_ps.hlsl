// Textures for pixel shader
// textures[0] is the diffuse map
// textures[1] is the normal map
Texture2D textures[2];
// SamplerState defines how to sample the images, ie Linear, Bilinear, Anisotropic
SamplerState samp;

// This structure is used to describe the lights properties
struct LightTemplate
{
	float4 color;
	float3 position;
	float coneAngle;
	float3 direction;
	int type;
};

// This is the constant buffer that holds the scene information 
// such as the ambient color, light count and the light array
cbuffer LightBuffer
{
	float4 g_ambientColor;
	int g_lightCount;
	float3 g_padding;
	LightTemplate g_lights[10];
}

// PixelBuffer is just random information passed from the vertex buffer
// that is needed to properly calculate the lighting and the final color 
// of the pixel.  The name bitangent has been changed to binormal, and 
// conormal is now just the normal.
struct PixelBuffer
{
	float4 position : SV_Position;
	float2 texcoord : TEXCOORD;
	float4 color : COLOR;
	float4 worldPosition : POSITION;
	float3 tangent : TANGENT;
	float3 Binormal : BINORMAL;
	float3 normal : NORMAL;
};

// Calculates directional light only.
float4 CalculateInfiniteLight(
	float4 LightColor, 
	float3 LightDirection, 
	float3 SurfaceNormal)
{
    // Invert the light direction for calculations.
	float3 lightDir = -LightDirection;

    // Calculate the amount of light on this pixel.
	float intensity = saturate(dot(SurfaceNormal, lightDir));

	// return the scaled light color
	return LightColor * intensity;
}

// Calculates point lights, based on position of the light and
// position of the point sampled on the surface.
float4 CalculatePointLight(
	float4 LightColor, 
	float3 LightPosition, 
	float3 SurfacePosition, 
	float3 SurfaceNormal)
{
	// Calculate the direction of the light pointing to the surface
	// position
	float3 surfaceToLightDir = normalize(LightPosition - SurfacePosition);

	// Calculate the amount of light hitting the point on the surface
	float intensity = saturate(dot(surfaceToLightDir, SurfaceNormal));

	// return the scaled light color
	return LightColor * intensity;
}

// Calculates spot lights, based on position and direction of the light and
// of the direction of the surface.
float4 CalculateSpotLight(
	float4 LightColor, 
	float3 LightPosition, 
	float3 LightDirection, 
	float3 SurfacePosition, 
	float3 SurfaceNormal,
	float ConeAngle)
{
	// Calculate the direction of the light to the surface
	float3 surfaceToLightVector = SurfacePosition - LightPosition;
	float3 surfaceToLightDir = normalize(surfaceToLightVector);
	// Calculate how much of light is reaching point on surface
	float cuttoff = saturate(dot(surfaceToLightDir, LightDirection));

	// make sure ConeAngle is set properly between 1 and 180
	ConeAngle = min(180.f, max(1.f, ConeAngle));
	// normalize ConeAngle 
	ConeAngle /= 180.f;

	float4 color = float4(0.f, 0.f, 0.f, 0.f);
	// test to see if light is in the cone of light, 
	// 1 - ConeAngle so increasing cone angle increases light disc
	if (cuttoff >= 1.f - ConeAngle)
	{
		float range = 1.f - cuttoff;
		float gradientIntensity = (cuttoff - (1.f - ConeAngle)) / range;
		// Calculate how much the surface is facing the light
		float intensity = saturate(dot(-LightDirection, SurfaceNormal)) * gradientIntensity;
		intensity *= 1.f / length(surfaceToLightVector);
		// set color to scaled color
		color = LightColor * intensity;
	}

	// If not in light disc, return 0; else return scaled light color
	return color;
}


float4 main(PixelBuffer input):SV_Target
{
	// Gets the sampled pixel color from diffuse map
	float4 texColor = textures[0].Sample(samp, input.texcoord);
	// Gets the sampled pixel color from the normal map
	float4 nMapColor = textures[1].Sample(samp, input.texcoord);

	// Corrects the value of the normal map to be between -1 and 1
	float3 nMap = (nMapColor.rgb * 2.f) - float3(1.f, 1.f, 1.f);
	
	// Initialize the tangent matrix
	float3x3 tanMatrix = float3x3(input.tangent, input.Binormal, input.normal);
	// Transform the normal map normal by the tangent matrix
	float3 lightTan = mul(nMap, tanMatrix);

	// Scale the texture color by the ambient color to be the default return value
	float4 color = g_ambientColor * texColor;

	// Loop through each light in the array, up to g_lightCount
	for (int idx = 0; idx < g_lightCount; ++idx)
	{
		// Temp light color
		float4 lColor = float4(0.f, 0.f, 0.f, 0.f);

		// If infinite/directional light type = 0
		if(g_lights[idx].type == 0)
		{
			lColor = saturate(CalculateInfiniteLight(g_lights[idx].color, g_lights[idx].direction, lightTan));
		}
		// If point light type = 1
		else if(g_lights[idx].type == 1)
		{
			lColor = saturate(CalculatePointLight(g_lights[idx].color, g_lights[idx].position, input.worldPosition.xyz, lightTan));
		}
		// If spot light type = 2
		else if(g_lights[idx].type == 2)
		{
			lColor = saturate(CalculateSpotLight(g_lights[idx].color, g_lights[idx].position, g_lights[idx].direction, input.worldPosition.xyz, lightTan, g_lights[idx].coneAngle));
		}
		lColor *= g_lights[idx].color.w;
		// lights are additive, so add temp light color to return color value
		color += lColor;
	}

	// Saturate and return final color value
	return saturate(color);

}