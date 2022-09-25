#include "deferred_rendering.h"

//利用render_system给物体分配GPU内存
//利用render_system构建多个pass
//利用render_system构建rt
//利用render_system设定输出目标

//延迟渲染
void deferred_rendering::init(HINSTANCE in_instance, REDNER_API in_render_api)
{
	typedef GPU_RESOURCE_LAYOUT::GPU_RESOURCE_TYPE GPU_RES_TYPE;
	typedef constant_pass::pass_layout cs_pass_layout;
	typedef constant_pass::PASS_RESOURCE_TYPE pass_res_type;
	auto pass_allocater = memory_allocater_group["pass_allocater"];

	deferred_render_system = new render_system();
	deferred_render_system->init(in_instance, in_render_api);

	gpu_resource* render_target_1 = deferred_render_system->create_gpu_texture("render_target_1", GPU_RES_TYPE::GPU_RES_RENDER_TARGET);
	gpu_resource* render_target_2 = deferred_render_system->create_gpu_texture("render_target_2", GPU_RES_TYPE::GPU_RES_RENDER_TARGET);
	gpu_resource* render_target_3 = deferred_render_system->create_gpu_texture("render_target_3", GPU_RES_TYPE::GPU_RES_RENDER_TARGET);
	gpu_resource* render_target_4 = deferred_render_system->create_gpu_texture("render_target_4", GPU_RES_TYPE::GPU_RES_RENDER_TARGET);
	gpu_resource* render_target_5 = deferred_render_system->create_gpu_texture("render_target_5", GPU_RES_TYPE::GPU_RES_RENDER_TARGET);

	gpu_resource* depth_stencil = deferred_render_system->create_gpu_texture("depth_stencil", GPU_RES_TYPE::GPU_RES_DEPTH_STENCIL);


	cs_pass_layout base_pass_layout;
	base_pass_layout.pass_type = constant_pass::PASS_TYPE::MESH_PASS;
	//Camera
	base_pass_layout.input_gpu_resource_layout.
		push_back({ pass_res_type::PASS_RES_BUFFER,
			1,nullptr});


	//pass creation
	//BuildDescriptorHeaps
	//CreateDescriptors
	//BuildRootSignature
	//BuildShadersAndInputLayout
	//BuildPSO
	constant_pass* base_pass = (constant_pass*)pass_allocater->allocate<constant_pass, std::string>("base_pass");

	
}

void deferred_rendering::over()
{

}

void deferred_rendering::draw(s_sence* in_sence)
{

}

//