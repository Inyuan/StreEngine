#include "render_system.h"
#include "Core/Render_Core/directx_render.h"
#include "Resource/cg_resource_factory.h"
s_memory_allocater_register render_memory_allocater("render_memory_allocater");

void render_system::init(HINSTANCE in_instance, REDNER_API in_render_api)
{
	memory_allocater = memory_allocater_group["render_memory_allocater"];

	//gpu_res_factory = memory_allocater->allocate<gpu_resource_factory>();

	render_window = memory_allocater->allocate<s_window>();

	render_window->init(in_instance, CLIENT_WIDTH, CLIENT_HEIGHT);

	switch (in_render_api)
	{
	case DIRECTX_RENDER:
		renderer = (render*)memory_allocater->allocate<directx_render>();
		renderer->init(render_window->get_hwnd());
		break;
	default://DirectX
		renderer = (render*)memory_allocater->allocate<directx_render>();
		renderer->init(render_window->get_hwnd());
		break;
	}
}

constant_pass* render_system::allocate_pass(constant_pass::pass_layout in_constant_pass_layout)
{

}


s_memory_allocater_register gpu_resource_ptr_allocater("gpu_resource_ptr_allocater");

//将物体的CPU资源复制到GPU上
gpu_resource* render_system::allocate_gpu_memory(cg_resource* in_resource)
{
	auto allocater = memory_allocater_group["gpu_resource_ptr_allocater"];

	gpu_resource* gpu_resource_ptr = (gpu_resource*)allocater->allocate<gpu_resource>();

	auto it = in_resource->resource_gpu_layout.begin();
	for (; it != in_resource->resource_gpu_layout.end(); it++)
	{
		gpu_resource_ptr->gpu_resource_group[it->first]
			= renderer->allocate_gpu_memory(it->second);
	}

	switch (in_resource->get_type())
	{
	case s_resource::RESOURCE_TYPE::RES_EMPTY_OBJECT:
		//包围盒线条
		break;
	case s_resource::RESOURCE_TYPE::RES_CAMERA:
		//构建cameraCB
		break;
	case s_resource::RESOURCE_TYPE::RES_LIGHT:
		break;
	case s_resource::RESOURCE_TYPE::RES_MATERIAL:
		break;
	case s_resource::RESOURCE_TYPE::RES_TEXTURE:
		break;
	case s_resource::RESOURCE_TYPE::RES_SENCE:
		break;
	case s_resource::RESOURCE_TYPE::RES_STATIC_OBJECT:
		break;
	case s_resource::RESOURCE_TYPE::RES_DYNAMIC_OBJECT:
		break;
	}

	return gpu_resource_ptr;
}

gpu_resource* render_system::create_gpu_texture(std::string in_gpu_texture_name)
{

}
