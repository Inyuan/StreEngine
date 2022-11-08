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

/// <summary>
/// GPU资源基本类型，包含GPU读取方法枚举，和资源指针信息。被图像API继承并封装具体化。接口操作GPU的基本单位。
/// </summary>
struct gpu_shader_resource
{
	enum SHADER_RESOURCE_TYPE
	{
		SHADER_RESOURCE_TYPE_CUSTOM_BUFFER, // CSV
		SHADER_RESOURCE_TYPE_CUSTOM_BUFFER_GROUP, //SRV
		//跟随mesh子物体顺序读取的CBV group ex:material
		SHADER_RESOURCE_TYPE_CUSTOM_BUFFER_GROUP_FOLLOW_MESH,
		SHADER_RESOURCE_TYPE_TEXTURE, //SRV
		SHADER_RESOURCE_TYPE_TEXTURE_GROUP, //TABLE //贴图组类型必须再用package_textures才能构建
		SHADER_RESOURCE_TYPE_RENDER_TARGET, //SRV
		SHADER_RESOURCE_TYPE_RENDER_TARGET_GROUP, // TABLE //贴图组类型必须再用package_textures才能构建
		SHADER_RESOURCE_TYPE_RENDER_DEPTH_STENCIL, // DSV 
		SHADER_RESOURCE_TYPE_RENDER_DEPTH_STENCIL_GROUP //贴图组类型必须再用package_textures才能构建
	} shader_resource_type;


	uint32_t register_index = 0; //使用的寄存器序号

	std::string name;

	uint32_t element_size = 0;
	uint32_t element_count = 0;
	//...
};

/// <summary>
/// GPU绘制过程类型，被图像API继承并封装具体化,接口操作GPU的基本单位
/// </summary>
struct gpu_pass
{
	//??? 关切到着色器的名字
	s_uid uid;
};

/// <summary>
/// 着色器布局类型，是控制绘制过程的必要参数。接口操作GPU的基本单位。
/// </summary>
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

/// <summary>
/// 模型资源的输入类型，包括顶点 索引 材质等等与模型绑定的资源。也可以作为屏幕空间绘制的输入类型（只输入4个顶点和索引并不带任何额外数据）。
/// </summary>
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

/// <summary>
/// 绘制过程类型，接口控制GPU绘制的基本单位，负责装填GPU资源和被执行绘制。
/// </summary>
struct s_pass
{
public:
	//??? 关切到着色器的名字
	s_uid uid;

	bool is_output = false;

	//不同渲染器对应不同的gpu pass内容
	gpu_pass* gpu_pass_ptr = nullptr;

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
	std::map < std::string,gpu_shader_resource*> gpu_rt_texture_ptr;
};