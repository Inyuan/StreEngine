#ifdef  DLL_GRAPHICS_API
#else
#define DLL_GRAPHICS_API _declspec(dllexport)
#endif

#include "resource_factory.h"

template<typename t_cpu_res_type>
s_custom_manager<t_cpu_res_type>* resource_manager_factory::create_manager()
{
	return new custom_manager<t_cpu_res_type>();
}
 
s_material_manager* resource_manager_factory::create_material_manager()
{
	return new material_manager();
}

s_texture_manager* resource_manager_factory::create_texture_manager()
{
	return new texture_manager();
}

s_mesh_manager* resource_manager_factory::create_mesh_manager()
{
	return new mesh_manager();
}

s_sence_manager* resource_manager_factory::create_sence_manager()
{
	return new sence_manager();
} 

template<>
s_custom_manager<cpu_vertex>* resource_manager_factory::create_manager<cpu_vertex>()
{
	return new custom_manager<cpu_vertex>;
}

template<>
s_custom_manager<cpu_index>* resource_manager_factory::create_manager<cpu_index>()
{
	return new custom_manager<cpu_index>;
}

template<>
s_custom_manager<cpu_camera>* resource_manager_factory::create_manager<cpu_camera>()
{
	return new custom_manager<cpu_camera>;
}

template<>
s_custom_manager<cpu_light>* resource_manager_factory::create_manager<cpu_light>()
{
	return new custom_manager<cpu_light>;
}



