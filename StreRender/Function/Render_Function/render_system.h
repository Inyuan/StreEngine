#pragma once
#ifdef  DLL_GRAPHICS_API
#else
#define DLL_GRAPHICS_API _declspec(dllexport)
#endif
#include "Core/Render_Core/directx_render.h"
#include "gpu_resource.h"

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

	virtual void draw_pass(s_pass* in_pass) override;

	//��������ˢ����
	virtual void update_gpu_memory() override;

	virtual void execute_command() override;

private:
	virtual void init_in_HWND(HWND in_HWND, UINT in_width, UINT in_height) override;

	virtual void init_new_window(HINSTANCE in_instance, UINT in_width, UINT in_height) override;

	virtual void over() override  {};
};



