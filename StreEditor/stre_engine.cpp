#include "stre_engine.h"


void set_screen_vertex_index(cpu_mesh* in_cpu_mesh);

void stre_engine::render_system_init(
	HWND in_HWND,
	UINT in_width, 
	UINT in_height)
{
	render_system_instance = render_factory().create_render_system<s_directx_render>();
	render_system_instance->init_in_HWND(in_HWND, in_width, in_height);
}

//多线程的这个位置要等
void stre_engine::update_gpu_memory()
{
	render_system_instance->update_gpu_memory();
}

void stre_engine::draw_pass(s_pass* in_pass)
{
	render_system_instance->draw_pass(in_pass);
}

void stre_engine::execute_command()
{
	render_system_instance->execute_command();
}

bool stre_engine::allocate_pass(s_pass* in_out_pass)
{
	pass_fy->dx_allocate_gpu_pass(in_out_pass);
	return true;
}



cpu_texture* stre_engine::create_texture(gpu_shader_resource::SHADER_RESOURCE_TYPE in_sr_type)
{
	auto texture_ptr = texture_manager->create_resource();
	
	//偷懒直接分配
	texture_manager->dx_allocate_gpu_resource(texture_ptr, in_sr_type);
	
	return texture_ptr;
}

cpu_mesh* stre_engine::create_viewport_mesh()
{
	auto mesh_ptr = mesh_manager->create_resource();

	mesh_ptr->vertex_ptr = vertex_manager->create_resource(4);
	mesh_ptr->index_ptr = index_manager->create_resource(6);
	set_screen_vertex_index(mesh_ptr);

	mesh_manager->dx_allocate_gpu_resource(mesh_ptr);
	mesh_manager->update_gpu(mesh_ptr);
	return mesh_ptr;
}

s_pass* stre_engine::create_pass()
{
	auto pass_ptr = pass_fy->create_pass();
	return pass_ptr;
}


bool stre_engine::pass_add_render_target(s_pass* in_out_pass, cpu_texture* in_texture)
{
	update_gpu_memory();//多线程的这个位置要等
	return pass_fy->add_render_target(in_out_pass, in_texture);
}

bool stre_engine::pass_add_mesh(s_pass* in_out_pass, cpu_mesh* in_mesh)
{
	update_gpu_memory();//多线程的这个位置要等
	return pass_fy->add_mesh(in_out_pass, in_mesh);
}

bool stre_engine::pass_set_shader_layout(s_pass* in_out_pass, shader_layout& in_shader_layout)
{
	update_gpu_memory();//多线程的这个位置要等
	return pass_fy->set_shader_layout(in_out_pass, in_shader_layout);
}

void stre_engine::package_textures(std::vector<cpu_texture*> in_textures, cpu_texture* in_out_package_container)
{
	update_gpu_memory();//多线程的这个位置要等
	texture_manager->package_textures(in_textures,in_out_package_container);
}










void set_screen_vertex_index(cpu_mesh* in_cpu_mesh)
{
	auto debug_vertex_ptr = in_cpu_mesh->vertex_ptr->get_data();
	debug_vertex_ptr[0].position = s_float3(-1.0f, 1.0f, 0.0f);
	debug_vertex_ptr[1].position = s_float3(1.0f, 1.0f, 0.0f);
	debug_vertex_ptr[2].position = s_float3(-1.0f, -1.0f, 0.0f);
	debug_vertex_ptr[3].position = s_float3(1.0f, -1.0f, 0.0f);

	debug_vertex_ptr[0].normal = s_float3(0, 0.0f, -1.0f);
	debug_vertex_ptr[1].normal = s_float3(0, 0.0f, -1.0f);
	debug_vertex_ptr[2].normal = s_float3(0, 0.0f, -1.0f);
	debug_vertex_ptr[3].normal = s_float3(0, 0.0f, -1.0f);

	debug_vertex_ptr[0].texC = s_float2(0.0f, 0.0f);
	debug_vertex_ptr[1].texC = s_float2(1.0f, 0.0f);
	debug_vertex_ptr[2].texC = s_float2(0.0f, 1.0f);
	debug_vertex_ptr[3].texC = s_float2(1.0f, 1.0f);

	auto debug_index_ptr = in_cpu_mesh->index_ptr->get_data();

	debug_index_ptr[0] = 0;
	debug_index_ptr[1] = 1;
	debug_index_ptr[2] = 2;
	debug_index_ptr[3] = 1;
	debug_index_ptr[4] = 2;
	debug_index_ptr[5] = 3;

	in_cpu_mesh->mesh_element_index_count.push_back(6);
}