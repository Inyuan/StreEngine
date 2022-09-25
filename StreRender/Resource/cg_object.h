#include "stre_render.h"
#include "Core/Memory/s_memory.h"
#include<map>
#include<string>

//先有cg_resource 再构建物体 再构建gpu资源布局


//要求每个物体类型都有自己的构造函数
//并在构造函数定义gpu资源布局


struct GPU_RESOURCE_LAYOUT
{
	enum GPU_RESOURCE_TYPE
	{
		GPU_RES_VERTEX = 0,
		GPU_RES_INDEX = 1,
		GPU_RES_TEXTURE = 2,
		GPU_RES_BUFFER = 3,
		GPU_RES_RENDER_TARGET = 4,
		GPU_RES_DEPTH_STENCIL = 5,
		GPU_RES_TYPE_NUMBER = 6
	};

	enum GPU_RESOURCE_STATE
	{
		GPU_RES_UPLOAD,
		GPU_RES_CONSTANT
	};

	std::string gpu_resource_name;
	//指向cg_resource的data_ptr
	void** cpu_data; 
	// 一个元素的大小
	UINT cpu_data_size; 
	// 元素的数量
	UINT cpu_data_number;

	GPU_RESOURCE_TYPE gpu_resource_type = GPU_RESOURCE_TYPE::GPU_RES_BUFFER;
	GPU_RESOURCE_STATE gpu_resource_state = GPU_RESOURCE_STATE::GPU_RES_CONSTANT;

	GPU_RESOURCE_LAYOUT() = default;

	GPU_RESOURCE_LAYOUT(
		std::string in_name, 
		void **in_cpu_data,
		UINT in_cpu_data_size,
		UINT in_cpu_data_number = 1,
		GPU_RESOURCE_TYPE in_type = GPU_RESOURCE_TYPE::GPU_RES_BUFFER,
		GPU_RESOURCE_STATE in_state = GPU_RESOURCE_STATE::GPU_RES_CONSTANT)
		: gpu_resource_name(in_name),
		cpu_data(in_cpu_data),
		cpu_data_size(in_cpu_data_size),
		cpu_data_number(in_cpu_data_number),
		gpu_resource_type(in_type),
		gpu_resource_state(in_state)
	{};

	//??? =
};


//资源
class cg_resource : public s_resource
{
public:

	std::map<std::string, GPU_RESOURCE_LAYOUT> resource_gpu_layout;

public:
	RESOURCE_TYPE resource_type = RESOURCE_TYPE::RES_BUFFER;
	//是否在本地存在
	bool is_local = false;

	char local_path[256] = { '\0' };

	void* data_ptr;

	char name[256] = { '\0' };

public:
	virtual bool is_in_local() override { return is_local; } ;
	virtual char* get_path() override { return local_path; };
	virtual char* get_name() override { return name; };
	virtual RESOURCE_TYPE get_type() override { return resource_type; };

};



//////////////////////////////////////////////
////////具体化/////////////////////////////////

//物体
class cg_object : public s_object
{
public:
	SENCE_OBJECT_TYPE object_type = SENCE_OBJECT_TYPE::OBJ_EMPTY_OBJECT;
	//坐标等

	cg_resource* resource;

	struct bound_box
	{
		s_float3 min_position;
		s_float3 max_position;
	};

	struct cpu_object_data
	{
		s_float4x4 world_position = s_float4x4::identity4x4();
		s_float4x4 tex_transform = s_float4x4::identity4x4();
		bound_box object_bound_box;
		s_float material_index = 0;
	};

public:
	cg_object()
	{
		object_type = SENCE_OBJECT_TYPE::OBJ_EMPTY_OBJECT;
		resource->resource_type = s_resource::RESOURCE_TYPE::RES_EMPTY_OBJECT;
	}

	virtual s_resource* get_resource() override { return resource; };

};

class cg_mesh_object : public cg_object
{
public:
	char mesh_type[32] = { '\0' };
	struct cpu_mesh_data
	{
		std::uint32_t vertex_group_size = 0;
		std::uint32_t index_group_size = 0;
		std::uint32_t material_size = 0;
		cpu_object_data object_constant_data;
		cg_material* material_group_ptr;
		std::uint32_t* material_index_offset_group_ptr;
		s_vertex* vertex_group_ptr = nullptr;
		s_index* index_group_ptr = nullptr;
	};

public:

};

