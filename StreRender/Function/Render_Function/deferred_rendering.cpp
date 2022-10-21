#include "deferred_rendering.h"

//利用render_system给物体分配GPU内存
//利用render_system构建多个pass
//利用render_system构建rt
//利用render_system设定输出目标

//延迟渲染
template<typename T_render>
 void deferred_rendering<T_render>::pass_init()
{
	typedef GPU_RESOURCE_LAYOUT::GPU_RESOURCE_TYPE GPU_RES_TYPE;
	typedef constant_pass::pass_layout cs_pass_layout;
	typedef constant_pass::PASS_INPUT_RESOURCE_TYPE pass_inpu_res_type;
	typedef constant_pass::shader_layout::SHADER_TYPE SHADER_TYPE;
	typedef constant_pass::shader_layout::shader_input::INPUT_ELEMENT_SIZE INPUT_ELEMENT_SIZE;

	gpu_resource* shadow_map = deferred_render_system->create_gpu_texture("shadow_map", GPU_RES_TYPE::GPU_RES_RENDER_TARGET);

	gpu_resource* render_target_1 = deferred_render_system->create_gpu_texture("render_target_1", GPU_RES_TYPE::GPU_RES_RENDER_TARGET);
	gpu_resource* render_target_2 = deferred_render_system->create_gpu_texture("render_target_2", GPU_RES_TYPE::GPU_RES_RENDER_TARGET);
	gpu_resource* render_target_3 = deferred_render_system->create_gpu_texture("render_target_3", GPU_RES_TYPE::GPU_RES_RENDER_TARGET);
	gpu_resource* render_target_4 = deferred_render_system->create_gpu_texture("render_target_4", GPU_RES_TYPE::GPU_RES_RENDER_TARGET);
	gpu_resource* render_target_5 = deferred_render_system->create_gpu_texture("render_target_5", GPU_RES_TYPE::GPU_RES_RENDER_TARGET);



	gpu_resource* depth_stencil = deferred_render_system->create_gpu_texture("depth_stencil", GPU_RES_TYPE::GPU_RES_DEPTH_STENCIL);
	//BASE PASS
	{
		cs_pass_layout base_pass_layout;
		base_pass_layout.pass_type = constant_pass::PASS_TYPE::MESH_PASS;
		
		base_pass_layout.use_resource_flag[pass_inpu_res_type::USE_CUSTOM_TEXTURE];

		base_pass_layout.input_gpu_resource.push_back(shadow_map);

		base_pass_layout.pass_shader_layout.shader_vaild[SHADER_TYPE::VS] = true;
		base_pass_layout.pass_shader_layout.shader_vaild[SHADER_TYPE::PS] = true;
		base_pass_layout.pass_shader_layout.shader_path[SHADER_TYPE::VS] = L"Shaders\\BasePass.hlsl";
		base_pass_layout.pass_shader_layout.shader_path[SHADER_TYPE::PS] = L"Shaders\\BasePass.hlsl";

		base_pass_layout.pass_shader_layout.shader_input_group.push_back(
			{ "POSITION",INPUT_ELEMENT_SIZE::INPUT_ELEMENT_SIZE_R32G32B32 });
		base_pass_layout.pass_shader_layout.shader_input_group.push_back(
			{ "NORMAL",INPUT_ELEMENT_SIZE::INPUT_ELEMENT_SIZE_R32G32B32 });
		base_pass_layout.pass_shader_layout.shader_input_group.push_back(
			{ "TEXCOORD",INPUT_ELEMENT_SIZE::INPUT_ELEMENT_SIZE_R32G32 });
		base_pass_layout.pass_shader_layout.shader_input_group.push_back(
			{ "TANGENT",INPUT_ELEMENT_SIZE::INPUT_ELEMENT_SIZE_R32G32B32 });


		base_pass_layout.output_gpu_resource.push_back(render_target_1);
		base_pass_layout.output_gpu_resource.push_back(render_target_2);
		base_pass_layout.output_gpu_resource.push_back(render_target_3);
		base_pass_layout.output_gpu_resource.push_back(render_target_4);
		base_pass_layout.output_gpu_resource.push_back(render_target_5);

		base_pass_layout.output_gpu_depth_stencil_resource = depth_stencil;

		constant_pass* base_pass = deferred_render_system->allocate_pass(base_pass_layout);
	}

}

template<typename T_render>
void deferred_rendering<T_render>::over()
{

}

template<typename T_render>
void deferred_rendering<T_render>::draw(s_sence* in_sence)
{
	//


}

//