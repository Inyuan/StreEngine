#pragma once
#include "Core/Window/s_window.h"
#include "stre_pass.h"

class render
{
public:

	//所有资源按照要求安装好了 直接执行pass
	virtual void draw_pass(const s_pass* in_pass) = 0;

	virtual gpu_pass* allocate_pass() = 0;

	virtual gpu_shader_resource* allocate_shader_resource(
		gpu_shader_resource::SHADER_RESOURCE_TYPE in_shader_res_type) = 0;

	virtual gpu_resource_element* create_gpu_texture(
		std::string in_gpu_texture_name,
		GPU_RESOURCE_LAYOUT::GPU_RESOURCE_TYPE in_resoure_type) = 0;

	virtual void clear_gpu_texture(gpu_resource_element* in_gpu_resource_element) = 0;

	virtual void init(HWND in_main_wnd) = 0;

	virtual void over() = 0;
};

