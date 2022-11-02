#ifdef  DLL_GRAPHICS_API
#else
#define DLL_GRAPHICS_API _declspec(dllexport)
#endif

#include "stre_render.h"
#include "Core/Memory/s_memory.h"
////////////////////////////////////////////////////////////////////
//create object

s_memory_allocater_register texture_resource_allocater("texture_resource_allocater");

template<class t_render>
cpu_texture* custom_manager<cpu_texture, t_render>::create_resource()
{
	auto allocater = memory_allocater_group["texture_resource_allocater"];

	return allocater->allocate<cpu_texture>();

}

template<class t_render>
cpu_texture* custom_manager<cpu_texture, t_render>::load_resource(wchar_t* in_path)
{

}

template<class t_render>
void custom_manager<cpu_texture, t_render>::update_gpu(const cpu_texture* in_cpu_data)
{

}