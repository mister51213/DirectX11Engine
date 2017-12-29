// Vertex shader

cbuffer Transforms
{
	matrix g_world; // world matrix for each
	matrix g_view; // view matrix for camera
	matrix g_proj; // projection matrix for screen
};

struct VertexBuffer
{
	float3 position : POSITION;
	float2 texcoord : TEXCOORD;
	float4 color : COLOR;
	float3 tangent : TANGENT;
	float3 binormal : BINORMAL;
	float3 normal : NORMAL;
};

struct PixelBuffer
{
	float4 position : SV_Position;
	float2 texcoord : TEXCOORD;
	float4 color : COLOR;
	float4 worldPosition : POSITION;
	float3 tangent: TANGENT;
	float3 Binormal : BINORMAL;
	float3 normal : NORMAL;
};

PixelBuffer main(VertexBuffer input)
{
	PixelBuffer output;

	output.position = float4(input.position, 1.f);
	output.position = mul(output.position, g_world);
	output.worldPosition = output.position;
	output.position = mul(output.position, g_view);
	output.position = mul(output.position, g_proj);

	float3x3 rotation = (float3x3) g_world;
	output.tangent = normalize(mul(input.tangent, rotation));
	output.Binormal = normalize(mul(input.binormal, rotation));
	output.normal = normalize(mul(input.normal, rotation));

	output.color = input.color;
	output.texcoord = input.texcoord;

	return output;
}