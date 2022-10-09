#pragma once
#include "Core/Window/s_window.h"
#include "Resource/constant_pass.h"

class render
{
public:
	virtual void draw_call(
		constant_pass * in_pass,
		std::vector<gpu_resource*>& in_object,
		gpu_resource* in_sence) = 0;

	virtual constant_pass* allocate_pass(constant_pass::pass_layout in_constant_pass_layout) = 0;

	virtual gpu_resource* allocate_gpu_resource(cg_resource* in_resource) = 0;

	virtual void update_gpu_resource(cg_resource* in_resource, gpu_resource* in_out_gpu_resource_ptr) = 0;

	virtual gpu_resource_element* allocate_gpu_memory(GPU_RESOURCE_LAYOUT& in_resource_layout) = 0;

	virtual void update_gpu_memory(GPU_RESOURCE_LAYOUT& in_resource_layout, gpu_resource_element* in_out_resource_elem_ptr) = 0;


	virtual gpu_resource_element* create_gpu_texture(
		std::string in_gpu_texture_name,
		GPU_RESOURCE_LAYOUT::GPU_RESOURCE_TYPE in_resoure_type) = 0;

	virtual void clear_gpu_texture(gpu_resource_element* in_gpu_resource_element) = 0;

	virtual void init(HWND in_main_wnd) = 0;

	virtual void over() = 0;
};

