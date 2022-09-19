#pragma once
#include "render.h"
#include "Core/Memory/s_memory.h"
#include "Core/Process/BProcess.h"
class render_system
{
protected:
	s_window* render_window;
	render* renderer;
	s_memory* memory_allocater;
	BProcess* render_window_process;
public:

	void init(HINSTANCE in_instance,REDNER_API in_render_api);
	void over();
};