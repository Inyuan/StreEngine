#pragma once
#include "directx_render.h"
#include "Core/Window/s_window.h"



class render
{
public:
	virtual void init(HWND in_main_wnd) = 0;
	virtual void over() = 0;
};

