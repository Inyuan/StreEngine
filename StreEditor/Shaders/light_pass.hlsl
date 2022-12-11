struct light
{
    float3 Strength;
    float FalloffStart; // point/spot light only
    float3 Direction;   // directional/spot light only
    float FalloffEnd;   // point/spot light only
    float3 Position;    // point light only
    float SpotPower;    // spot light only
};

struct camera
{
    float4x4 view;
    float4x4 inv_view;
    float4x4 project;
    float4x4 inv_project;

    float4x4 view_project;
    float4x4 inv_view_project;
    float4x4 shadow_transform;

    float3 eye_world_position;
    float2 render_target_size;
    float2 inv_render_target_size;

    float near_z;
    float far_z;

    float4 ambient_light;
};

StructuredBuffer<light> light_buffer : register(t0);
StructuredBuffer<camera> camera_buffer : register(t1);
Texture2D GBuffer[3] : register(t2);

SamplerState gsamPointWrap        : register(s0);
SamplerState gsamPointClamp       : register(s1);
SamplerState gsamLinearWrap       : register(s2);
SamplerState gsamLinearClamp      : register(s3);
SamplerState gsamAnisotropicWrap  : register(s4);
SamplerState gsamAnisotropicClamp : register(s5);
SamplerComparisonState gsamShadow : register(s6);

struct vs_in {
	float4 position : POSITION;
	float2 texcoord : TEXCOORD;
};

struct vs_out {
	float4 position : SV_POSITION;
	float2 texcoord : TEXCOORD;
};

vs_out  VS(vs_in vIn)
{
	vs_out vOut;
	vOut.position = vIn.position;
	vOut.texcoord = vIn.texcoord;
	return vIn;
}

float4 Sky_lights(vs_out pIn)
{
    float4 gbuffera = GBuffer[0][pIn.position.xy];
    float3  baseColor = gbuffera.rgb;
    return float4(baseColor, 1.0f);
}

// Schlick gives an approximation to Fresnel reflectance (see pg. 233 "Real-Time Rendering 3rd Ed.").
// R0 = ( (n-1)/(n+1) )^2, where n is the index of refraction.
float3 SchlickFresnel(float3 R0, float3 normal, float3 lightVec)
{
    float cosIncidentAngle = saturate(dot(normal, lightVec));

    float f0 = 1.0f - cosIncidentAngle;
    float3 reflectPercent = R0 + (1.0f - R0) * (f0 * f0 * f0 * f0 * f0);

    return reflectPercent;
}

float3 BlinnPhong(float3 lightStrength, float3 lightVec, float3 normal, float3 toEye,float3 DiffuseAlbedo)
{
    float Shininess = 1.0f;
    float3 FresnelR0 = float3(0.1f, 0.1f, 0.1f);
    float3 Roughness = 0.3f;
    const float m = Shininess * 256.0f;
    float3 halfVec = normalize(toEye + lightVec);

    float roughnessFactor = (m + 8.0f) * pow(max(dot(halfVec, normal), 0.0f), m) / 8.0f;
    float3 fresnelFactor = SchlickFresnel(FresnelR0, halfVec, lightVec);

    float3 specAlbedo = fresnelFactor * roughnessFactor;

    // Our spec formula goes outside [0,1] range, but we are 
    // doing LDR rendering.  So scale it down a bit.
    specAlbedo = specAlbedo / (specAlbedo + 1.0f);

    return (DiffuseAlbedo + specAlbedo) * lightStrength;
}

float4 PhysicalRender(vs_out pIn)
{
    float4 gbuffera = GBuffer[0][pIn.position.xy];// Albedo
    float4 gbufferb = GBuffer[1][pIn.position.xy];// w_position
    float4 gbufferc = GBuffer[2][pIn.position.xy];// w_normal

    float3 DiffuseAlbedo = gbuffera.rgb;
    float3  worldnormal = gbufferb.rgb;
    float3  worldposition = gbufferc.rgb;

    float3 lightVec = -light_buffer[0].Direction;
    float3 toEye = normalize(camera_buffer[0].eye_world_position - worldposition);

    // Scale light down by Lambert's cosine law.
    float ndotl = max(dot(lightVec, worldnormal), 0.0f);
    float3 lightStrength = light_buffer[0].Strength * ndotl;

    return float4(BlinnPhong(lightStrength, lightVec, worldnormal, toEye, DiffuseAlbedo),1.0f);
}


float4 PS(vs_out pIn) : SV_TARGET
{
    float4 Color = float4(1.0f, 1.0f, 1.0f, 1.0f);
    Color = PhysicalRender(pIn);

    return Color;
}

