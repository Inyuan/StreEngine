
SamplerState gsamPointWrap        : register(s0);
SamplerState gsamPointClamp       : register(s1);
SamplerState gsamLinearWrap       : register(s2);
SamplerState gsamLinearClamp      : register(s3);
SamplerState gsamAnisotropicWrap  : register(s4);
SamplerState gsamAnisotropicClamp : register(s5);
SamplerComparisonState gsamShadow : register(s6);


// Constant data that varies per frame.
struct object_constant
{
	float4x4 world_transform;
	float4x4 tex_transform;
	float3 min_position;
	float3 max_position;
	float material_index;
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

StructuredBuffer<object_constant> obj_buffer : register(t0);
StructuredBuffer<camera> camera_buffer : register(t1);

struct vs_in
{
	float3 local_postion : POSITION;
	float3 local_normal  : NORMAL;
	float2 texcoord      : TEXCOORD;
	float3 tangentU      : TANGENT;
};

struct vs_out
{
	float4 h_position     : SV_POSITION;
	float3 world_position : POSITION0;
	float3 world_normal   : NORMAL;
	float3 world_tangent  : TANGENT;
	float2 texcoord		  : TEXCOORD0;
	float4 depth : TEXCOORD1;
};


vs_out VS(vs_in vin)
{
	vs_out vout = (vs_out)0.0f;

	// Transform to world space.
	float4 world_position = mul(float4(vin.local_postion, 1.0f), obj_buffer[0].world_transform);
	vout.world_position = world_position.xyz;

	// Transform to homogeneous clip space.
	vout.h_position = mul(world_position, camera_buffer[0].view_project);

	// Assumes nonuniform scaling; otherwise, need to use inverse-transpose of world matrix.
	vout.world_normal = mul(vin.local_normal, (float3x3)obj_buffer[0].world_transform);

	vout.world_tangent = mul(vin.local_normal, (float3x3)obj_buffer[0].world_transform);

	vout.depth = vout.h_position;

	return vout;
}

float4 PS(vs_out pin) : SV_TARGET
{
	return float4(pin.depth.z / pin.depth.w,0,0,0);
}
