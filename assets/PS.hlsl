struct PSInput
{
	float4 position : SV_POSITION; 	// perspective変換後の頂点座標
	float4 position_w : POSITION;  	// ワールド空間の頂点座標
	float4 color : COLOR;		   	// 頂点カラー
	float4 normal_w : NORMAL;	   	// ワールド空間の法線
	float2 uv : TEXCOORD;		   	// テクスチャuv
};

cbuffer MaterialInfo : register(b1)
{
	float3 baseColor;				// ベースカラー
	float roughness;				// 表面の荒さ
	float metallic;					// 金属感
	float reflectance;				// 反射率
	float2 pad;						// 使わない
	float3 view;					// 視点座標
};
cbuffer LightInfo : register(b2)
{
	float3 direction;				// ライトの方向
	float3 lightColor;				// ライトのカラー
	float intensity;				// ライトの強さ
};

Texture2D g_texture : register(t10);
SamplerState g_sampler : register(s0);

float4 PSMain(PSInput input) : SV_TARGET
{
	return float4(0, 0, 0, 1);
}