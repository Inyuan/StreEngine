#pragma once
#include "Core/Window/s_window.h"
#include "Resource/cg_object.h"
#include "Resource/gpu_resource.h"


class render
{
public:
	virtual void draw_call() = 0;

	virtual void allocate_pass() = 0;

	virtual gpu_resource_element* allocate_gpu_memory(GPU_RESOURCE_LAYOUT in_resource_layout) = 0;
	
	//???
	virtual gpu_resource* update_gpu_memory(cg_resource* in_resource) = 0;

	virtual gpu_resource_element* create_gpu_texture(
		std::string in_gpu_texture_name,
		GPU_RESOURCE_LAYOUT::GPU_RESOURCE_TYPE in_resoure_type) = 0;

	virtual void init(HWND in_main_wnd) = 0;

	virtual void over() = 0;
};