//静态物体
class cg_static_object : public s_static_object, cg_mesh_object
{
public:
	cpu_mesh_data* cpu_static_mesh_data;
public:
	cg_static_object() = delete;

	cg_static_object(cg_resource* in_resource)
	{
		resource = in_resource;
		cpu_static_mesh_data = (cpu_mesh_data*)in_resource->data_ptr;
		object_type = SENCE_OBJECT_TYPE::OBJ_STATIC_OBJECT;
		resource->resource_type = s_resource::RESOURCE_TYPE::RES_STATIC_OBJECT;

		resource->resource_gpu_layout["vertex"] 
			= GPU_RESOURCE_LAYOUT(
				"vertex", 
				reinterpret_cast<void**>(&(cpu_static_mesh_data->vertex_group_ptr)),
				cpu_static_mesh_data->vertex_group_size * sizeof(s_vertex),
				1,
				GPU_RESOURCE_LAYOUT::GPU_RESOURCE_TYPE::GPU_RES_VERTEX,
				GPU_RESOURCE_LAYOUT::GPU_RESOURCE_STATE::GPU_RES_CONSTANT);
		//...
	}

	virtual s_resource* get_resource() override { return resource; };

};

//动态物体
class cg_dynamic_object : public s_dynamic_object, cg_mesh_object
{
public:
	struct cpu_dynamic_mesh_data
	{
		cpu_mesh_data mesh_data;
		//animation data
	};
public:
	cg_dynamic_object()
	{
		object_type = SENCE_OBJECT_TYPE::OBJ_DYNAMIC_OBJECT;
		resource->resource_type = s_resource::RESOURCE_TYPE::RES_DYNAMIC_OBJECT;
	}

	virtual s_resource* get_resource() override { return resource; };

};

//相机
class cg_camera : public s_camera, cg_object
{
public:
	//需不需要绘制
	bool is_recording = true;

	cg_resource* resource;

	struct cpu_camera_data
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

	};

public:
	cg_camera()
	{
		object_type = SENCE_OBJECT_TYPE::OBJ_CAMERA;
		resource->resource_type = s_resource::RESOURCE_TYPE::RES_CAMERA;
	}


	virtual s_resource* get_resource() override { return resource; };

};

//灯光
class cg_light : public s_light, cg_object
{
public:

	cg_resource* resource;

	struct cpu_light_data
	{
		s_float3 strength = { 10.0f, 10.0f, 10.0f };
		s_float fall_off_start = 1.0f;
		s_float3 Direction = { 0.0f, -1.0f, 0.0f };// directional/spot light only
		s_float fall_off_end = 10.0f;                           // point/spot light only
		s_float3 position = { 0.0f, 0.0f, 0.0f };  // point/spot light only
		s_float spot_power = 64.0f;
	};

public:
	cg_light()
	{
		object_type = SENCE_OBJECT_TYPE::OBJ_LIGHT;
		resource->resource_type = s_resource::RESOURCE_TYPE::RES_LIGHT;
	}

	virtual s_resource* get_resource() override { return resource; };

};

//材质
class cg_material : public s_material
{
public:

	cg_texture* textrue;

	cg_resource* resource;

	struct cpu_material_data
	{

	};

public:

	cg_material()
	{
		resource->resource_type = s_resource::RESOURCE_TYPE::RES_MATERIAL;
	}

	virtual s_resource* get_resource() override { return resource; };

};

//贴图
class cg_texture : public s_texture
{
public:

	cg_resource* resource;

public:

	cg_texture()
	{
		resource->resource_type = s_resource::RESOURCE_TYPE::RES_TEXTURE;
	}

	virtual s_resource* get_resource() override { return resource; };

};

//场景
class cg_sence : public s_sence
{
private://???
	std::map<std::string, cg_object*> empty_object_group;
	std::map<std::string, cg_static_object*> static_object_group;
	std::map<std::string, cg_dynamic_object*> dynamic_object_group;
	std::map<std::string, cg_camera*> camera_group;
	std::map<std::string, cg_light*> light_group;

public:

	cg_resource* resource;

	struct cpu_sence_data
	{
		s_float TotalTime = 0.0f;
		s_float DeltaTime = 0.0f;
	};

public:

	cg_sence()
	{
		resource->resource_type = s_resource::RESOURCE_TYPE::RES_SENCE;
	}

	virtual s_resource* get_resource() override { return resource; };

};