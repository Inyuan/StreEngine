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
#define NUM_RINGS 10
#define NUM_SAMPLES 256
#define EPS 1e-3
#define PI 3.141592653589793
#define PI2 6.283185307179586

//SSR related variables
#define M_PI 3.1415926535897932384626433832795
#define TWO_PI 6.283185307
#define INV_PI 0.31830988618
#define INV_TWO_PI 0.15915494309
#define NUM_SAMPLES_SSR 32
float rand_2to1(float2 uv)
{
    // 0 - 1
    const float a = 12.9898, b = 78.233, c = 43758.5453;
    float dt = dot(uv.xy, float2(a, b));
    float sn = fmod(dt, PI);
    return frac(sin(sn) * c);
}

float Rand1(in float p) 
{
    p = frac(p * .1031);
    p *= p + 33.33;
    p *= p + p;
    return frac(p);
}

float2 Rand2(in float p)
{
    return float2(Rand1(p), Rand1(p));
}

float3 SampleHemisphereUniform(in float s, out float pdf)
{
    float2 uv = Rand2(s);
    float z = uv.x;
    float phi = uv.y * TWO_PI;
    float sinTheta = sqrt(1.0 - z * z);
    float3 dir = float3(sinTheta * cos(phi), sinTheta * sin(phi), z);
    pdf = INV_TWO_PI;
    return dir;
}

float3 SampleHemisphereCos(in float s, out float pdf)
{
    float2 uv = Rand2(s);
    float z = sqrt(1.0 - uv.x);
    float phi = uv.y * TWO_PI;
    float sinTheta = sqrt(uv.x);
    float3 dir = float3(sinTheta * cos(phi), sinTheta * sin(phi), z);
    pdf = z * INV_PI;
    return dir;
}

void create_local_To_world_mat(float3 n, out float3x3 local_to_world_mat) 
{
    float sign_ = sign(n.z);
    if (n.z == 0.0) 
    {
        sign_ = 1.0;
    }
    float a = -1.0 / (sign_ + n.z);
    float b = n.x * n.y * a;
    float3 b1 = float3(1.0 + sign_ * n.x * n.x * a, sign_ * b, -sign_ * n.x);
    float3 b2 = float3(b, sign_ + n.y * n.y * a, -n.y);

    local_to_world_mat = float3x3(b1,b2, n);
}

