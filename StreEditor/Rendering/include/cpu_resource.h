#pragma once
#include <string>
#include <map>
#include "gpu_resource.h"
/***
************************************************************
*
* Resource
*
************************************************************
*/

//多线程开锁最小单位

struct s_light
{
	//只有方向光？
	s_float3 strength = { 10.0f, 10.0f, 10.0f };
	s_float fall_off_start = 1.0f;
	s_float3 Direction = { 0.0f, -1.0f, 0.0f };// directional/spot light only
	s_float fall_off_end = 10.0f;                           // point/spot light only
	s_float3 position = { 0.0f, 0.0f, 0.0f };  // point/spot light only
	s_float spot_power = 64.0f;
	//...
};

struct s_camera
{
	s_float4x4 View = s_float4x4::identity4x4();
	s_float4x4 InvView = s_float4x4::identity4x4();
	s_float4x4 Proj = s_float4x4::identity4x4();
	s_float4x4 InvProj = s_float4x4::identity4x4();
	s_float4x4 ViewProj = s_float4x4::identity4x4();
	s_float4x4 InvViewProj = s_float4x4::identity4x4();
	s_float4x4 ShadowTransform = s_float4x4::identity4x4();
	s_float3 EyePosW = { 0.0f, 0.0f, 0.0f };
	s_float2 RenderTargetSize = { 0.0f, 0.0f };
	s_float2 InvRenderTargetSize = { 0.0f, 0.0f };
	s_float NearZ = 0.0f;
	s_float FarZ = 0.0f;

	s_float4 AmbientLight = { 0.0f, 0.0f, 0.0f, 1.0f };
	//...
};

struct s_material
{
	//...
	//贴图索引 刷新时要改成对应pass的贴图序号
	unsigned int texture_index1;
	unsigned int texture_index2;
	unsigned int texture_index3;
	unsigned int texture_index4;
};

struct s_object_constant
{
	s_float4x4 world_position = s_float4x4::identity4x4();
	s_float4x4 tex_transform = s_float4x4::identity4x4();
	struct 
	{
		s_float3 min_position; // bound_box
		s_float3 max_position; // bound_box
	}object_bound_box;
	s_float material_index = 0;
};
struct s_sence_constant
{
	s_float TotalTime = 0.0f;
	s_float DeltaTime = 0.0f;
};

template<typename t_element>
struct cpu_resource
{
	//??? 自动生成
	s_uid uid;

	void* data = nullptr;

	size_t count = 0;

	t_element* get_data() { return (t_element*)data; };

	size_t get_element_size() { return sizeof(t_element); };

	size_t get_element_count() { return count; };

	const bool can_update = false;

	cpu_resource(bool in_can_update = false) : can_update(in_can_update) {}

	gpu_shader_resource* gpu_sr_ptr;
	//定义类型

	//??? 需要提供运算符重载 比如[]
};


typedef cpu_resource<s_vertex> cpu_vertex;						//一组顶点
typedef cpu_resource<s_index> cpu_index;						//一组索引
typedef cpu_resource<s_light> cpu_light;						//一组灯光
typedef cpu_resource<s_camera> cpu_camera;						//一组相机
typedef cpu_resource<s_material> cpu_material;					//一组材质
typedef cpu_resource<s_texture> cpu_texture;					//一张贴图 贴图只能一张
typedef cpu_resource<s_object_constant> cpu_object_constant;	//一组物体常量
typedef cpu_resource<s_sence_constant> cpu_sence_constant;		//一般只有一个场景缓存

//多个实例时多个这个，只是共用某些资源
template<
	class t_vertex = cpu_vertex,
	class t_index = cpu_index,
	class t_material = cpu_material,
	class t_texture = cpu_texture,
	class t_object_constant = cpu_object_constant>
	struct s_mesh
{
	s_uid uid;
	t_vertex* vertex_ptr;
	t_index* index_ptr;
	std::vector<UINT> mesh_element_index_count;
	t_object_constant* object_constant_ptr;
	t_material* material_ptr;
	std::map<std::string, t_texture*> texture_ptr;
};


template<
	class t_light = cpu_light,
	class t_camera = cpu_camera,
	class t_sence_constant = cpu_sence_constant>
	struct s_sence
{
	s_uid uid;

	std::vector < t_light*> light_group;
	std::vector < t_camera*> camera_group;
	t_sence_constant* sence_constant;
};


typedef s_mesh<> cpu_mesh;
typedef s_sence<> cpu_sence;




