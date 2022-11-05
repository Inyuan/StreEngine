#include "Rendering/stre_render.h"

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
	
	//构建pass
	pass_factory pass_factory_instance;

	auto debug_pass = pass_factory_instance.create_pass();

	//构建贴图
	auto texture_manager_instance = resource_manager_factory().create_texture_manager();

	auto debug_texture_rt = texture_manager_instance->create_resource();
	auto debug_texture_ds = texture_manager_instance->create_resource();

	texture_manager_instance->dx_allocate_gpu_resource(debug_texture_rt,gpu_shader_resource::SHADER_RESOURCE_TYPE_RENDER_TARGET);
	texture_manager_instance->dx_allocate_gpu_resource(debug_texture_ds, gpu_shader_resource::SHADER_RESOURCE_TYPE_RENDER_DEPTH_STENCIL);

	
	//debug_pass->gpu_shader_layout

	//pass_factory_instance.add_mesh(debug_pass,);

	//pass_factory_instance.add_shader_resource(debug_pass, );

	//pass_factory_instance.set_shader_layout(debug_pass,);

	//添加渲染目标
	pass_factory_instance.add_render_target(debug_pass, debug_texture_rt);
	//添加渲染目标
	pass_factory_instance.add_render_target(debug_pass, debug_texture_ds);

	
}