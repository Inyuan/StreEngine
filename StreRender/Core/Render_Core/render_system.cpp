#include "render_system.h"


void render_system::init(HINSTANCE in_instance, REDNER_API in_render_api)
{
	memory_allocater = new s_memory();
	
	render_window = memory_allocater->allocate<s_window>();

	render_window->init(in_instance, render_configuration::client_width, render_configuration::client_height);

	switch (in_render_api)
	{
	case DIRECTX_RENDER:
		renderer = memory_allocater->allocate<directx_render>();
		renderer->init(render_window->get_hwnd());
		break;
	default://DirectX
		renderer = memory_allocater->allocate<directx_render>();
		renderer->init(render_window->get_hwnd());
		break;
	}
}