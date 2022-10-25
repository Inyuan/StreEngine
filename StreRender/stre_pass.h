#pragma once
#include <vector>
#include <map>
#include "gpu_reource.h"
// 输入：
// vertex layout
// rootsignature resource
// 输出：
// texutre resource
//
struct s_pass
{
public:
	std::string pass_name;

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
	} pass_shader_layout;
	

	

	//物体资源 一一对应实例
	struct mesh_resource
	{
		//顶点和索引
		struct vertex_layout
		{
			const gpu_shader_resource* vertex;
			const gpu_shader_resource* index;
		} gpu_vertex_layout;
		std::map < std::string, const gpu_shader_resource*> gpu_mesh_resource_ptr;
	};

	std::map< std::string, mesh_resource > gpu_mesh;

	//SRV资源组 buffer texture
	std::map < std::string, const gpu_shader_resource*> gpu_pass_resource_ptr;
	//输出资源 
	std::map < std::string, const gpu_shader_resource*> gpu_rt_texture_ptr;

	

};