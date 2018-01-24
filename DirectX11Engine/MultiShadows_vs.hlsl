/////////////
// DEFINES //
/////////////
#define NUM_LIGHTS 3

/////////////
// GLOBALS //
/////////////
struct LightTemplate_VS
{
   	matrix lightViewMatrix;
    matrix lightProjMatrix;
	float4 lightPosition_WS;
};

cbuffer MatrixBuffer:register(b0)
{
	matrix cb_worldMatrix;
	matrix cb_viewMatrix;
	matrix cb_projectionMatrix;
};

cbuffer SceneLightBufferType_VS:register(b1)
{
	LightTemplate_VS cb_lights[NUM_LIGHTS];
}

struct VertexInputType
{
	float4 vertex_ModelSpace : POSITION;
    float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
};

struct PixelInputType
{
	float4 vertex_ScrnSpace : SV_POSITION;
    float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
    float4 vertex_ProjLightSpace[NUM_LIGHTS] : TEXCOORD1;
    float3 lightPos_LS[NUM_LIGHTS] : TEXCOORD4;
};

PixelInputType main( VertexInputType input )
{
	//The final output for the vertex shader
	PixelInputType output;

	// Pass through tex coordinates untouched
	output.tex  = input.tex;

	// Pre-calculate vertex position in world space
	input.vertex_ModelSpace.w = 1.0f;
	float4 vertex_WS = mul(input.vertex_ModelSpace, cb_worldMatrix);

	// Bring vertex from model space into world, view (camera), then screen space
	float4 vertex_ViewSpace = mul(vertex_WS, cb_viewMatrix);
	output.vertex_ScrnSpace = mul(vertex_ViewSpace, cb_projectionMatrix);

	// Bring normal, tangent, and binormal into world space
    output.normal = normalize(mul(input.normal, (float3x3)cb_worldMatrix));
	
	//... SHADOWING ...
	for(int i = 0; i< NUM_LIGHTS; ++i)
	{
		// Bring the vertex into light view space, then project it onto the texture
		output.vertex_ProjLightSpace[i] = mul(vertex_WS, cb_lights[i].lightViewMatrix);
		output.vertex_ProjLightSpace[i] = mul(output.vertex_ProjLightSpace[i], cb_lights[i].lightProjMatrix);

		// Calculate light position relative to the vertex in WORLD SPACE
		output.lightPos_LS[i] = cb_lights[i].lightPosition_WS - vertex_WS.xyz;
	}

	return output;
}