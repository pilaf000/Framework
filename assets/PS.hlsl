struct PSInput
{
	float4 position : SV_POSITION;
	float4 position_w : POSITION; 
	float4 color : COLOR;		 
	float4 normal_w : NORMAL;	  
	float2 uv : TEXCOORD;
};

cbuffer MaterialInfo : register(b1)
{
	float3 baseColor;				
	float roughness;				
	float metallic;					
	float reflectance;				
	float2 pad;						
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
	return float4(0, 0, 0, 1.0);
}