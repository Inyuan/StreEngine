#ifdef  DLL_GRAPHICS_API
#else
#define DLL_GRAPHICS_API _declspec(dllexport)
#endif

#include "stre_render.h"
#include "render_system.h"
#include "Core/Render_Core/directx_render.h"
#include "Core/Memory/s_memory.h"


template<>
s_render_system* render_factory::create_render_system<s_directx_render>()
{
	return new render_system<directx_render>();
}


template<typename t_render>
void render_system<t_render>::draw_pass(const s_pass* in_pass)
{
	renderer->draw_pass(in_pass);
}


/***
************************************************************
*
* DX Instance
*
************************************************************
*/

//必须遵守顺序
//分配内存，
//分配pass，
//刷新内存，
//刷新pass，
//绘制
//提交命令
//交换链

//遍历所有刷新数

void render_system<directx_render>::update_gpu_memory()
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


void render_system<directx_render>::init(HINSTANCE in_instance, UINT in_width, UINT in_height)
{
	//s_memory* memory_allocater = memory_allocater_group["render_memory_allocater"];

	//gpu_res_factory = memory_allocater->allocate<gpu_resource_factory>();

	//render_window = memory_allocater->allocate<s_window>();

	render_window = new s_window();

	render_window->init(in_instance, in_width, in_height);

	//renderer = (render*)memory_allocater->allocate<t_render>();

	renderer = new directx_render();

	renderer->init(render_window->get_hwnd());
}








