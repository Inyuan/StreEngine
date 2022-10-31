#pragma once
#include "Core/Render_Core/directx_render.h"
#include "Core/Memory/s_memory.h"
#include "cpu_resource.h"
#include <queue>
#include <functional>

//重名了？？
typedef std::function<void(directx_render* in_render)> 
	dx_pass_function;

typedef std::function<void(directx_render* in_render)>
	dx_shader_resource_function;

//functor
template<class t_function>
class function_command
{
public:

	static std::queue<t_function> command_queue;
};



s_memory_allocater_register render_memory_allocater("render_memory_allocater");

//策略模式 选择渲染策略
template<typename t_render>
class render_system
{
public:
	render_system(HINSTANCE in_instance) { init(in_instance); };
protected:
	render_system() {};
	//不许复制
	render_system(const render_system&);
	render_system& operator=(const render_system&);
	~render_system()
	{
		over();
	};
protected:
	s_window* render_window = nullptr;
	t_render* renderer = nullptr;

public:

	void draw_pass(const s_pass* in_pass)
	{
		renderer->draw_pass(in_pass);
	}

	//遍历所有刷新数
	void update_gpu_memory()
	{
		while (!function_command<dx_pass_function>::command_queue.empty())
		{
			auto& update_function = function_command<dx_pass_function>::command_queue.front();
			//执行刷新函数
			update_function.operator()(renderer);

			function_command<dx_pass_function>::command_queue.pop();
		}

		while (!function_command<dx_shader_resource_function>::command_queue.empty())
		{
			auto& update_function = function_command<dx_shader_resource_function>::command_queue.front();
			//执行刷新函数
			update_function.operator()(renderer);

			function_command<dx_shader_resource_function>::command_queue.pop();
		}

	}

private:
	void init(HINSTANCE in_instance, UINT in_width, UINT in_height)
	{
		s_memory* memory_allocater = memory_allocater_group["render_memory_allocater"];

		//gpu_res_factory = memory_allocater->allocate<gpu_resource_factory>();

		render_window = memory_allocater->allocate<s_window>();

		render_window->init(in_instance, in_width, in_height);

		renderer = (render*)memory_allocater->allocate<t_render>();

		renderer->init(render_window->get_hwnd());
	}

	void over() {};
};


