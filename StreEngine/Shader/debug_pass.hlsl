#include "Common.hlsl"

struct vs_in {
    float3 local_postion    : POSITION;
    float3 local_normal : NORMAL;
    float2 texC    : TEXCOORD;
    float3 tangentU : TANGENT;
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

float4 PS(vs_out pIn) : SV_TARGET
{

    float4 rescolor = pIn.position;

    return rescolor;
}