#ifdef  DLL_GRAPHICS_API
#else
#define DLL_GRAPHICS_API _declspec(dllexport)
#endif

#include "stre_render.h"
#include "render_system.h"
#include "Core/Render_Core/directx_render.h"
#include "Core/Memory/s_memory.h"
#include "render_functor.h"

function_command<dx_function> dx_pass_command;

function_command<dx_function> dx_shader_resource_command;


template<>
s_render_system* render_factory::create_render_system<s_directx_render>()
{
	return new render_system<directx_render>();
}


template<typename t_render>
void render_system<t_render>::draw_pass(s_pass* in_pass)
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

//��������˳��
//�����ڴ棬
//����pass��
//ˢ���ڴ棬
//ˢ��pass��
//����
//�ύ����
//������

//��������ˢ����

void render_system<directx_render>::update_gpu_memory()
{

	while (!dx_pass_command.command_queue.empty())
	{
		auto& update_function = dx_pass_command.command_queue.front();
		//ִ��ˢ�º���
		update_function.operator()(renderer);

		dx_pass_command.command_queue.pop();
	}


	while (!dx_shader_resource_command.command_queue.empty())
	{
		auto& update_function = dx_shader_resource_command.command_queue.front();
		//ִ��ˢ�º���
		update_function.operator()(renderer);

		dx_shader_resource_command.command_queue.pop();
	}

}

void render_system<directx_render>::init_in_HWND(HWND in_HWND, UINT in_width, UINT in_height)
{
	renderer = new directx_render();

	renderer->init(in_HWND, in_width, in_height);
}

void render_system<directx_render>::init_new_window(HINSTANCE in_instance, UINT in_width, UINT in_height)
{
	//s_memory* memory_allocater = memory_allocater_group["render_memory_allocater"];

	//gpu_res_factory = memory_allocater->allocate<gpu_resource_factory>();

	//render_window = memory_allocater->allocate<s_window>();

	render_window = new s_window();

	render_window->init(in_instance, in_width, in_height);

	//renderer = (render*)memory_allocater->allocate<t_render>();

	renderer = new directx_render();

	renderer->init(render_window->get_hwnd(), in_width, in_height);
}


void render_system<directx_render>::execute_command()
{
	renderer->excute_command_list();

	renderer->switch_swap_chain();

	renderer->reset_command_allocator();
}







