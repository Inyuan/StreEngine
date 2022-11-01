#include "render_system.h"
#include "Core/Render_Core/directx_render.h"
#include "Core/Memory/s_memory.h"

s_memory_allocater_register render_memory_allocater("render_memory_allocater");

template<typename t_render>
void render_system<t_render>::draw_pass(const s_pass* in_pass)
{
	renderer->draw_pass(in_pass);
}

//遍历所有刷新数
template<typename t_render>
void render_system<t_render>::update_gpu_memory()
{
	while (!dx_pass_command.command_queue.empty())
	{
		auto& update_function = dx_pass_command.command_queue.front();
		//执行刷新函数
		update_function.operator()(renderer);

		dx_pass_command.command_queue.pop();
	}

	while (!dx_shader_resource_command.command_queue.empty())
	{
		auto& update_function = dx_shader_resource_command.command_queue.front();
		//执行刷新函数
		update_function.operator()(renderer);

		dx_shader_resource_command.command_queue.pop();
	}

}

template<typename t_render>
void render_system<t_render>::init(HINSTANCE in_instance, UINT in_width, UINT in_height)
{
	s_memory* memory_allocater = memory_allocater_group["render_memory_allocater"];

	//gpu_res_factory = memory_allocater->allocate<gpu_resource_factory>();

	render_window = memory_allocater->allocate<s_window>();

	render_window->init(in_instance, in_width, in_height);

	renderer = (render*)memory_allocater->allocate<t_render>();

	renderer->init(render_window->get_hwnd());
}







