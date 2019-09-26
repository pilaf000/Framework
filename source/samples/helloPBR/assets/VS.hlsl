struct PSInput
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
	float3 normal : NORMAL;
	float2 uv : TEXCOORD;
};

struct Vertex
{
	float3 position : POSITION;
	float4 color : COLOR;
	float3 normal : NORMAL;
	float2 uv : TEXCOORD;
};

cbuffer MatrixInfo : register(b0)
{
	float4x4 world;
	float4x4 view;
	float4x4 projection;
};

PSInput VSMain(Vertex vertex)
{
	float4 pos = float4(vertex.position, 1.0);
	pos = mul(pos, world);
	pos = mul(pos, view);
	pos = mul(pos, projection);

	PSInput result;
	result.position = pos;
	result.color = vertex.color;
	result.normal= vertex.normal;
	result.uv = vertex.uv;

	return result;
}