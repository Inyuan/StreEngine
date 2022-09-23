#include "stre_render.h"
#include<map>
#include<string>
//资源
class cg_resource : public s_resource
{
public:
	RESOURCE_TYPE resource_type = RESOURCE_TYPE::RES_EMPTY_OBJECT;
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

public:
	cg_static_object()
	{
		object_type = SENCE_OBJECT_TYPE::OBJ_STATIC_OBJECT;
		resource->resource_type = s_resource::RESOURCE_TYPE::RES_STATIC_OBJECT;
	}

	virtual s_resource* get_resource() override { return resource; };

};

//动态物体
class cg_dynamic_object : public s_dynamic_object, cg_mesh_object
{
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

public:
	cg_light()
	{
		object_type = SENCE_OBJECT_TYPE::OBJ_LIGHT;
		resource->resource_type = s_resource::RESOURCE_TYPE::RES_LIGHT;
	}

	virtual s_resource* get_resource() override { return resource; };

};

//材质
class cg_material : public s_material, cg_resource
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
class cg_texture : public s_texture, cg_resource
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
class cg_sence : public s_sence, cg_resource
{
private://???
	std::map<std::string, cg_object*> empty_object_group;
	std::map<std::string, cg_static_object*> static_object_group;
	std::map<std::string, cg_dynamic_object*> dynamic_object_group;
	std::map<std::string, cg_camera*> camera_group;
	std::map<std::string, cg_light*> light_group;

public:

	cg_resource* resource;

public:

	cg_sence()
	{
		resource->resource_type = s_resource::RESOURCE_TYPE::RES_SENCE;
	}

	virtual s_resource* get_resource() override { return resource; };

};