#pragma once
#ifdef  DLL_GRAPHICS_API
#else
#define DLL_GRAPHICS_API _declspec(dllexport)
#endif
#include "Core/Render_Core/directx_render.h"
#include "stre_render.h"
#include <queue>
#include <functional>

//�����ˣ���
typedef std::function<void(directx_render_abstract* in_render)>
	dx_function;


//functor
template<class t_function>
class function_command
{
public:

	std::queue<t_function> command_queue;
};

static function_command<dx_function> dx_pass_command;

static function_command<dx_function> dx_shader_resource_command;

//����ģʽ ѡ����Ⱦ����
template<typename t_render>
class render_system : public s_render_system
{
public:
	//render_system(HINSTANCE in_instance) { init(in_instance); };
	render_system() {};
protected:
	
	//������
	render_system(const render_system&) {};
	render_system& operator=(const render_system&) {};
	~render_system()
	{
		over();
	};
protected:
	s_window* render_window = nullptr;
	t_render* renderer = nullptr;

public:

	void draw_pass(const s_pass* in_pass);

	//��������ˢ����
	void update_gpu_memory();

private:
	void init(HINSTANCE in_instance, UINT in_width, UINT in_height);

	void over() {};
};


