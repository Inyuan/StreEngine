#pragma once
#include "base_type.h"
#include <vector>
#include <string>

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
		SHADER_RESOURCE_TYPE_TEXTURE, //SRV
		SHADER_RESOURCE_TYPE_TEXTURE_GROUP, //TABLE
		SHADER_RESOURCE_TYPE_RENDER_TARGET, //SRV
		SHADER_RESOURCE_TYPE_RENDER_TARGET_GROUP, // TABLE
		SHADER_RESOURCE_TYPE_RENDER_DEPTH_STENCIL // DSV
	}shader_resource_type;

	size_t register_index = 0; //使用的寄存器序号

	std::string name;
	const bool can_update = false;
	//只有上传堆有效
	BYTE* mapped_data = nullptr;

	size_t element_size = 0;
	size_t element_count = 0;


	//隔多少个元素为一组
	std::vector<UINT> element_group_number; //用于存储index等数据的内部偏移( [0] 第0组有多少个元素)
	
	gpu_shader_resource(bool in_can_update = false) : can_update(in_can_update) {};
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