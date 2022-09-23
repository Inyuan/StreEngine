#include "render_system.h"

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



gpu_resource* render_system::allocate_gpu_memory(s_mesh_object* in_mesh_object)
{

}

gpu_resource* render_system::allocate_gpu_memory(s_camera* in_camera)
{

}

gpu_resource* render_system::allocate_gpu_memory(s_light* in_light)
{

}

gpu_resource* render_system::create_gpu_texture(std::string in_gpu_texture_name)
{

}
