#pragma once
#include "directx_render.h"
#include "Core/Window/s_window.h"



class render
{
public:
	virtual void draw_call() = 0;

	virtual void allocate_pass() = 0;

	virtual void init(HWND in_main_wnd) = 0;

	virtual void over() = 0;
};

