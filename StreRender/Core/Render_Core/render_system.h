#pragma once
#include "render.h"
#include "Core/Memory/s_memory.h"
#include "Core/Process/BProcess.h"
#include "Resource/GPU_Resource/constant_pass_factory.h"

class render_system
{
protected:
	s_window* render_window;
	render* renderer;
	s_memory* memory_allocater;
	//BProcess* render_window_process;

public:

	void add_pass();

	void allocate_gpu();

	void init(HINSTANCE in_instance,REDNER_API in_render_api);
	void over();
};