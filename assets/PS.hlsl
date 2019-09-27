struct PSInput
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
	float3 normal : NORMAL;
	float2 uv : TEXCOORD;
};

cbuffer MaterialInfo : register(b1)
{
	float3 baseColor;
	float roughness;
	float metallic;
	float reflectance;
	float3 view;
};
cbuffer LightInfo : register(b2)
{
	float3 direction;
	float3 lightColor;
	float intensity;
};

Texture2D g_texture : register(t10);
SamplerState g_sampler : register(s0);

float4 PSMain(PSInput input) : SV_TARGET
{
	return float4(g_texture.Sample(g_sampler, input.uv) * baseColor * lightColor, 1.0);
	//return float4(0, 0, 0, 1);
}