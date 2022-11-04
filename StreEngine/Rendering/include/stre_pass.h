#pragma once
#include <vector>
#include <map>
#include "gpu_resource.h"


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
			UINT index_start_offset = 0;
			UINT index_number = 0;
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


