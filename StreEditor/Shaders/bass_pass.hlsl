
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
	float2 texcoord		  : TEXCOORD;
	float4 depth : TEXCOORD1;
};

struct ps_output
{
	float4 GBufferA : SV_TARGET0;
	float4 GBufferB : SV_TARGET1;
	float4 GBufferC : SV_TARGET2;
	//float4 GBufferD : SV_TARGET3;
	//float4 GBufferE : SV_TARGET4;
};

vs_out VS(vs_in vin)
{
	vs_out vout = (vs_out)0.0f;

	// Transform to world space.
	float4 world_position = mul(float4(vin.local_postion, 1.0f), obj_buffer[0].world_transform);
	vout.world_position = world_position.xyz;

	// Assumes nonuniform scaling; otherwise, need to use inverse-transpose of world matrix.
	vout.world_normal = mul(vin.local_normal, (float3x3)obj_buffer[0].world_transform);

	vout.world_tangent = mul(vin.local_normal, (float3x3)obj_buffer[0].world_transform);

	// Transform to homogeneous clip space.
	vout.h_position = mul(world_position, camera_buffer[0].view_project);
	vout.depth = vout.h_position;
	// Output vertex attributes for interpolation across triangle.
	//float4 tex_c = mul(float4(vin.texcoord, 0.0f, 1.0f), tex_transform);
	//vout.TexC = mul(texC, matData.MatTransform).xy;

	// Generate projective tex-coords to project shadow map onto scene.
	//将点投影到阴影图中
	//vout.ShadowPosH = mul(posW, gShadowTransform);
	//vout.MaterialId = vin.MaterialId;
	return vout;
}

ps_output PS(vs_out pin)
{
	ps_output OUT;

	//Basecolor
	//float4 BaseColor = gTextureMaps[textureMapIndex].Sample(gsamPointWrap, pin.TexC);//baseColor
	//float4 BaseColor = float4(matData.LightColor.r, matData.LightColor.g, matData.LightColor.b, 1);
	float4 base_color = float4(0.7,0.7,0.7,1.0);
	if (obj_buffer[0].material_index > 1)
	{
		base_color = float4(1, 0, 0, 1.0);
	}
	//Normal
	pin.world_normal = normalize(pin.world_normal);
	//float4 normalMapSample = gTextureMaps[normalMapIndex].Sample(gsamPointWrap, pin.TexC);//normal
	float3 world_normal = pin.world_normal;// NormalSampleToWorldSpace(normalMapSample.rgb, pin.NormalW, pin.TangentW);

	//Shadowfactors
	//float3 shadowFactor = float3(1.0f, 1.0f, 1.0f);
	//shadowFactor[0] = CalcShadowFactor(pin.ShadowPosH);//shadow

	//WorldPosition
	float3 world_position = pin.world_position;

	//depth
	float depth = pin.depth.z / pin.depth.w;

	//float Mat_id_store = gMaterialIndex & 0xff;
	OUT.GBufferA = base_color;
	OUT.GBufferB = float4(world_position, obj_buffer[0].material_index);
	OUT.GBufferC = float4(world_normal, depth);
	//OUT.GBufferD = float4(shadowFactor, 1.0f);
	//OUT.GBufferE = float4(1.0f, 1.0f, 1.0f, 1.0f);

	return OUT;
}