float CalcShadowFactor(float2 pIn_uv)
{
    //poisson分布
    float2 poissonDisk[NUM_SAMPLES];
    float ANGLE_STEP = PI2 * float(NUM_RINGS) / float(NUM_SAMPLES);
    float INV_NUM_SAMPLES = 1.0 / float(NUM_SAMPLES);
    float angle = rand_2to1(pIn_uv) * PI2;
    float radius = INV_NUM_SAMPLES;
    float radiusStep = radius;

    for (int k = 0; k < NUM_SAMPLES; k++)
    {
        poissonDisk[k] = float2(cos(angle), sin(angle)) * pow(radius, 0.75);
        radius += radiusStep;
        angle += ANGLE_STEP;
    }


    //世界坐标 转换为 light camera的HP坐标，以获得对应的深度 然后 和shadow_map比较
    float4 gbufferb = GBuffer[1][pIn_uv];// w_position
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

float4 PhysicalRender(float2 pIn_uv)
{
    float4 gbuffera = GBuffer[0][pIn_uv];// Albedo
    float4 gbufferb = GBuffer[1][pIn_uv];// w_position
    float4 gbufferc = GBuffer[2][pIn_uv];// w_normal

    float3 DiffuseAlbedo = gbuffera.rgb;
    float3  worldposition = gbufferb.rgb;
    float3  worldnormal = gbufferc.rgb;

    float3 lightVec = -normalize(light_buffer[0].Direction);
    float3 toEye = normalize(camera_buffer[0].eye_world_position - worldposition);

    // Scale light down by Lambert's cosine law.
    float ndotl = max(dot(lightVec, worldnormal), 0.1f);
    float3 lightStrength = light_buffer[0].Strength * ndotl;

    return float4(BlinnPhong(lightStrength, lightVec, worldnormal, toEye, DiffuseAlbedo), 1.0f);
}
//碰到比自己深的地方就证明遇到了表面
bool RayMarch(float3 ori, float3 dir, out float2 hitPos_uv)
{
    float step = 0.2;
    const int total_times = 300;
    float3 cur_pos = ori;
    float3 step_dir = normalize(dir) * step;

    float4x4 T = float4x4(
        0.5f, 0.0f, 0.0f, 0.0f,
        0.0f, -0.5f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.0f, 1.0f);
    float4x4 camera_transform = mul(-camera_buffer[0].view, camera_buffer[0].project);
    camera_transform = mul(camera_transform, T);

    ////test 检查uv变换正确性
    //float4 h_position = mul(float4(cur_pos, 1), camera_transform);
    //h_position.xyz /= h_position.w;
    //hitPos_uv = h_position.xy;
    //return true;

    for (int cur_step = 0; cur_step < total_times; cur_step++)
    {
        //当前世界位置
        cur_pos += step_dir;
        
        float4 h_position = mul(float4(cur_pos,1), camera_transform);

        // Complete projection by doing division by w.
        h_position.xyz /= h_position.w;

        // Depth in NDC space.
        float depth = h_position.z;

        int width, height, numMips;

        GBuffer[2].GetDimensions(0, width, height, numMips);
        //sample()的是0~1的uv,可能会迭代溢出，这里降精度
        //转换成像素索引才能获取[][]的值
        float2 index = float2(h_position.x * width, h_position.y * height);
        if (index.x > 0 && index.x < width && index.y > 0 && index.y < height)
        {
            //检查摄像机视角的depth
            float gbuffer_depth = GBuffer[2][index].w;

            if (depth - gbuffer_depth > 0.0001 && depth - gbuffer_depth < 0.001)
            {
                hitPos_uv.x = index.x;
                hitPos_uv.y = index.y;
                return true;
            }
        }
 
    }
    return false;
}

float4 screen_space_reflect(float2 pIn_uv)
{
    float4 gbuffera = GBuffer[0][pIn_uv];// Albedo
    float4 gbufferb = GBuffer[1][pIn_uv];// w_position
    float4 gbufferc = GBuffer[2][pIn_uv];// w_normal

    

    //粗糙度暂时默认
    float3 FresnelR0 = float3(0.1f, 0.1f, 0.1f);
    float3 origin_position = gbufferb.xyz;
    float3 normal = gbufferc.xyz;
    float3 lightVec = -normalize(light_buffer[0].Direction);
    float3 toEye = normalize(camera_buffer[0].eye_world_position - origin_position);
    float3 halfVec = normalize(toEye + lightVec);
    float3 fresnelFactor_p0 = SchlickFresnel(FresnelR0, halfVec, lightVec);

        
    //test 镜面反射
    //float3 relfect_dir = normalize(reflect(-toEye, normal));
    //float2 hitPos_uv = 0;//像素索引 不是0~1的uv
    //float4 result = 0;
    //if (RayMarch(origin_position, relfect_dir, hitPos_uv))
    //{
    //    //BSDF项 反射率
    //    float3 w_pos_p1 = GBuffer[1][hitPos_uv].xyz;
    //    float3 toEye_p1 = normalize(camera_buffer[0].eye_world_position - w_pos_p1);
    //    float3 halfVec_p1 = normalize(toEye_p1 + lightVec);
    //    float3 fresnelFactor_p1 = SchlickFresnel(FresnelR0, halfVec_p1, relfect_dir);
    //    //该方向间接光照强度
    //    result += float4(fresnelFactor_p0 / 0.9 * fresnelFactor_p1,1.0f) * PhysicalRender(hitPos_uv);
    //}
    //return result;
    
    ////默认diffuse 随机发射光线
    //float4 indirect_light = 0.0f;
    //float3x3 current_position_local_to_world_mat = 0;
    ////计算世界矩阵
    //create_local_To_world_mat(normal, current_position_local_to_world_mat);
    //for (int i = 0; i < NUM_SAMPLES_SSR; i++)
    //{
    //    float pdf = 0;
    //    //局部随机方向找碰到的地方
    //    float3 direction = SampleHemisphereCos(rand_2to1(pIn_uv.xy) + i, pdf);
    //    //转换到世界坐标系
    //    direction = normalize(mul(direction,current_position_local_to_world_mat));
    //    float2 hitPos_uv = 0;
    //    //计算碰到地方的直接光照
    //    //间接光照 = 碰到地方的直接光照 * 碰到地方的BSDF
    //    if(RayMarch(origin_position, direction, hitPos_uv))
    //    {
    //        //BSDF项 反射率
    //        float3 w_pos_p1 = GBuffer[1][hitPos_uv].xyz;
    //        float3 toEye_p1 = normalize(camera_buffer[0].eye_world_position - w_pos_p1);
    //        float3 halfVec_p1 = normalize(toEye_p1 + lightVec);
    //        float3 fresnelFactor_p1 = SchlickFresnel(FresnelR0, halfVec_p1, direction);
    //        //该方向间接光照强度
    //        indirect_light += float4(fresnelFactor_p0 / pdf * fresnelFactor_p1,1.0f) * PhysicalRender(hitPos_uv);
    //    }
    //}
    //return indirect_light / NUM_SAMPLES_SSR;

    //着重往reflect发射光线
    float4 indirect_light = 0.0f;
    //poisson分布
    float2 poissonDisk[NUM_SAMPLES_SSR];
    float ANGLE_STEP = PI2 * float(NUM_RINGS) / float(NUM_SAMPLES);
    float INV_NUM_SAMPLES = 1.0 / float(NUM_SAMPLES);
    float angle = rand_2to1(pIn_uv) * PI2;
    float radius = INV_NUM_SAMPLES;
    float radiusStep = radius;

    for (int k = 0; k < NUM_SAMPLES_SSR; k++)
    {
        poissonDisk[k] = float2(cos(angle), sin(angle)) * pow(radius, 0.75);
        radius += radiusStep;
        angle += ANGLE_STEP;
    }
    
    float3 relfect_dir = normalize(reflect(-toEye, normal));
    float2 hitPos_uv = 0;//像素索引 不是0~1的uv
    float4 result = 0;
    float3x3 reflect_dire_matrix = 0;
    float range = 2.0f;
    create_local_To_world_mat(relfect_dir, reflect_dire_matrix);
    for (int i = 0; i < NUM_SAMPLES_SSR; i++)
    {
        float3 sample_dir = 0;
        //以relfect_dir 为z轴的坐标系，在z轴套上泊松分布圆圈
        sample_dir.x = (poissonDisk[i] * range).x;
        sample_dir.y = (poissonDisk[i] * range).y;
        sample_dir.z = 1;
        //转换到世界坐标系
        sample_dir = normalize(mul( sample_dir, reflect_dire_matrix));
        float2 hitPos_uv = 0;
        //计算碰到地方的直接光照
        //间接光照 = 碰到地方的直接光照 * 碰到地方的BSDF
        if(RayMarch(origin_position, sample_dir, hitPos_uv))
        {
            //BSDF项 反射率
            float3 w_pos_p1 = GBuffer[1][hitPos_uv].xyz;
            float3 toEye_p1 = normalize(camera_buffer[0].eye_world_position - w_pos_p1);
            float3 halfVec_p1 = normalize(toEye_p1 + lightVec);
            float3 fresnelFactor_p1 = SchlickFresnel(FresnelR0, halfVec_p1, sample_dir);
            //该方向间接光照强度
            indirect_light += float4(fresnelFactor_p0 / 0.6 * fresnelFactor_p1,1.0f) * PhysicalRender(hitPos_uv);
        }
    }
    return indirect_light / NUM_SAMPLES_SSR;
}

float4 PS(vs_out pIn) : SV_TARGET
{
    float shadow = CalcShadowFactor(pIn.position.xy);
    float4 Color = float4(0.0f, 0.0f, 0.0f, 0.0f);
    Color = PhysicalRender(pIn.position.xy) * camera_buffer[0].ambient_light;
    
    Color = Color * max(shadow,0.1f);
    //shadow Debug
    //float4 Color = float4(shadow, shadow, shadow, 1.0f);
    

    //SSR
    Color += screen_space_reflect(pIn.position.xy);
    //return screen_space_reflect(pIn.position.xy);
    //归一？
    //Color = clamp(Color, 0, 1);
    //Color = pow(Color, 1.0 / 2.2);
    //Color.a = 1.0f;
    return Color;
    
}

