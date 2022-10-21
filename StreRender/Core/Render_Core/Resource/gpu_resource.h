#pragma once
#include <wrl.h>
#include "Core/Render_Core/Render_API/d3dx12.h"
#include "stre_render.h"
#include "Resource/cg_object.h"
#include <vector>
using Microsoft::WRL::ComPtr;

// 自定义贴图只能有一个gpu_resource_element
// 只有mesh能有多张 texture 作为 gpu_resource_element
class gpu_resource
{
public:
	std::string name;
	//按照GPU_RESOURCE_TYPE分别保存数据
	std::vector<gpu_resource_element*> gpu_resource_group[GPU_RESOURCE_LAYOUT::GPU_RESOURCE_TYPE::GPU_RES_TYPE_NUMBER];
};

/// <summary>
/// gpu资源的元素类型，需要RENDER API 继承构建内存指针等
/// </summary>
class gpu_resource_element
{
	
public:
	std::string name;
	//??? 只有DX 的RTV有效
	bool need_clear = false;

	//只有上传堆有效
	BYTE* mapped_data = nullptr;

	UINT element_byte_size = 0; //  元素 顶点数据 等需要
	UINT memory_size = 0; // 字节为单位的大小
	UINT element_number = 0; // 元素总数量

	//隔多少个元素为一组
	std::vector<UINT> element_group_number; //用于存储index等数据的内部偏移( [0] 第0组有多少个元素)

	GPU_RESOURCE_LAYOUT* cpu_resource_layout;
};



