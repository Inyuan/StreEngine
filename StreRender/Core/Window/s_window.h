#pragma once

#include <windows.h>
#include <WindowsX.h>
#include <string>


//??? 需要管理窗体数组?
//
// 1.构建窗体
// 2.消息函数重载，对各种进行操作
class s_window
{
public:
	bool init(HINSTANCE in_instance, UINT in_width, UINT in_height);

	LRESULT massage_proc(HWND in_hwnd, UINT in_msg, WPARAM in_wparam, LPARAM in_lparam);

	static s_window* get_instance();

	HWND get_hwnd() { return mian_window_handle; };

	int get_width() { return client_width; };

	int get_height() { return client_height; };

	//void SetConctrolFunction(void (*IOnMouseDown_ptr)(WPARAM, int, int),
	//	void (*IOnMouseUp_ptr)(WPARAM, int, int),
	//	void (*IOnMouseMove_ptr)(WPARAM, int, int))
	//{
	//	InputFunction_OnMouseDown_ptr = IOnMouseDown_ptr;
	//	InputFunction_OnMouseUp_ptr = IOnMouseUp_ptr;
	//	InputFunction_OnMouseMove_ptr = IOnMouseMove_ptr;
	//}

private:

	static s_window* window_instance;

	std::wstring main_window_caption = L"Main Window";
	HINSTANCE instance_handle = nullptr; // application instance handle
	HWND      mian_window_handle = nullptr; // main window handle
	bool      app_paused = false;  // is the application paused?
	bool      minimized = false;  // is the application minimized?
	bool      maximized = false;  // is the application maximized?
	bool      resizing = false;   // are the resize bars being dragged?
	bool      full_screen_state = false;// fullscreen enabled

	int client_width = 800;
	int client_height = 600;

	//void (*InputFunction_OnMouseDown_ptr)(WPARAM, int, int);
	//void (*InputFunction_OnMouseUp_ptr)(WPARAM, int, int);
	//void (*InputFunction_OnMouseMove_ptr)(WPARAM, int, int);

};