#include "stre_engine.h"

//用库的功能再封装成功能函数

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
bool stre_engine::update_gpu_memory()
{
	return render_system_instance->update_gpu_memory();
}

void stre_engine::draw_pass(s_pass* in_pass)
{
	if (!in_pass->gpu_pass_ptr)
	{
		return;
	}
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

bool stre_engine::release_pass(s_pass* in_out_pass)
{
	return pass_fy->release_gpu_pass(in_out_pass);
}

bool stre_engine::check_pass(s_pass* in_out_pass)
{
	return pass_fy->check_pass(in_out_pass);
}

void stre_engine::update_texture_gpu(cpu_texture* in_texture)
{
	//类型改变在in_texture->gpu_sr_ptr中
	texture_manager->dx_allocate_gpu_resource(in_texture, in_texture->gpu_sr_ptr->shader_resource_type);
}

void stre_engine::update_mesh_gpu(cpu_mesh* in_mesh)
{
	mesh_manager->update_gpu(in_mesh);
}

cpu_texture* stre_engine::create_texture(gpu_shader_resource::SHADER_RESOURCE_TYPE in_sr_type)
{
	auto texture_ptr = texture_manager->create_resource();
	
	//提交分配分配命令
	texture_manager->dx_allocate_gpu_resource(texture_ptr, in_sr_type);
	
	return texture_ptr;
}

cpu_mesh* stre_engine::create_viewport_mesh()
{
	auto mesh_ptr = mesh_manager->create_resource();
	mesh_ptr->is_view_mesh = true;
	mesh_ptr->vertex_ptr = vertex_manager->create_resource(4,true);
	mesh_ptr->index_ptr = index_manager->create_resource(6, true);
	set_screen_vertex_index(mesh_ptr);

	mesh_manager->dx_allocate_gpu_resource(mesh_ptr);
	mesh_manager->update_gpu(mesh_ptr);
	return mesh_ptr;
}

cpu_mesh* stre_engine::create_mesh_from_fbx(std::wstring path)
{
	auto mesh_ptr = mesh_manager->load_fbx(path.c_str());
	mesh_ptr->is_view_mesh = false;
	mesh_manager->dx_allocate_gpu_resource(mesh_ptr);
	update_mesh_gpu(mesh_ptr);
	return mesh_ptr;

}

s_pass* stre_engine::create_pass()
{
	auto pass_ptr = pass_fy->create_pass();
	return pass_ptr;
}

bool stre_engine::pass_add_render_target(s_pass* in_out_pass, cpu_texture* in_texture)
{
	return pass_fy->add_render_target(in_out_pass, in_texture);
}

bool stre_engine::pass_add_mesh(s_pass* in_out_pass, cpu_mesh* in_mesh)
{
	return pass_fy->add_mesh(in_out_pass, in_mesh);
}

bool stre_engine::pass_set_shader_layout(s_pass* in_out_pass, shader_layout& in_shader_layout)
{
	return pass_fy->set_shader_layout(in_out_pass, in_shader_layout);
}

bool stre_engine::pass_remove_render_target(s_pass* in_out_pass, cpu_texture* in_texture)
{
	return pass_fy->remove_render_target(in_out_pass, in_texture);
}
bool stre_engine::pass_remove_mesh(s_pass* in_out_pass, cpu_mesh* in_mesh)
{
	return pass_fy->remove_mesh(in_out_pass, in_mesh);
}
bool stre_engine::pass_remove_shader_layout(s_pass* in_out_pass)
{
	return pass_fy->remove_shader_layout(in_out_pass);
}


void stre_engine::package_textures(std::vector<cpu_texture*> in_textures, cpu_texture* in_out_package_container)
{
	//重新分配贴图组的类型
	update_texture_gpu(in_out_package_container);

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