#include "deferred_rendering.h"

//����render_system���������GPU�ڴ�
//����render_system�������pass
//����render_system����rt
//����render_system�趨���Ŀ��

//�ӳ���Ⱦ
void deferred_rendering::init(HINSTANCE in_instance, REDNER_API in_render_api)
{
	typedef GPU_RESOURCE_LAYOUT::GPU_RESOURCE_TYPE GPU_RES_TYPE;
	typedef constant_pass::pass_layout cs_pass_layout;
	typedef constant_pass::PASS_RESOURCE_TYPE pass_res_type;
	
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

	//ObjCB
	base_pass_layout.input_gpu_resource_layout.
		push_back({ pass_res_type::PASS_RES_CBV,
			1,nullptr });
	//Camera
	base_pass_layout.input_gpu_resource_layout.
		push_back({ pass_res_type::PASS_RES_CBV,
			1,nullptr });
	//MatCB
	base_pass_layout.input_gpu_resource_layout.
		push_back({ pass_res_type::PASS_RES_SRV,
			1,nullptr });
	//ShadowMap
	base_pass_layout.input_gpu_resource_layout.
		push_back({ pass_res_type::PASS_RES_SRV,
			1,nullptr });
	//Texture
	base_pass_layout.input_gpu_resource_layout.
		push_back({ pass_res_type::PASS_RES_DESCRIPTOR_TBALE,
			1,nullptr });

	base_pass_layout.output_gpu_resource.push_back(render_target_1);
	base_pass_layout.output_gpu_resource.push_back(render_target_2);
	base_pass_layout.output_gpu_resource.push_back(render_target_3);
	base_pass_layout.output_gpu_resource.push_back(render_target_4);
	base_pass_layout.output_gpu_resource.push_back(render_target_5);

	base_pass_layout.output_gpu_depth_stencil_resource = depth_stencil;


	constant_pass* base_pass = deferred_render_system->allocate_pass(base_pass_layout);
}

void deferred_rendering::over()
{

}

void deferred_rendering::draw(s_sence* in_sence)
{

}

//