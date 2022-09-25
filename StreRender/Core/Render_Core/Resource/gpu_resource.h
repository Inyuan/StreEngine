#pragma once
#include <wrl.h>
#include "Core/Render_Core/Render_API/d3dx12.h"
#include "stre_render.h"
#include "Resource/cg_object.h"
#include <vector>
using Microsoft::WRL::ComPtr;


class gpu_resource
{
public:
	std::string name;
	std::vector<gpu_resource_element*> gpu_resource_group[GPU_RESOURCE_LAYOUT::GPU_RESOURCE_TYPE::GPU_RES_TYPE_NUMBER];
};

/// <summary>
/// gpu资源的元素类型，需要RENDER API 继承构建内存指针等
/// </summary>
class gpu_resource_element
{
	
public:
	std::string name;

	//只有上传堆有效
	BYTE* mapped_data = nullptr;

	UINT element_byte_size = 0;
	UINT memory_size = 0;

};
