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
//#define NUM_LIGHTS 4
#define NUM_LIGHTS 3

/////////////
// GLOBALS //
/////////////
struct LightTemplate_VS
{
   	matrix lightViewMatrix;
    matrix lightProjMatrix;
	float4 lightPosition;
};

//////////////////////
// CONSTANT BUFFERS //
//////////////////////
cbuffer MatrixBuffer:register(b0)
{
	//int cb_lightCount;
	//float3 cb_padding;

	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
   //	matrix lightViewMatrix[NUM_LIGHTS]; // @TODO - REMOVE 
   // matrix lightProjectionMatrix[NUM_LIGHTS]; // @TODO - REMOVE
	LightTemplate_VS cb_lights[NUM_LIGHTS];
};

// pass in position of camera for reflection
cbuffer CameraBuffer:register(b1)
{
    float3 cameraPosition; // okay not to be float4?
    float padding;
};

//cbuffer LightShadowBuffer:register(b2) // @TODO: REMOVE
//{
//	float4 c_lightShadowPos[NUM_LIGHTS];
//};

//cbuffer FogBuffer:register(b3) // @TODO: -> b2
cbuffer FogBuffer:register(b2) // @TODO: -> b2
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
    float4 lightViewPositions[NUM_LIGHTS] : TEXCOORD6;
    float3 lightPositions[NUM_LIGHTS] : TEXCOORD9;
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
    //output.lightPos1.xyz = normalize(lightPosition[0].xyz - worldPosition.xyz);
    //output.lightPos2.xyz = normalize(lightPosition[1].xyz - worldPosition.xyz);
    //output.lightPos3.xyz = normalize(lightPosition[2].xyz - worldPosition.xyz);
	
	//... SHADOWING ... ////... SHADOWING ... ////... SHADOWING ... ////... SHADOWING ... ////... SHADOWING ... //
	for(int i = 0; i< NUM_LIGHTS; ++i)
	{
		//// Calculate the position of the vertice as viewed by the light sources.
		//output.lightViewPositions[i] = mul(input.position, worldMatrix);
		//output.lightViewPositions[i] = mul(output.lightViewPositions[i], lightViewMatrix[i]);
		//output.lightViewPositions[i] = mul(output.lightViewPositions[i], lightProjectionMatrix[i]);
		////output.lightPositions[i] = normalize(c_lightShadowPos[i].xyz - worldPosition.xyz);

		//// Calculate the world offset of the light from the vertex
		//output.lightPositions[i] = c_lightShadowPos[i].xyz - worldPosition.xyz;

		// NEW ALL IN ONE LIGHT BUFFER METHOD
		// Calculate the position of the vertice as viewed by the light sources.
		output.lightViewPositions[i] = mul(input.position, worldMatrix);
		output.lightViewPositions[i] = mul(output.lightViewPositions[i], cb_lights[i].lightViewMatrix);
		output.lightViewPositions[i] = mul(output.lightViewPositions[i], cb_lights[i].lightProjMatrix);
		//output.lightPositions[i] = normalize(c_lightShadowPos[i].xyz - worldPosition.xyz);

		// Calculate the world offset of the light from the vertex
		output.lightPositions[i] = cb_lights[i].lightPosition - worldPosition.xyz;
	}

	return output;
}