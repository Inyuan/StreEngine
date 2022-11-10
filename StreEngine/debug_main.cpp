#include "Rendering/stre_render.h"


void set_screen_vertex_index(cpu_mesh * in_cpu_mesh);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance,
	PSTR cmdLine, int showCmd)
{
	//Debug
	wchar_t* path = new wchar_t[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, path);
	printf("%s\n", path);

	//构建渲染器
	auto render_system_instance = render_factory().create_render_system<s_directx_render>();

	render_system_instance->init(hInstance,800,600);
	
	resource_manager_factory res_m_fy;

	//构建贴图
	auto texture_manager_instance = res_m_fy.create_texture_manager();

	auto debug_texture_rt = texture_manager_instance->create_resource();
	auto debug_texture_rt_group = texture_manager_instance->create_resource();
	auto debug_texture_ds = texture_manager_instance->create_resource();
	auto debug_texture_ds_group = texture_manager_instance->create_resource();

	texture_manager_instance->dx_allocate_gpu_resource(debug_texture_rt, gpu_shader_resource::SHADER_RESOURCE_TYPE_RENDER_TARGET);
	texture_manager_instance->dx_allocate_gpu_resource(debug_texture_rt_group, gpu_shader_resource::SHADER_RESOURCE_TYPE_RENDER_TARGET_GROUP);
	texture_manager_instance->dx_allocate_gpu_resource(debug_texture_ds, gpu_shader_resource::SHADER_RESOURCE_TYPE_RENDER_DEPTH_STENCIL);
	texture_manager_instance->dx_allocate_gpu_resource(debug_texture_ds_group, gpu_shader_resource::SHADER_RESOURCE_TYPE_RENDER_DEPTH_STENCIL_GROUP);

	std::vector<cpu_texture*> debug_input_rt_group;
	debug_input_rt_group.push_back(debug_texture_rt);
	texture_manager_instance->package_textures(debug_input_rt_group, debug_texture_rt_group);
	std::vector<cpu_texture*> debug_input_ds_group;
	debug_input_ds_group.push_back(debug_texture_ds);
	texture_manager_instance->package_textures(debug_input_ds_group, debug_texture_ds_group);
	
	//构建mesh 
	auto mesh_manager_instance = res_m_fy.create_mesh_manager();

	auto debug_mesh = mesh_manager_instance->create_resource();

	auto vertex_manager_instance = res_m_fy.create_vertex_manager();
	auto index_manager_instance = res_m_fy.create_index_manager();

	debug_mesh->vertex_ptr = vertex_manager_instance->create_resource(4);
	debug_mesh->index_ptr = index_manager_instance->create_resource(6);
	set_screen_vertex_index(debug_mesh);

	mesh_manager_instance->dx_allocate_gpu_resource(debug_mesh);
	mesh_manager_instance->update_gpu(debug_mesh);
	//执行渲染
	render_system_instance->update_gpu_memory();

	//构建pass

	pass_factory pass_factory_instance;

	auto debug_pass = pass_factory_instance.create_pass();

	shader_layout debug_shader_layout;
	debug_shader_layout.shader_vaild[shader_layout::VS] = true;
	debug_shader_layout.shader_vaild[shader_layout::PS] = true;
	debug_shader_layout.shader_path[shader_layout::VS] = L"Shaders\\debug_pass.hlsl";
	debug_shader_layout.shader_path[shader_layout::PS] = L"Shaders\\debug_pass.hlsl";
	debug_shader_layout.shader_input_group.push_back(
		{ "POSITION",shader_layout::shader_input::INPUT_ELEMENT_SIZE_R32G32B32 });
	debug_shader_layout.shader_input_group.push_back(
		{ "NORMAL",shader_layout::shader_input::INPUT_ELEMENT_SIZE_R32G32B32 });
	debug_shader_layout.shader_input_group.push_back(
		{ "TEXCOORD",shader_layout::shader_input::INPUT_ELEMENT_SIZE_R32G32 });
	debug_shader_layout.shader_input_group.push_back(
		{ "TANGENT",shader_layout::shader_input::INPUT_ELEMENT_SIZE_R32G32B32 });

	//debug_pass->gpu_shader_layout

	pass_factory_instance.add_mesh(debug_pass, debug_mesh);

	//pass_factory_instance.add_shader_resource(debug_pass, );

	pass_factory_instance.set_shader_layout(debug_pass, debug_shader_layout);

	debug_pass->is_output = true;

	//添加渲染目标
	pass_factory_instance.add_render_target(debug_pass, debug_texture_rt_group);
	//添加渲染目标
	pass_factory_instance.add_render_target(debug_pass, debug_texture_ds_group);

	pass_factory_instance.dx_allocate_gpu_pass(debug_pass);


	//执行渲染
	render_system_instance->update_gpu_memory();

	while (1)
	{
		render_system_instance->update_gpu_memory();

		render_system_instance->draw_pass(debug_pass);

		//执行渲染
		render_system_instance->execute_command();
	}
}


void set_screen_vertex_index(cpu_mesh* in_cpu_mesh)
{
	auto debug_vertex_ptr = in_cpu_mesh->vertex_ptr->get_data();
	debug_vertex_ptr[0].position = s_float3( -1.0f,1.0f, 0.0f);
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