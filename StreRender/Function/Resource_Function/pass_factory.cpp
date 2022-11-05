#include "stre_render.h"
#include "Function/Render_Function/render_system.h"
#include "Core/Memory/s_memory.h"

//创建pass

s_memory_allocater_register pass_allocater("pass_allocater");

s_pass* pass_factory::create_pass()
{
	auto pass_allocater = memory_allocater_group["pass_allocater"];

	///return pass_allocater->allocate<s_pass>();
	return new s_pass();
}

void pass_factory::dx_allocate_gpu_pass(s_pass* in_out_pass)
{
	dx_function pass_functor = [in_out_pass](s_directx_render* in_render)
	{

			//????
			const CD3DX12_STATIC_SAMPLER_DESC pointWrap(
				0, // shaderRegister
				D3D12_FILTER_MIN_MAG_MIP_POINT, // filter
				D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressU
				D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressV
				D3D12_TEXTURE_ADDRESS_MODE_WRAP); // addressW*/

			const CD3DX12_STATIC_SAMPLER_DESC pointClamp(
				1, // shaderRegister
				D3D12_FILTER_MIN_MAG_MIP_POINT, // filter
				D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressU
				D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressV
				D3D12_TEXTURE_ADDRESS_MODE_CLAMP); // addressW

			const CD3DX12_STATIC_SAMPLER_DESC linearWrap(
				2, // shaderRegister
				D3D12_FILTER_MIN_MAG_MIP_LINEAR, // filter
				D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressU
				D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressV
				D3D12_TEXTURE_ADDRESS_MODE_WRAP); // addressW

			const CD3DX12_STATIC_SAMPLER_DESC linearClamp(
				3, // shaderRegister
				D3D12_FILTER_MIN_MAG_MIP_LINEAR, // filter
				D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressU
				D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressV
				D3D12_TEXTURE_ADDRESS_MODE_CLAMP); // addressW

			const CD3DX12_STATIC_SAMPLER_DESC anisotropicWrap(
				4, // shaderRegister
				D3D12_FILTER_ANISOTROPIC, // filter
				D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressU
				D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressV
				D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressW
				0.0f,                             // mipLODBias
				8);                               // maxAnisotropy

			const CD3DX12_STATIC_SAMPLER_DESC anisotropicClamp(
				5, // shaderRegister
				D3D12_FILTER_ANISOTROPIC, // filter
				D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressU
				D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressV
				D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressW
				0.0f,                              // mipLODBias
				8);                                // maxAnisotropy

			const CD3DX12_STATIC_SAMPLER_DESC shadow(
				6, // shaderRegister
				D3D12_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT, // filter
				D3D12_TEXTURE_ADDRESS_MODE_BORDER,  // addressU
				D3D12_TEXTURE_ADDRESS_MODE_BORDER,  // addressV
				D3D12_TEXTURE_ADDRESS_MODE_BORDER,  // addressW
				0.0f,                               // mipLODBias
				16,                                 // maxAnisotropy
				D3D12_COMPARISON_FUNC_LESS_EQUAL,
				D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK);

			std::array<const CD3DX12_STATIC_SAMPLER_DESC, 7Ui64> smaples_group = {
				pointWrap, pointClamp,
				linearWrap, linearClamp,
				anisotropicWrap, anisotropicClamp,
				shadow
			};



		//根签名
		{
			in_out_pass->gpu_pass = in_render->allocate_pass();
			//...把已有的release
			{
				UINT input_cb_number = 0;
				UINT input_texture_number = 0;
				std::vector<CD3DX12_ROOT_PARAMETER> slotRootParameter;

				//只读取一个获取类型
				for (auto it : in_out_pass->gpu_mesh)
				{
					for (auto itt : it.second.gpu_mesh_resource_ptr)
					{
						in_render->load_rootparpameter(slotRootParameter, itt.second);
						break;
					}
					break;
				}

				for (auto it : in_out_pass->gpu_pass_resource_ptr)
				{
					in_render->load_rootparpameter(slotRootParameter, it.second);
				}

				auto staticSamplers = smaples_group;

				// A root signature is an array of root parameters.
				CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc((UINT)slotRootParameter.size(), slotRootParameter.data(),
					(UINT)staticSamplers.size(), staticSamplers.data(),
					D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

				in_render->create_rootsignature(
					rootSigDesc,
					in_out_pass->gpu_pass);
			}
		}

		//着色器输入布局 +  PSO构建
		{
			typedef gpu_shader_resource::SHADER_RESOURCE_TYPE GPU_SR_TYPE;

			UINT rt_number = 0;
			for (auto it : in_out_pass->gpu_rt_texture_ptr)
			{
				if (it.second->shader_resource_type == GPU_SR_TYPE::SHADER_RESOURCE_TYPE_RENDER_TARGET)
				{
					rt_number = 1;
					break;
				}
				if (it.second->shader_resource_type == GPU_SR_TYPE::SHADER_RESOURCE_TYPE_RENDER_TARGET_GROUP)
				{
					rt_number = it.second->element_count;
				}
			}

			in_render->create_pso(
				in_out_pass->gpu_shader_layout,
				in_out_pass->gpu_pass,
				rt_number,
				false);
		}

	};

	dx_pass_command.command_queue.push(pass_functor);
}


void pass_factory::add_mesh(
	s_pass* in_out_pass, 
	const cpu_mesh*  in_mesh)
{
	s_pass::gpu_mesh_resource g_mesh;
	g_mesh.vertex = in_mesh->vertex_ptr->gpu_sr_ptr;
	g_mesh.index = in_mesh->index_ptr->gpu_sr_ptr;

	g_mesh.gpu_mesh_resource_ptr[in_mesh->object_constant_ptr->uid.name] = (in_mesh->object_constant_ptr->gpu_sr_ptr);
	g_mesh.gpu_mesh_resource_ptr[in_mesh->material_ptr->uid.name] = (in_mesh->material_ptr->gpu_sr_ptr);
	for (auto it : in_mesh->texture_ptr)
	{
		g_mesh.gpu_mesh_resource_ptr[it.second->uid.name] = (it.second->gpu_sr_ptr);
	}
	//装填子mesh
	UINT index_offset = 0;
	for (int i = 0; i < in_mesh->mesh_element_index_count.size(); i++)
	{
		s_pass::gpu_mesh_resource::mesh_element mesh_e;
		mesh_e.index_number = in_mesh->mesh_element_index_count[i];
		mesh_e.index_start_offset = index_offset;
		index_offset += mesh_e.index_number;

		g_mesh.mesh_element_group.push_back(mesh_e);
	}

	in_out_pass->gpu_mesh[in_mesh->uid.name] = g_mesh;


	//可以喊别的线程做
	//auto labdma = [in_out_pass, in_mesh](
	//	s_pass* in_out_pass,
	//	const cpu_mesh* in_mesh)
	//{
		//s_pass::mesh_resource g_mesh;
		//g_mesh.gpu_vertex_layout.vertex = in_mesh->vertex_ptr->gpu_sr_ptr;
		//g_mesh.gpu_vertex_layout.index = in_mesh->index_ptr->gpu_sr_ptr;
		//g_mesh.gpu_mesh_resource_ptr[in_mesh->object_constant_ptr->uid.name] = (in_mesh->object_constant_ptr->gpu_sr_ptr);
		//g_mesh.gpu_mesh_resource_ptr[in_mesh->material_ptr->uid.name] = (in_mesh->material_ptr->gpu_sr_ptr);
		//for (int i = 0; i < in_mesh->texture_ptr.size(); i++)
		//{
		//	g_mesh.gpu_mesh_resource_ptr[in_mesh->texture_ptr[i]->uid.name] = (in_mesh->texture_ptr[i]->gpu_sr_ptr);
		//}
		//in_out_pass->gpu_mesh[in_mesh->uid.name] = g_mesh;
		//...
	//};
	//render_command<t_render>::command_queue.push(labdma);
}

template<class t_cpu_resource>
void pass_factory::add_shader_resource(
	s_pass* in_out_pass,
	const t_cpu_resource* in_sr)
{
	in_out_pass->gpu_pass_resource_ptr[in_sr->uid.name] = in_sr->gpu_sr_ptr;

	//可以喊别的线程做
	//auto labdma = [
	//	in_out_pass, 
	//		in_sr->uid,
	//		in_sr->gpu_sr_ptr](
	//	s_pass* in_out_pass,
	//	const s_uid in_uid,
	//	const gpu_shader_resource* in_gpu_sr)
	//{
	// 		//...
	//};
	//render_command<t_render>::command_queue.push(labdma);
}

void pass_factory::add_render_target(
	s_pass* in_out_pass,
	const cpu_texture* in_gpu_rt)
{
	in_out_pass->gpu_rt_texture_ptr[in_gpu_rt->uid.name] = in_gpu_rt->gpu_sr_ptr;
}

void pass_factory::remove_mesh(
	s_pass* in_out_pass,
	const cpu_mesh* in_mesh)
{
	in_out_pass->gpu_mesh.erase(in_mesh->uid.name);
}
template<class t_cpu_resource>
void pass_factory::remove_shader_resource(
	s_pass* in_out_pass,
	const t_cpu_resource* in_sr)
{
	in_out_pass->gpu_pass_resource_ptr.erase(in_sr->uid.name);
}

void pass_factory::remove_render_target(
	s_pass* in_out_pass,
	const cpu_texture* in_gpu_rt)
{
	in_out_pass->gpu_rt_texture_ptr.erase(in_gpu_rt->uid.name);
}

void pass_factory::set_shader_layout(
	s_pass* in_out_pass,
	const shader_layout in_shade_layout)
{
	in_out_pass->gpu_shader_layout = in_shade_layout;
}