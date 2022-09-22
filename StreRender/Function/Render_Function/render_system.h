#pragma once
#include "Core/Render_Core/render.h"
#include "Core/Memory/s_memory.h"
#include "Core/Process/BProcess.h"
#include "Core/Render_Core/Resource/constant_pass.h"

#include <vector>

class render_system
{
protected:
	s_window* render_window = nullptr;
	render* renderer = nullptr;
	s_memory* memory_allocater = nullptr;
	//BProcess* render_window_process;

public:

	constant_pass* allocate_pass(constant_pass::pass_layout in_constant_pass_layout);

	void allocate_gpu_memory();

	void init(HINSTANCE in_instance,REDNER_API in_render_api);
	void over();
};