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
StructuredBuffer<camera> light_camera_buffer : register(t2);
Texture2D shadow_map : register(t3);
Texture2D GBuffer[3] : register(t4);


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

float GetRandomNumber(float2 texCoord, int Seed)
{
    return frac(sin(dot(texCoord.xy, float2(12.9898, 78.233)) + Seed) * 43758.5453);
}

// Shadow map related variables
#define BLOCKER_SEARCH_NUM_SAMPLES NUM_SAMPLES
#define PCF_NUM_SAMPLES NUM_SAMPLES
#define NUM_RINGS 10
#define NUM_SAMPLES 256
#define EPS 1e-3
#define PI 3.141592653589793
#define PI2 6.283185307179586


float rand_2to1(float2 uv)
{
    // 0 - 1
    const float a = 12.9898, b = 78.233, c = 43758.5453;
    float dt = dot(uv.xy, float2(a, b));
    float sn = fmod(dt, PI);
    return frac(sin(sn) * c);
}

float2 get_shadow_pos_uv(float4 worldposition, float4x4 shadow_transform)
{
    float4 shadowPosH = mul(worldposition, shadow_transform);
    // Complete projection by doing division by w.
    shadowPosH.xyz /= shadowPosH.w;
    return shadowPosH.xy;
}

float CalcShadowFactor(vs_out pIn)
{
    //poisson分布
    float2 poissonDisk[NUM_SAMPLES];
    float ANGLE_STEP = PI2 * float(NUM_RINGS) / float(NUM_SAMPLES);
    float INV_NUM_SAMPLES = 1.0 / float(NUM_SAMPLES);
    float angle = rand_2to1(pIn.position.xy) * PI2;
    float radius = INV_NUM_SAMPLES;
    float radiusStep = radius;

    for (int k = 0; k < NUM_SAMPLES; k++)
    {
        poissonDisk[k] = float2(cos(angle), sin(angle)) * pow(radius, 0.75);
        radius += radiusStep;
        angle += ANGLE_STEP;
    }


    //世界坐标 转换为 light camera的HP坐标，以获得对应的深度 然后 和shadow_map比较
    float4 gbufferb = GBuffer[1][pIn.position.xy];// w_position
    //float4 gbufferb = GBuffer[1].Sample(gsamPointClamp, pIn.position.xy).r;
    float4 worldposition = gbufferb;
    worldposition.w = 1;
    
    float4x4 T = float4x4(
        0.5f, 0.0f, 0.0f, 0.0f,
        0.0f, -0.5f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.0f, 1.0f);
    float4x4 shadow_transform = mul(-light_camera_buffer[0].view, light_camera_buffer[0].project);
    shadow_transform = mul(shadow_transform,T);
    float4 shadowPosH = mul(worldposition, shadow_transform);


    // Complete projection by doing division by w.
    shadowPosH.xyz /= shadowPosH.w;

    // Depth in NDC space.
    float depth = shadowPosH.z;

    uint width, height, numMips;

    shadow_map.GetDimensions(0, width, height, numMips);

    //PCF
    // Texel size.
    //float dx = 1.0f / (float)width;
    //float dy = 1.0f / (float)height;
    //float percentLit = 0.0f;
    //const float2 offsets[25] =
    //{
    //    float2(-2 * dx, -2 * dy), float2(-dx,-2 * dy), float2(0.0f,-2 * dy),float2(dx,-2 * dy), float2(2 * dx,-2 * dy),
    //    float2(-2 * dx, -dy), float2( -dx, -dy), float2(0.0f, -dy),float2(dx, -dy), float2(2 * dx, -dy),
    //    float2(-2 * dx,0.0f), float2( -dx,0.0f), float2(0.0f,0.0f),float2(dx,0.0f), float2(2 * dx,0.0f),
    //    float2(-2 * dx,  dy), float2( -dx,  dy), float2(0.0f,  dy),float2(dx,  dy), float2(2 * dx,  dy),
    //    float2(-2 * dx, 2 * dy), float2( -dx, 2 * dy), float2(0.0f, 2 * dy),float2(dx, 2 * dy), float2(2 * dx, 2 * dy)
    //};
    ////Debug
    ////return step(depth-0.0005 , shadow_map.Sample(gsamPointClamp, shadowPosH.xy).r);
    ////[unroll]
    //for (int i = 0; i < 25; ++i)
    //{
    //    float this_percentLit = step(depth - 0.0005, shadow_map.Sample(gsamPointClamp, shadowPosH.xy + offsets[i]).r);
    //    percentLit += this_percentLit;
    //}
    //percentLit = smoothstep(0, 25, percentLit);
    //return percentLit;

    ////固定半径全均值 泊松分布采样
    //float percentLit = 0.0f;
    //float filterStride = 5.0;
    //float filter_range = filterStride / 1080;
    //for (int i = 0; i < NUM_SAMPLES; i++)
    //{
    //    float this_percentLit = step(depth - 0.0005, shadow_map.Sample(gsamPointClamp, shadowPosH.xy+ poissonDisk[i] * filter_range).r);
    //    percentLit += this_percentLit;
    //}
    //percentLit = smoothstep(0, NUM_SAMPLES, percentLit);
    //return percentLit;

    //PCSS 固定采样数 按距离决定采样半径
    // 1.平均深度
    float filterStride = 70.0f;
    float filter_range = filterStride / 1080;
    float avarage_depth = 0.0f;
    for (uint i = 0; i < NUM_SAMPLES; i++)
    {
        avarage_depth += shadow_map.Sample(gsamPointClamp, shadowPosH.xy + poissonDisk[i] * filter_range).r;
    }
    avarage_depth /= NUM_SAMPLES;
    //2.采样半径
    float w_light = 900.0f;
    float shadow_map_sample_radius = (depth - avarage_depth) * w_light / avarage_depth;//决定采样半径大小
    filter_range = shadow_map_sample_radius * filterStride / 1080;
    //3.采样
    float percentLit = 0.0f;
    for (uint x = 0; x < NUM_SAMPLES; x++)
    {
        float this_percentLit = step(depth - 0.0005, shadow_map.Sample(gsamPointClamp, shadowPosH.xy + poissonDisk[x] * filter_range).r);
        percentLit += this_percentLit;
    }
    percentLit = smoothstep(0, NUM_SAMPLES, percentLit);
    return percentLit;
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

float3 BlinnPhong(float3 lightStrength, float3 lightVec, float3 normal, float3 toEye, float3 DiffuseAlbedo)
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
    float3  worldposition = gbufferb.rgb;
    float3  worldnormal = gbufferc.rgb;

    float3 lightVec = -normalize(light_buffer[0].Direction);
    float3 toEye = normalize(camera_buffer[0].eye_world_position - worldposition);

    // Scale light down by Lambert's cosine law.
    float ndotl = max(dot(lightVec, worldnormal), 0.0f);
    float3 lightStrength = light_buffer[0].Strength * ndotl;

    return float4(BlinnPhong(lightStrength, lightVec, worldnormal, toEye, DiffuseAlbedo), 1.0f);
}


float4 PS(vs_out pIn) : SV_TARGET
{
    float shadow = CalcShadowFactor(pIn);
    float4 Color = float4(0.0f, 0.0f, 0.0f, 0.0f);
    Color = PhysicalRender(pIn) * camera_buffer[0].ambient_light;
    
    Color = Color * max(shadow,0.0f);
    //
    //float4 Color = float4(shadow, shadow, shadow, 1.0f);
    
    return Color;
}

