

struct vs_in {
    float3 local_postion    : POSITION;
    float3 local_normal : NORMAL;
    float2 texcoord    : TEXCOORD;
    float3 tangentU : TANGENT;
};

struct vs_out {
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD;
};

vs_out  VS(vs_in vIn)
{
    vs_out vOut;
    vOut.position = float4(vIn.local_postion,1.0f);
    vOut.texcoord = vIn.texcoord;
    return vOut;
}

float4 PS(vs_out pIn) : SV_TARGET
{

    float4 rescolor = float4(1,0,0,0);

    return rescolor;
}