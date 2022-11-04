#include "Rendering/stre_render.h"



void main()
{

	
	pass_factory pass_factory_instance;
	auto pass = pass_factory_instance.create_pass();

	cpu_material aaa111;

	render_factory r_fy;

	s_render_system* r_sys;
	r_sys = r_fy.create_render_system<s_directx_render>();

	resource_manager_factory res_m_fy;

	auto m_m = res_m_fy.create_material_manager();
	m_m->update_gpu(&aaa111);
}