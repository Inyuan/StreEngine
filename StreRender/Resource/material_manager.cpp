#ifdef  DLL_GRAPHICS_API
#else
#define DLL_GRAPHICS_API _declspec(dllexport)
#endif

#include "stre_render.h"
#include "Core/Memory/s_memory.h"

/***
************************************************************
*
* Create & Load Function
*
************************************************************
*/

s_memory_allocater_register material_resource_allocater("material_resource_allocater");

template<class t_render>
cpu_material* custom_manager<cpu_material, t_render>::create_resource()
{
	auto allocater = memory_allocater_group["material_resource_allocater"];

	return allocater->allocate<cpu_material>();

}

template<class t_render>
cpu_material* custom_manager<cpu_material, t_render>::load_resource(wchar_t* in_path)
{

}




/***
************************************************************
*
* Update Function
*
************************************************************
*/

template<class t_render>
void custom_manager<cpu_material, t_render>::change_texture()
{
	
}

template<class t_render>
void custom_manager<cpu_material, t_render>::add_texture()
{

}

template<class t_render>
void custom_manager<cpu_material, t_render>::release_texture()
{

}