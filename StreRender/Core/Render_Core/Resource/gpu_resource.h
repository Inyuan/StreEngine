#pragma once
#include <wrl.h>
#include "Core/Render_Core/Render_API/d3dx12.h"
#include "stre_render.h"
#include "Resource/cg_object.h"
using Microsoft::WRL::ComPtr;


class gpu_resource
{
public:
	std::map<std::string, gpu_resource_element*> gpu_resource_group;
};

/// <summary>
/// gpu��Դ��Ԫ�����ͣ���ҪRENDER API �̳й����ڴ�ָ���
/// </summary>
class gpu_resource_element
{

public:
	
    GPU_RESOURCE_LAYOUT gpu_resource_layout;
};
