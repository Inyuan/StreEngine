#pragma once
#ifdef  DLL_GRAPHICS_API
#else
#define DLL_GRAPHICS_API _declspec(dllexport)
#endif
#include "Core/Render_Core/directx_render.h"
#include "render_functor.h"
#include "gpu_resource.h"

//策略模式 选择渲染策略
template<typename t_render>
class render_system : public s_render_system
{
public:
	//render_system(HINSTANCE in_instance) { init(in_instance); };
	render_system() {};
protected:
	
	//不许复制
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

	//遍历所有刷新数
	void update_gpu_memory();


private:
	void init(HINSTANCE in_instance, UINT in_width, UINT in_height);

	void over() {};
};


