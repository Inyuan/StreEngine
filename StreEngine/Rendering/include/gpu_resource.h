#pragma once
#include "base_type.h"
#include <vector>
#include <string>
#include <map>
//多线程开锁最小单位

//可以刷新，或者常量的数据块
//custom buffer
//index
//vertex

//应该是模板？只为对应的api提供对应的信息？
struct gpu_shader_resource
{
	enum SHADER_RESOURCE_TYPE
	{
		SHADER_RESOURCE_TYPE_CUSTOM_BUFFER, // CSV
		SHADER_RESOURCE_TYPE_CUSTOM_BUFFER_GROUP, //SRV
		//跟随mesh子物体顺序读取的CBV group ex:material
		SHADER_RESOURCE_TYPE_CUSTOM_BUFFER_GROUP_FOLLOW_MESH,
		SHADER_RESOURCE_TYPE_TEXTURE, //SRV
		SHADER_RESOURCE_TYPE_TEXTURE_GROUP, //TABLE
		SHADER_RESOURCE_TYPE_RENDER_TARGET, //SRV
		SHADER_RESOURCE_TYPE_RENDER_TARGET_GROUP, // TABLE
		SHADER_RESOURCE_TYPE_RENDER_DEPTH_STENCIL, // DSV
		SHADER_RESOURCE_TYPE_RENDER_DEPTH_STENCIL_GROUP
	} shader_resource_type;


	uint32_t register_index = 0; //使用的寄存器序号

	std::string name;
	const bool can_update = false;
	//只有上传堆有效
	unsigned char* mapped_data = nullptr;

	uint32_t element_size = 0;
	uint32_t element_count = 0;
	//...
};

struct gpu_pass
{
	//??? 关切到着色器的名字
	s_uid uid;
};

struct gpu_rander_target
{
	//??? 关切到渲染目标的名字
	s_uid uid;

	gpu_shader_resource* gpu_sr_ptr;
};

struct shader_layout
{
	struct shader_input
	{
		enum INPUT_ELEMENT_SIZE
		{
			INPUT_ELEMENT_SIZE_R32 = 4,
			INPUT_ELEMENT_SIZE_R32G32 = 8,
			INPUT_ELEMENT_SIZE_R32G32B32 = 12,
			INPUT_ELEMENT_SIZE_R32G32B32A32 = 16,
		};
		std::string name;
		//必须为4的整数
		INPUT_ELEMENT_SIZE size;

		shader_input() = delete;

		shader_input(
			std::string in_name,
			INPUT_ELEMENT_SIZE in_size)
			: name(in_name),
			size(in_size) {}
	};
	enum SHADER_TYPE
	{
		VS = 0,
		DS = 1,
		HS = 2,
		GS = 3,
		PS = 4
	};
	bool shader_vaild[5] = { false };
	std::wstring shader_path[5];
	std::vector<shader_input> shader_input_group;
};


struct mesh_shader_resource
{

	std::vector<const gpu_shader_resource*> sr_res;
};

// 输入：
// vertex layout
// rootsignature resource
// 输出：
// texutre resource
//
struct s_pass
{
public:
	//??? 关切到着色器的名字
	s_uid uid;



	//不同渲染器对应不同的gpu pass内容
	gpu_pass* gpu_pass = nullptr;

	shader_layout gpu_shader_layout;


	//物体资源 一一对应实例
	//
	struct gpu_mesh_resource
	{
		const gpu_shader_resource* vertex;
		const gpu_shader_resource* index;

		struct mesh_element
		{
			unsigned int index_start_offset = 0;
			unsigned int index_number = 0;
		};
		std::vector<mesh_element> mesh_element_group;

		std::map < std::string, const gpu_shader_resource*> gpu_mesh_resource_ptr;
	};

	std::map < std::string, gpu_mesh_resource> gpu_mesh;

	//SRV资源组 buffer texture
	std::map < std::string, const gpu_shader_resource*> gpu_pass_resource_ptr;
	//输出资源 
	std::map < std::string, const gpu_shader_resource*> gpu_rt_texture_ptr;
};