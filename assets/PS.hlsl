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
	float intensity;				
	float3 lightColor;
};

Texture2D g_texture : register(t10);
SamplerState g_sampler : register(s0);

float D_GGX(float NoH, float roughness)
{
	float a = NoH * roughness;
	float k = roughness / (1.0 - NoH * NoH + a * a);
	return k * k * (1.0 / 3.1415926);
}

float3 F_Schlick(float VoH, float3 f0, float f90) {
    return f0 + (float3(f90, f90, f90) - f0) * pow(1.0 - VoH, 5.0);
}

float V_SmithGGXCorrelated(float NoV, float NoL, float roughness) {
    float a2 = roughness * roughness;
    float GGXV = NoL * sqrt(NoV * NoV * (1.0 - a2) + a2);
    float GGXL = NoV * sqrt(NoL * NoL * (1.0 - a2) + a2);
    return 0.5 / (GGXV + GGXL);
}

float Fd_Burley(float NoV, float NoL, float LoH, float roughness) {
	float f90 = 0.5 + 2.0 * roughness * LoH * LoH;
	float lightScatter = F_Schlick(NoL, 1.0, f90);
	float viewScatter = F_Schlick(NoV, 1.0, f90);
    return lightScatter * viewScatter * (1.0 / 3.1415926);
}


float4 PSMain(PSInput input) : SV_TARGET
{
	float4 result;

	float3 v = normalize(-view);
	float3 n = normalize(input.normal_w.xyz);
	float3 l = normalize(direction - input.position_w.xyz);
	float3 h = normalize(direction + view);

	float NoV = abs(dot(n, v)) + 1e-5;
    float NoL = clamp(dot(n, l), 0.0, 1.0);
    float NoH = clamp(dot(n, h), 0.0, 1.0);
    float LoH = clamp(dot(l, h), 0.0, 1.0);


	float3  diffuseColor = (1.0 - metallic) * baseColor.rgb;

	float3 f0 = 0.16 * reflectance * reflectance * (1.0 - metallic) + baseColor * metallic;
	float f90 = 0.5 + 2.0 * roughness * LoH * LoH;

    float D = D_GGX(NoH, roughness);
    float3  F = F_Schlick(LoH, f0, f90);
    float V = V_SmithGGXCorrelated(NoV, NoL, roughness);

    float3 Fr = (D * V) * F;
    float3 Fd = diffuseColor * Fd_Burley(NoV, NoL, LoH, roughness);

	float3 brdf = Fr + Fd;

	float3 illuminance =  lightColor * (intensity * NoL);

	result = float4((brdf + illuminance), 1);
	return result;
}