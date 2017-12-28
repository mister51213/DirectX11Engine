////////////////////////////////////////////////////////////////////////////////
// Filename: light_vs
// does the actual rendering of models.
// Called for every single vertex in the vertex buffer.
// Processes the vertices from a model and transforms them into view space.
// The vertex shader program will be called by the GPU for each vertex it needs to process.
////////////////////////////////////////////////////////////////////////////////

/////////////
// DEFINES //
/////////////
#define NUM_LIGHTS 4

/////////////
// GLOBALS //
/////////////
/*
* These global variables can be modified externally from the C++ code.
* Generally they should be put in buffer object types called "cbuffer" for single global variable. 
*/
//////////////////////
// CONSTANT BUFFERS //
//////////////////////
cbuffer MatrixBuffer:register(b0)
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
	//matrix lightViewMatrix;
 //   matrix lightProjectionMatrix;
 	matrix lightViewMatrix[2];
    matrix lightProjectionMatrix[2];
};

// pass in position of camera for reflection
cbuffer CameraBuffer:register(b1)
{
    float3 cameraPosition; // okay not to be float4?
    float padding;
};

cbuffer LightPositionBuffer:register(b2)
{
    float4 lightPosition[NUM_LIGHTS];
};

cbuffer LightShadowBuffer:register(b3)
{
    //float3 c_lightShadowPos;
    //float padding2;
	float3 c_lightShadowPos1;
    float padding1;
	float3 c_lightShadowPos2;
	float padding2;
};

cbuffer FogBuffer:register(b4)
{
    float fogStart;
    float fogEnd;
};


//////////////
// TYPEDEFS //
//////////////

/** 
* Must create two different structures here since the semantics are different 
* for vertex and pixel shaders even though the structures are the same otherwise. 
* POSITION, COLOR, and SV_POSITION are semantics that convey to the GPU the use of the variable
*/
struct VertexInputType
{
	float4 position : POSITION;
    float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
    float3 binormal : BINORMAL;
};

struct PixelInputType
{
	float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
    float3 binormal : BINORMAL;
	float3 viewDirection : TEXCOORD1;
	float3 lightPos1 : TEXCOORD2;
    float3 lightPos2 : TEXCOORD3;
    float3 lightPos3 : TEXCOORD4;
    float3 lightPos4 : TEXCOORD5;
	float4 lightViewPosition : TEXCOORD6;
    float3 lightShadowPos : TEXCOORD7;
	float4 lightViewPosition2 : TEXCOORD8;
    float3 lightShadowPos2 : TEXCOORD9;
	float fogFactor : FOG; 
};

// The output of the vertex shader will be sent to the pixel shader.
////////////////////////////////////////////////////////////////////////////////
// Vertex Shader
////////////////////////////////////////////////////////////////////////////////
PixelInputType LightVertexShader(VertexInputType input)
{
	PixelInputType output;
	float4 worldPosition;

	// Change the position vector to be 4 units for proper matrix calculations.
	input.position.w = 1.0f;

	// Calculate the position of the vertex against the world, view, and projection matrices.
	// place the vertex in the correct location for rendering in 3D space according to our view and then onto the 2D screen.
	output.position = mul(input.position, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	// Store the input color for the pixel shader to use.
	output.tex  = input.tex;

	// Calculate the normal vector against the world matrix only.
    output.normal = mul(input.normal, (float3x3)worldMatrix);
	output.normal = normalize(output.normal);

	// Calculate the tangent vector against the world matrix only and then normalize the final value.
    output.tangent = mul(input.tangent, (float3x3)worldMatrix);
    output.tangent = normalize(output.tangent);

    // Calculate the binormal vector against the world matrix only and then normalize the final value.
    output.binormal = mul(input.binormal, (float3x3)worldMatrix);
    output.binormal = normalize(output.binormal);

	// Calculate the position of the vertex in the world.
    worldPosition = mul(input.position, worldMatrix);

    // Determine the viewing direction based on the position of the camera and the position of the vertex in the world.
    output.viewDirection = cameraPosition.xyz - worldPosition.xyz;

    // Calculate the camera position. // is this the same as the following method?
    float4 cameraPosition = mul(input.position, worldMatrix);
    cameraPosition = mul(cameraPosition, viewMatrix);
	//float4 vertexPosViewSpace = mul(worldPosition, viewMatrix); @TODO: decide which way is better

    // Calculate linear fog.
    output.fogFactor = saturate((fogEnd - /*vertexPosViewSpace.z*/cameraPosition.z) / (fogEnd - fogStart));

    // Normalize the viewing direction vector.
    output.viewDirection = normalize(output.viewDirection);

	//////////// POINT LIGHTS ////////////////////
	// Determine the light positions based on the position of the lights and the position of the vertex in the world.
    output.lightPos1.xyz = lightPosition[0].xyz - worldPosition.xyz;
    output.lightPos2.xyz = lightPosition[1].xyz - worldPosition.xyz;
    output.lightPos3.xyz = lightPosition[2].xyz - worldPosition.xyz;
    output.lightPos4.xyz = lightPosition[3].xyz - worldPosition.xyz;

    // Normalize the light position vectors.
    output.lightPos1 = normalize(output.lightPos1);
    output.lightPos2 = normalize(output.lightPos2);
    output.lightPos3 = normalize(output.lightPos3);
    output.lightPos4 = normalize(output.lightPos4);
	
	//... SHADOWING ... ////... SHADOWING ... ////... SHADOWING ... ////... SHADOWING ... ////... SHADOWING ... //
	//... SHADOWING ... ////... SHADOWING ... ////... SHADOWING ... ////... SHADOWING ... ////... SHADOWING ... //
	// Calculate the position of the vertice as viewed by the light source.
    output.lightViewPosition = mul(input.position, worldMatrix);
    output.lightViewPosition = mul(output.lightViewPosition, lightViewMatrix[0]);
    output.lightViewPosition = mul(output.lightViewPosition, lightProjectionMatrix[0]);
	
	// Calculate the position of the vertice as viewed by the second light source.
    output.lightViewPosition2 = mul(input.position, worldMatrix);
    output.lightViewPosition2 = mul(output.lightViewPosition2, lightViewMatrix[1]);
    output.lightViewPosition2 = mul(output.lightViewPosition2, lightProjectionMatrix[1]);
	
	 // Determine the light position based on the position of the light and the position of the vertex in the world.
    //output.lightShadowPos = normalize(c_lightShadowPos.xyz - worldPosition.xyz);
    //output.lightShadowPos2 = normalize(lightPosition2.xyz - worldPosition.xyz);
	output.lightShadowPos = normalize(c_lightShadowPos1.xyz - worldPosition.xyz);
    output.lightShadowPos2 = normalize(c_lightShadowPos2.xyz - worldPosition.xyz);

	return output;
}