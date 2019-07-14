struct PSInput
{
	float4 position : SV_POSITION;
	float2 uv : TEXCOORD;
};

struct Vertex
{
	float3 position : POSITION;
	float2 uv : TEXCOORD;
};

cbuffer ConstantBuffer : register(b0)
{
	float4x4 model;
	float4x4 view;
	float4x4 projection;
}

PSInput VSMain(Vertex vertex)
{
	float4 pos = float4(vertex.position, 1.0);
	pos = mul(pos, model);
	pos = mul(pos, view);
	pos = mul(pos, projection);

	PSInput result;
	result.position = pos;
	result.uv = vertex.uv;

	return result;
}