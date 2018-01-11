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
	matrix cb_worldMatrix;
	matrix cb_viewMatrix;
	matrix cb_projectionMatrix;

	LightTemplate_VS cb_lights[NUM_LIGHTS];
};

// pass in position of camera for reflection
cbuffer CameraBuffer:register(b1)
{
    float3 cb_cameraPosition; // okay not to be float4?
    float cb_padding;
};

cbuffer FogBuffer:register(b2)
{
    float cb_fogStart;
    float cb_fogEnd;
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
	float4 vertex_ModelSpace : POSITION;
    float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
    float3 binormal : BINORMAL;
};

struct PixelInputType
{
	float4 vertex_ModelSpace : TEXCOORD12;
	float4 vertex_ScrnSpace : SV_POSITION;
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
//PixelInputType LightVertexShader(VertexInputType input)
//{
//	// The output of this function, to be passed to the pixel shader
//	PixelInputType output;

//	// Pass in tex coordinate untouched
//	output.tex  = input.tex;

//	// Pre-calculate vertex position in world space
//	input.vertex_ModelSpace.w = 1.0f;
//	float4 vertex_WS =  mul(input.vertex_ModelSpace, cb_worldMatrix);

//	// Bring the vertex position into view, then screen space
//	output.vertex_ScrnSpace = mul(vertex_WS, cb_viewMatrix);
//	output.vertex_ScrnSpace = mul(output.vertex_ScrnSpace, cb_projectionMatrix);

//	// get view direction in world space
//	output.viewDirection = normalize(cb_cameraPosition.xyz - vertex_WS); 

//	// Bring normal, tangent, and binormal into world space
//    output.normal = normalize(mul(input.normal, (float3x3)cb_worldMatrix));
//    output.tangent = normalize(mul(input.tangent, (float3x3)cb_worldMatrix));
//    output.binormal = normalize(mul(input.binormal, (float3x3)cb_worldMatrix));

//	// Calculate the position of the vertex in the world.
//   // float4 vertexPositionWS = mul(input.vertex_ModelSpace, cb_worldMatrix);

//    // Determine the viewing direction based on the position of the camera and the position of the vertex in the world.
//    //output.viewDirection = cb_cameraPosition.xyz - vertexPositionWS.xyz;

//    // Calculate the camera position. // is this the same as the following method?
//    float4 cb_cameraPosition = mul(input.vertex_ModelSpace, cb_worldMatrix);
//    cb_cameraPosition = mul(cb_cameraPosition, cb_viewMatrix);
//	//float4 vertexPosViewSpace = mul(vertexPositionWS, cb_viewMatrix); @TODO: decide which way is better

//    // Calculate linear fog.
//    output.fogFactor = saturate((cb_fogEnd - /*vertexPosViewSpace.z*/cb_cameraPosition.z) / (cb_fogEnd - cb_fogStart));

//    // Normalize the viewing direction vector.
//   // output.viewDirection = normalize(output.viewDirection);

//	//... SHADOWING ... //
//	for(int i = 0; i< NUM_LIGHTS; ++i)
//	{
//		output.lightViewPositions[i] = mul(input.vertex_ModelSpace, cb_worldMatrix);
//		output.lightViewPositions[i] = mul(output.lightViewPositions[i], cb_lights[i].lightViewMatrix);
//		output.lightViewPositions[i] = mul(output.lightViewPositions[i], cb_lights[i].lightProjMatrix);
//		//output.lightPositions[i] = normalize(c_lightShadowPos[i].xyz - vertexPositionWS.xyz);

//		// Calculate the world offset of the light from the vertex
//		output.lightPositions[i] = cb_lights[i].lightPosition - vertex_WS.xyz;

//		//output.lightPositions[i] = mul(cb_lights[i].lightPosition, cb_worldMatrix);
//		//output.lightPositions[i] = output.lightPositions[i] - vertexPositionWS.xyz;
//	}

//	return output;
//}

// BACKUP /////
// The output of the vertex shader will be sent to the pixel shader.
////////////////////////////////////////////////////////////////////////////////
// Vertex Shader
////////////////////////////////////////////////////////////////////////////////
PixelInputType LightVertexShader(VertexInputType input)
{
	PixelInputType output;
	float4 vertexPositionWS;

	// Change the position vector to be 4 units for proper matrix calculations.
	input.vertex_ModelSpace.w = 1.0f;
	output.vertex_ModelSpace = input.vertex_ModelSpace;

	// Calculate the position of the vertex against the world, view, and projection matrices.
	// place the vertex in the correct location for rendering in 3D space according to our view and then onto the 2D screen.
	output.vertex_ScrnSpace = mul(input.vertex_ModelSpace, cb_worldMatrix);
	//output.rawPosition = input.position;
	output.vertex_ScrnSpace = mul(output.vertex_ScrnSpace, cb_viewMatrix);
	output.vertex_ScrnSpace = mul(output.vertex_ScrnSpace, cb_projectionMatrix);

	// Store the input color for the pixel shader to use.
	output.tex  = input.tex;

	// Calculate the normal vector against the world matrix only.
    output.normal = mul(input.normal, (float3x3)cb_worldMatrix);
	output.normal = normalize(output.normal);

	// Calculate the tangent vector against the world matrix only and then normalize the final value.
    output.tangent = mul(input.tangent, (float3x3)cb_worldMatrix);
    output.tangent = normalize(output.tangent);

    // Calculate the binormal vector against the world matrix only and then normalize the final value.
    output.binormal = mul(input.binormal, (float3x3)cb_worldMatrix);
    output.binormal = normalize(output.binormal);

	// Calculate the position of the vertex in the world.
    vertexPositionWS = mul(input.vertex_ModelSpace, cb_worldMatrix);

    // Determine the viewing direction based on the position of the camera and the position of the vertex in the world.
    output.viewDirection = cb_cameraPosition.xyz - vertexPositionWS.xyz;

    // Calculate the camera position. // is this the same as the following method?
    float4 cb_cameraPosition = mul(input.vertex_ModelSpace, cb_worldMatrix);
    cb_cameraPosition = mul(cb_cameraPosition, cb_viewMatrix);
	//float4 vertexPosViewSpace = mul(vertexPositionWS, cb_viewMatrix); @TODO: decide which way is better

    // Calculate linear fog.
    output.fogFactor = saturate((cb_fogEnd - /*vertexPosViewSpace.z*/cb_cameraPosition.z) / (cb_fogEnd - cb_fogStart));

    // Normalize the viewing direction vector.
    output.viewDirection = normalize(output.viewDirection);

	//////////// POINT LIGHTS ////////////////////
	// Determine the light positions based on the position of the lights and the position of the vertex in the world.
    //output.lightPos1.xyz = normalize(lightPosition[0].xyz - vertexPositionWS.xyz);
    //output.lightPos2.xyz = normalize(lightPosition[1].xyz - vertexPositionWS.xyz);
    //output.lightPos3.xyz = normalize(lightPosition[2].xyz - vertexPositionWS.xyz);
	
	//... SHADOWING ... ////... SHADOWING ... ////... SHADOWING ... ////... SHADOWING ... ////... SHADOWING ... //
	for(int i = 0; i< NUM_LIGHTS; ++i)
	{
		//// Calculate the position of the vertice as viewed by the light sources.
		//output.lightViewPositions[i] = mul(input.position, cb_worldMatrix);
		//output.lightViewPositions[i] = mul(output.lightViewPositions[i], lightcb_viewMatrix[i]);
		//output.lightViewPositions[i] = mul(output.lightViewPositions[i], lightProjectionMatrix[i]);
		////output.lightPositions[i] = normalize(c_lightShadowPos[i].xyz - vertexPositionWS.xyz);

		//// Calculate the world offset of the light from the vertex
		//output.lightPositions[i] = c_lightShadowPos[i].xyz - vertexPositionWS.xyz;

		// NEW ALL IN ONE LIGHT BUFFER METHOD
		// Calculate the position of the vertice as viewed by the light sources.
		output.lightViewPositions[i] = mul(input.vertex_ModelSpace, cb_worldMatrix);
		output.lightViewPositions[i] = mul(output.lightViewPositions[i], cb_lights[i].lightViewMatrix);
		output.lightViewPositions[i] = mul(output.lightViewPositions[i], cb_lights[i].lightProjMatrix);
		//output.lightPositions[i] = normalize(c_lightShadowPos[i].xyz - vertexPositionWS.xyz);

		// Calculate the world offset of the light from the vertex
		output.lightPositions[i] = cb_lights[i].lightPosition - vertexPositionWS.xyz;

		//output.lightPositions[i] = mul(cb_lights[i].lightPosition, cb_worldMatrix);
		//output.lightPositions[i] = output.lightPositions[i] - vertexPositionWS.xyz;
	}

	return output;
}