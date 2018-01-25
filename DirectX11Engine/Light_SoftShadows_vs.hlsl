
/////////////
// DEFINES //
/////////////
#define NUM_LIGHTS 3

/////////////
// GLOBALS //
/////////////
struct LightTemplate_VS
{
   	matrix lightViewMatrix; // NOT USED IN THIS SHADER
    matrix lightProjMatrix; // NOT USED IN THIS SHADER
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

// pass in position of camera for reflection
cbuffer CameraBuffer:register(b2) /*b1*/
{
    float3 cb_camPosition_WS; // okay not to be float4?
    float cb_padding;
};

struct VertexInputType
{
	float4 vertex_ModelSpace : POSITION;
    float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
    float3 binormal : BINORMAL;
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


PixelInputType main( VertexInputType input )
{
	//The final output for the vertex shader
	PixelInputType output;

	// Pass through tex coordinates untouched
	output.tex  = input.tex;

	// Pre-calculate vertex position in world space
	input.vertex_ModelSpace.w = 1.0f;

	// Calculate the position of the vertex against the world, view, and projection matrices.
	output.vertex_ModelSpace = mul(input.vertex_ModelSpace, cb_worldMatrix);
	output.vertex_ModelSpace = mul(output.vertex_ModelSpace, cb_viewMatrix);
	output.vertex_ModelSpace = mul(output.vertex_ModelSpace, cb_projectionMatrix);

	// Store the position of the vertice as viewed by the camera in a separate variable.
	output.vertex_ScrnSpace = output.vertex_ModelSpace;

	// Bring normal, tangent, and binormal into world space
    output.normal = normalize(mul(input.normal, (float3x3)cb_worldMatrix));
    output.tangent = normalize(mul(input.tangent, (float3x3)cb_worldMatrix));
    output.binormal = normalize(mul(input.binormal, (float3x3)cb_worldMatrix));

	// Store worldspace view direction for specular calculations
	float4 vertex_WS = mul(input.vertex_ModelSpace, cb_worldMatrix);
	output.viewDirection = normalize(cb_camPosition_WS.xyz - vertex_WS.xyz);

	for(int i = 0; i< NUM_LIGHTS; ++i)
	{
		// Calculate light position relative to the vertex in WORLD SPACE
		output.lightPos_LS[i] = cb_lights[i].lightPosition_WS - vertex_WS.xyz;
	}

	return output;

	//	//// WORKING VERSION 2
	////The final output for the vertex shader
	//PixelInputType output;

	//// Pass through tex coordinates untouched
	//output.tex  = input.tex;

	//// Pre-calculate vertex position in world space
	//input.vertex_ModelSpace.w = 1.0f;
	//float4 vertex_WS = mul(input.vertex_ModelSpace, cb_worldMatrix);

	//// Store worldspace view direction for specular calculations
	//output.viewDirection = normalize(cb_camPosition_WS.xyz - vertex_WS.xyz);

	//// Change the position vector to homogeneous coordinates for proper position-based calculations.
	////input.vertex_ModelSpace.w = 1.0f;
	//output.vertex_ModelSpace = input.vertex_ModelSpace; // store the raw vert position in model space for pixel shader

	//// Bring vertex from model space into world, view (camera), then screen space
	//float4 vertex_ViewSpace = mul(vertex_WS, cb_viewMatrix);
	//output.vertex_ScrnSpace = mul(vertex_ViewSpace, cb_projectionMatrix);

	//// Bring normal, tangent, and binormal into world space
 //   output.normal = normalize(mul(input.normal, (float3x3)cb_worldMatrix));
 //   output.tangent = normalize(mul(input.tangent, (float3x3)cb_worldMatrix));
 //   output.binormal = normalize(mul(input.binormal, (float3x3)cb_worldMatrix));

	//for(int i = 0; i< NUM_LIGHTS; ++i)
	//{
	//	output.lightPos_LS[i] = cb_lights[i].lightPosition_WS - vertex_WS.xyz;
	//}

	//return output;


	//// WORKING VERSION 1
 //   PixelInputType output;
	//float4 worldPosition;
    
	//// Change the position vector to be 4 units for proper matrix calculations.
 //   input.vertex_ModelSpace.w = 1.0f;

	//// Calculate the position of the vertex against the world, view, and projection matrices.
 //   output.vertex_ModelSpace = mul(input.vertex_ModelSpace, cb_worldMatrix);
 //   output.vertex_ModelSpace = mul(output.vertex_ModelSpace, cb_viewMatrix);
 //   output.vertex_ModelSpace = mul(output.vertex_ModelSpace, cb_projectionMatrix);
    
	////// Store the position of the vertice as viewed by the camera in a separate variable.
 //   output.vertex_ScrnSpace = output.vertex_ModelSpace;

	//// Store the texture coordinates for the pixel shader.
 //   output.tex = input.tex;
    
	//// Calculate the normal vector against the world matrix only.
 //   output.normal = mul(input.normal, (float3x3)cb_worldMatrix);
	
 //   // Normalize the normal vector.
 //   output.normal = normalize(output.normal);
	
	//return output;
 ////WORKING VERSION
}