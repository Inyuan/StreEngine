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
	return renderer->allocate_pass(in_constant_pass_layout);
}


s_memory_allocater_register gpu_resource_ptr_allocater("gpu_resource_ptr_allocater");

//按照GPU_RESOURCE_TYPE
//将物体的CPU资源复制到GPU上
//每个type有自己的资源数组
gpu_resource* render_system::allocate_gpu_memory(cg_resource* in_resource)
{
	auto allocater = memory_allocater_group["gpu_resource_ptr_allocater"];

	gpu_resource* gpu_resource_ptr = (gpu_resource*)allocater->allocate<gpu_resource>();
	gpu_resource_ptr->name = in_resource->get_name();

	//遍历每种类型往里面塞
	for (int i = 0; i < GPU_RESOURCE_LAYOUT::GPU_RESOURCE_TYPE::GPU_RES_TYPE_NUMBER; i++)
	{
		auto it = in_resource->resource_gpu_layout[i].begin();

		for (; it != in_resource->resource_gpu_layout[i].end(); it++)
		{
			auto gpu_res_type = it->gpu_resource_type;
			gpu_resource_ptr->gpu_resource_group[
				gpu_res_type]
				.push_back(renderer->allocate_gpu_memory(
					*it));
		}
	}


	return gpu_resource_ptr;
}

//必须是 RT 或者Depth
gpu_resource* render_system::create_gpu_texture(
	std::string in_gpu_texture_name, 
	GPU_RESOURCE_LAYOUT::GPU_RESOURCE_TYPE in_resoure_type)
{
	typedef GPU_RESOURCE_LAYOUT::GPU_RESOURCE_TYPE GPU_RES_TYPE;

	switch (in_resoure_type)
	{
	case GPU_RES_TYPE::GPU_RES_RENDER_TARGET:
	case GPU_RES_TYPE::GPU_RES_DEPTH_STENCIL:

		auto allocater = memory_allocater_group["gpu_resource_ptr_allocater"];

		gpu_resource* gpu_resource_ptr = (gpu_resource*)allocater->allocate<gpu_resource>();
		gpu_resource_ptr->name = in_gpu_texture_name;

		gpu_resource_ptr->gpu_resource_group[in_resoure_type].push_back(renderer->create_gpu_texture(in_gpu_texture_name, in_resoure_type));
		return gpu_resource_ptr;
		break;
	}

	return nullptr;

}

void render_system::clear_gpu_texture(gpu_resource* in_gpu_resource)
{
	typedef GPU_RESOURCE_LAYOUT::GPU_RESOURCE_TYPE GPU_RES_TYPE;

	for (auto it : in_gpu_resource->gpu_resource_group[GPU_RES_TYPE::GPU_RES_RENDER_TARGET])
	{

	}

	for (auto it : in_gpu_resource->gpu_resource_group[GPU_RES_TYPE::GPU_RES_DEPTH_STENCIL])
	{

	}
}