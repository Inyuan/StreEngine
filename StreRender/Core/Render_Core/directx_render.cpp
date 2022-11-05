#include "directx_render.h"
#include "stre_render.h"
#include <WindowsX.h>
#include "Core/Exception/exception.h"
#include <array>
#include <string>
#include "Core/Memory/s_memory.h"

/***
************************************************************
*
* Pass Function
*
************************************************************
*/


s_memory_allocater_register gpu_pass_allocater("gpu_pass_allocater");

gpu_pass* directx_render::allocate_pass()
{
	auto pass_allocater = memory_allocater_group["gpu_pass_allocater"];

	//return pass_allocater->allocate<directx_render::directx_pass>();
	return new directx_render::directx_pass();
}

void directx_render::load_rootparpameter(
	std::vector<CD3DX12_ROOT_PARAMETER>& in_out_root_parameter, 
	const gpu_shader_resource* in_gpu_sr)
{
	switch (in_gpu_sr->shader_resource_type)
	{
	case gpu_shader_resource::SHADER_RESOURCE_TYPE_CUSTOM_BUFFER:
	{
		CD3DX12_ROOT_PARAMETER r_p;
		r_p.InitAsConstantBufferView(in_gpu_sr->register_index);
		in_out_root_parameter.push_back(r_p);
	}
		break;
	case gpu_shader_resource::SHADER_RESOURCE_TYPE_CUSTOM_BUFFER_GROUP:
	case gpu_shader_resource::SHADER_RESOURCE_TYPE_TEXTURE:
	{
		CD3DX12_ROOT_PARAMETER r_p;
		r_p.InitAsShaderResourceView(in_gpu_sr->register_index);
		in_out_root_parameter.push_back(r_p);
	}
		break;
	case gpu_shader_resource::SHADER_RESOURCE_TYPE_TEXTURE_GROUP:
	{
		CD3DX12_DESCRIPTOR_RANGE texTable;
		texTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, in_gpu_sr->element_count, in_gpu_sr->register_index);
		CD3DX12_ROOT_PARAMETER texture_rp;
		texture_rp.InitAsDescriptorTable(1, &texTable, D3D12_SHADER_VISIBILITY_PIXEL);
		in_out_root_parameter.push_back(texture_rp);
	}
		break;
	default:
		break;
	}
}

void directx_render::create_rootsignature(
	CD3DX12_ROOT_SIGNATURE_DESC& in_rootsig_desc,
	gpu_pass* in_gpu_pass)
{
	create_rootsignature(
		in_rootsig_desc,
		static_cast<directx_pass*>(in_gpu_pass)->rootsignature);
}

void directx_render::create_pso(
	shader_layout in_shader_layout,
	gpu_pass* in_gpu_pass,
	UINT in_rt_number,
	bool is_translate)
{
	typedef shader_layout::SHADER_TYPE SHADER_TYPE;
	typedef shader_layout::shader_input::INPUT_ELEMENT_SIZE INPUT_ELEMENT_SIZE;
	auto& shader_layout = in_shader_layout;
	auto pass = static_cast<directx_pass*>(in_gpu_pass);

	if (in_shader_layout.shader_vaild[SHADER_TYPE::VS])
	{
		pass->shader_group[std::string(pass->uid.name) + "VS"]
			= complie_shader(
				shader_layout.shader_path[SHADER_TYPE::VS],
				nullptr, "VS", "vs_5_1");
	}
	if (in_shader_layout.shader_vaild[SHADER_TYPE::DS])
	{
		pass->shader_group[std::string(pass->uid.name) + "DS"]
			= complie_shader(
				shader_layout.shader_path[SHADER_TYPE::DS],
				nullptr, "DS", "ds_5_1");
	}
	if (in_shader_layout.shader_vaild[SHADER_TYPE::HS])
	{
		pass->shader_group[std::string(pass->uid.name) + "HS"]
			= complie_shader(
				shader_layout.shader_path[SHADER_TYPE::HS],
				nullptr, "HS", "hs_5_1");
	}
	if (in_shader_layout.shader_vaild[SHADER_TYPE::GS])
	{
		pass->shader_group[std::string(pass->uid.name) + "GS"]
			= complie_shader(
				shader_layout.shader_path[SHADER_TYPE::GS],
				nullptr, "GS", "gs_5_1");
	}
	if (in_shader_layout.shader_vaild[SHADER_TYPE::PS])
	{
		pass->shader_group[std::string(pass->uid.name) + "PS"]
			= complie_shader(
				shader_layout.shader_path[SHADER_TYPE::PS],
				nullptr, "PS", "ps_5_1");
	}

	UINT elem_size_offset = 0;
	for (auto it : shader_layout.shader_input_group)
	{
		D3D12_INPUT_ELEMENT_DESC input_elem_desc;

		DXGI_FORMAT input_elem_format = DXGI_FORMAT_UNKNOWN;
		UINT offset = 0;
		switch (it.size)
		{
		case INPUT_ELEMENT_SIZE::INPUT_ELEMENT_SIZE_R32:
			input_elem_format = DXGI_FORMAT_R32_FLOAT;
			offset = 4;
			break;
		case INPUT_ELEMENT_SIZE::INPUT_ELEMENT_SIZE_R32G32:
			input_elem_format = DXGI_FORMAT_R32G32_FLOAT;
			offset = 8;
			break;
		case INPUT_ELEMENT_SIZE::INPUT_ELEMENT_SIZE_R32G32B32:
			input_elem_format = DXGI_FORMAT_R32G32B32_FLOAT;
			offset = 12;
			break;
		case INPUT_ELEMENT_SIZE::INPUT_ELEMENT_SIZE_R32G32B32A32:
			input_elem_format = DXGI_FORMAT_R32G32B32A32_FLOAT;
			offset = 16;
			break;
		}
		input_elem_desc = { it.name.c_str(), 0, input_elem_format, 0, elem_size_offset, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };

		pass->input_layout.push_back(input_elem_desc);

		elem_size_offset += offset;
	}

	{

		auto pass = static_cast<directx_pass*>(in_gpu_pass);

		auto rt_number = in_rt_number;

		CD3DX12_RASTERIZER_DESC RastDesc(D3D12_DEFAULT);
		RastDesc.CullMode = D3D12_CULL_MODE_NONE;

		D3D12_GRAPHICS_PIPELINE_STATE_DESC PsoDesc;
		ZeroMemory(&PsoDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
		PsoDesc.InputLayout = { pass->input_layout.data(), (UINT)pass->input_layout.size() };
		PsoDesc.pRootSignature = pass->rootsignature.Get();
		PsoDesc.RasterizerState = RastDesc;
		PsoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
		PsoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
		PsoDesc.SampleMask = UINT_MAX;
		PsoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		PsoDesc.SampleDesc.Count = MSAAX4_STATE ? 4 : 1;
		PsoDesc.SampleDesc.Quality = MSAAX4_STATE ? (MSAAX4_QUALITY - 1) : 0;
		PsoDesc.DSVFormat = depth_stencil_format;
		PsoDesc.NumRenderTargets = rt_number;
		for (UINT i = 0; i < rt_number; i++)
			PsoDesc.RTVFormats[i] = back_buffer_format;


		if (in_shader_layout.shader_vaild[SHADER_TYPE::VS])
		{
			PsoDesc.VS =
			{
				reinterpret_cast<BYTE*>(
					pass->shader_group[std::string(pass->uid.name) + "VS"]->GetBufferPointer()),
				pass->shader_group[std::string(pass->uid.name) + "VS"]->GetBufferSize()
			};
		}
		if (in_shader_layout.shader_vaild[SHADER_TYPE::DS])
		{
			PsoDesc.DS =
			{
				reinterpret_cast<BYTE*>(
					pass->shader_group[std::string(pass->uid.name) + "DS"]->GetBufferPointer()),
				pass->shader_group[std::string(pass->uid.name) + "DS"]->GetBufferSize()
			};
		}
		if (in_shader_layout.shader_vaild[SHADER_TYPE::HS])
		{
			PsoDesc.HS =
			{
				reinterpret_cast<BYTE*>(
					pass->shader_group[std::string(pass->uid.name) + "HS"]->GetBufferPointer()),
				pass->shader_group[std::string(pass->uid.name) + "HS"]->GetBufferSize()
			};
		}
		if (in_shader_layout.shader_vaild[SHADER_TYPE::GS])
		{
			PsoDesc.GS =
			{
				reinterpret_cast<BYTE*>(
					pass->shader_group[std::string(pass->uid.name) + "GS"]->GetBufferPointer()),
				pass->shader_group[std::string(pass->uid.name) + "GS"]->GetBufferSize()
			};
		}
		if (in_shader_layout.shader_vaild[SHADER_TYPE::PS])
		{
			PsoDesc.PS =
			{
				reinterpret_cast<BYTE*>(
					pass->shader_group[std::string(pass->uid.name) + "PS"]->GetBufferPointer()),
				pass->shader_group[std::string(pass->uid.name) + "PS"]->GetBufferSize()
			};
		}

		create_pso(
			PsoDesc,
			pass->pso);
	}
}

/***
************************************************************
*
* Shader Resource Function
*
************************************************************
*/

//s_memory_allocater_register gpu_shader_res_allocater("gpu_shader_res_allocater");

gpu_shader_resource* directx_render::allocate_shader_resource(gpu_shader_resource::SHADER_RESOURCE_TYPE in_shader_res_type)
{
	auto gpu_shader_res_allocater = memory_allocater_group["gpu_shader_res_allocater"];

	switch (in_shader_res_type)
	{
		//结构体 CBV
	case gpu_shader_resource::SHADER_RESOURCE_TYPE_CUSTOM_BUFFER:
	{
		//auto gpu_ptr = dynamic_cast<directx_sr_custom_buffer*>(
		//	gpu_shader_res_allocater->allocate<directx_sr_custom_buffer>());
		//

		auto gpu_ptr = new directx_sr_custom_buffer();
		
		gpu_ptr->shader_resource_type = in_shader_res_type;

		//建了也只有被集中进table时才有用
		create_descriptor(
			DIRECTX_RESOURCE_DESC_TYPE::DX_CBV,
			gpu_ptr);

		return gpu_ptr;
	}
	break;

	//结构体组 SRV
	case gpu_shader_resource::SHADER_RESOURCE_TYPE_CUSTOM_BUFFER_GROUP_FOLLOW_MESH:
	case gpu_shader_resource::SHADER_RESOURCE_TYPE_CUSTOM_BUFFER_GROUP:
	{
		//auto gpu_ptr = dynamic_cast<directx_sr_custom_buffer_group*>(
		//	gpu_shader_res_allocater->allocate<directx_sr_custom_buffer_group>());
		
		auto gpu_ptr = new directx_sr_custom_buffer_group();
		
		gpu_ptr->shader_resource_type = in_shader_res_type;

		

		//建了也只有被集中进table时才有用
		create_descriptor( // CBV???
			DIRECTX_RESOURCE_DESC_TYPE::DX_CBV,
			gpu_ptr);

		return gpu_ptr;

	}
	break;

	//贴图 SRV
	case gpu_shader_resource::SHADER_RESOURCE_TYPE_TEXTURE:
	{
		//auto gpu_ptr = dynamic_cast<directx_sr_texture*>(
		//	gpu_shader_res_allocater->allocate<directx_sr_texture>());
		//
		auto gpu_ptr = new directx_sr_texture();


		gpu_ptr->shader_resource_type = in_shader_res_type;

		//不应该让外面控制精度吗？？？
		gpu_ptr->dx_format = back_buffer_format;

		//建了也只有被集中进table时才有用
		create_descriptor(
			DIRECTX_RESOURCE_DESC_TYPE::DX_SRV,
			gpu_ptr);

		return gpu_ptr;
	}
	break;
	//堆 TABLE
	case gpu_shader_resource::SHADER_RESOURCE_TYPE_TEXTURE_GROUP:
	{
		//auto gpu_ptr = dynamic_cast<directx_sr_texture_group*>(
		//	gpu_shader_res_allocater->allocate<directx_sr_texture_group>());
		//
		auto gpu_ptr = new directx_sr_texture_group();

		gpu_ptr->shader_resource_type = in_shader_res_type;

		return gpu_ptr;
	}
	break;
	//RT
	case gpu_shader_resource::SHADER_RESOURCE_TYPE_RENDER_TARGET:
	{
		//auto gpu_ptr = dynamic_cast<directx_sr_render_target*>(
		//	gpu_shader_res_allocater->allocate<directx_sr_render_target>());
		//
		auto gpu_ptr = new directx_sr_render_target();


		gpu_ptr->shader_resource_type = in_shader_res_type;
		gpu_ptr->dx_format = back_buffer_format;
		create_gpu_texture(gpu_ptr);

		create_descriptor(
			DIRECTX_RESOURCE_DESC_TYPE::DX_SRV,
			gpu_ptr);

		create_descriptor(
			DIRECTX_RESOURCE_DESC_TYPE::DX_RTV,
			gpu_ptr);

		return gpu_ptr;
	}
	break;
	case gpu_shader_resource::SHADER_RESOURCE_TYPE_RENDER_TARGET_GROUP:
	{
		//auto gpu_ptr = dynamic_cast<directx_sr_render_target_group*>(
		//	gpu_shader_res_allocater->allocate<directx_sr_render_target_group>());
		//
		auto gpu_ptr = new directx_sr_render_target_group();

		gpu_ptr->shader_resource_type = in_shader_res_type;
		return gpu_ptr;
	}
	break;
	//Depth
	case gpu_shader_resource::SHADER_RESOURCE_TYPE_RENDER_DEPTH_STENCIL:
	{
		//auto gpu_ptr = dynamic_cast<directx_sr_depth_stencil*>(
		//	gpu_shader_res_allocater->allocate<directx_sr_depth_stencil>());
		//
		auto gpu_ptr = new directx_sr_depth_stencil();

		gpu_ptr->shader_resource_type = in_shader_res_type;
		gpu_ptr->dx_format = depth_stencil_format;
		create_gpu_texture(gpu_ptr);

		create_descriptor(
			DIRECTX_RESOURCE_DESC_TYPE::DX_SRV,
			gpu_ptr);

		create_descriptor(
			DIRECTX_RESOURCE_DESC_TYPE::DX_DSV,
			gpu_ptr);

		return gpu_ptr;
	}
	case gpu_shader_resource::SHADER_RESOURCE_TYPE_RENDER_DEPTH_STENCIL_GROUP:
	{
		//auto gpu_ptr = dynamic_cast<directx_sr_depth_stencil_group*>(
		//	gpu_shader_res_allocater->allocate<directx_sr_depth_stencil_group>());
		//
		auto gpu_ptr = new directx_sr_depth_stencil_group();

		gpu_ptr->shader_resource_type = in_shader_res_type;

		return gpu_ptr;
	}
	break;
	 
	default:
		{
		//auto gpu_ptr = dynamic_cast<directx_sr_custom_buffer*>(
		//	gpu_shader_res_allocater->allocate<directx_sr_custom_buffer>());
		//
		auto gpu_ptr = new directx_sr_custom_buffer();

		gpu_ptr->shader_resource_type = in_shader_res_type;
			return gpu_ptr;
		}
		break;
	}
}

void directx_render::allocate_default_resource(
	gpu_shader_resource* in_res,
	UINT in_elem_size,
	UINT in_number,
	void* in_cpu_data)
{
	auto in_res_elem = static_cast<directx_shader_resource*>(in_res);
	UINT memory_size = in_elem_size * in_number;
	in_res_elem->element_size = in_elem_size;
	in_res_elem->element_count = in_number;
	// Create the actual default buffer resource.
	CD3DX12_HEAP_PROPERTIES Heapproperties(D3D12_HEAP_TYPE_DEFAULT);
	CD3DX12_RESOURCE_DESC Resourcedesc = CD3DX12_RESOURCE_DESC::Buffer(memory_size);

	create_gpu_memory(
		in_res_elem->dx_resource,
		Heapproperties,
		Resourcedesc
	);

	//??? 不知道这样用上传堆对不对
	ComPtr<ID3D12Resource> upload_buffer;
	// In order to copy CPU memory data into our default buffer, we need to create
	// an intermediate upload heap. 
	CD3DX12_HEAP_PROPERTIES Heappropertiesup(D3D12_HEAP_TYPE_UPLOAD);
	Resourcedesc = CD3DX12_RESOURCE_DESC::Buffer(memory_size);

	create_gpu_memory(
		upload_buffer,
		Heappropertiesup,
		Resourcedesc
	);

	// Describe the data we want to copy into the default buffer.
	D3D12_SUBRESOURCE_DATA subResourceData = {};
	subResourceData.pData = in_cpu_data;
	subResourceData.RowPitch = memory_size;
	subResourceData.SlicePitch = subResourceData.RowPitch;

	// Schedule to copy the data to the default buffer resource.  At a high level, the helper function UpdateSubresources
	// will copy the CPU memory into the intermediate upload heap.  Then, using ID3D12CommandList::CopySubresourceRegion,
	// the intermediate upload heap data will be copied to mBuffer.
	switch_gpu_memory_state(
		in_res_elem->dx_resource,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		D3D12_RESOURCE_STATE_COPY_DEST);

	UpdateSubresources<1>(
		command_list.Get(),
		in_res_elem->dx_resource.Get(),
		upload_buffer.Get(),
		0, 0, 1, &subResourceData);

	switch_gpu_memory_state(
		in_res_elem->dx_resource,
		D3D12_RESOURCE_STATE_COPY_DEST,
		D3D12_RESOURCE_STATE_GENERIC_READ);


	// Note: uploadBuffer has to be kept alive after the above function calls because
	// the command list has not been executed yet that performs the actual copy.
	// The caller can Release the uploadBuffer after it knows the copy has been executed.
}

void directx_render::allocate_upload_resource(
	gpu_shader_resource* in_res,
	UINT in_elem_size,
	UINT in_number)
{
	auto in_res_elem = static_cast<directx_shader_resource*>(in_res);
	UINT memory_size = in_elem_size * in_number;
	in_res_elem->element_size = in_elem_size;
	in_res_elem->element_count = in_number;
	CD3DX12_HEAP_PROPERTIES Heapproperties(D3D12_HEAP_TYPE_UPLOAD);
	CD3DX12_RESOURCE_DESC Resourcedesc = CD3DX12_RESOURCE_DESC::Buffer(memory_size);

	create_gpu_memory(
		in_res_elem->dx_resource,
		Heapproperties,
		Resourcedesc
	);

	ThrowIfFailed(in_res_elem->dx_resource->Map(0, nullptr, reinterpret_cast<void**>(&(in_res_elem->mapped_data))));

}

//注意 渲染目标 和 贴图 深度图等的匹配
void directx_render::pakeage_textures(
	std::vector<const gpu_shader_resource*>& in_texture_group, 
	gpu_shader_resource* in_out_table)
{
	switch (in_out_table->shader_resource_type)
	{
	case gpu_shader_resource::SHADER_RESOURCE_TYPE::SHADER_RESOURCE_TYPE_TEXTURE_GROUP:
	{
		auto gpu_table = static_cast<directx_sr_texture_group*>(in_out_table);
		if (gpu_table->srv_heap)
		{
			gpu_table->srv_heap->Release();
			//release()
		}

		create_descriptor_heap(
			gpu_table->srv_heap,
			DIRECTX_RESOURCE_DESC_TYPE::DX_SRV,
			(UINT)in_texture_group.size());

		CD3DX12_CPU_DESCRIPTOR_HANDLE handle(gpu_table->srv_heap->GetCPUDescriptorHandleForHeapStart());


		for (int i = 0; i < in_texture_group.size(); i++)
		{
			auto gpu_texture = static_cast<const directx_sr_texture*>(in_texture_group[i]);

			load_descriptor_into_heap(
				DIRECTX_RESOURCE_DESC_TYPE::DX_SRV, // ???
				gpu_texture,
				handle
			);
			handle.Offset(1, cbv_srv_uav_descriptor_size);
		}
	}
		break;
	case gpu_shader_resource::SHADER_RESOURCE_TYPE::SHADER_RESOURCE_TYPE_RENDER_TARGET_GROUP:
	{
		auto gpu_table = static_cast<directx_sr_render_target_group*>(in_out_table);
		if (gpu_table->rtv_heap)
		{
			gpu_table->rtv_heap->Release();
			//release()
		}

		create_descriptor_heap(
			gpu_table->rtv_heap,
			DIRECTX_RESOURCE_DESC_TYPE::DX_RTV,
			(UINT)in_texture_group.size());

		CD3DX12_CPU_DESCRIPTOR_HANDLE handle(gpu_table->rtv_heap->GetCPUDescriptorHandleForHeapStart());


		for (int i = 0; i < in_texture_group.size(); i++)
		{
			auto gpu_texture = static_cast<const directx_sr_texture*>(in_texture_group[i]);

			load_descriptor_into_heap(
				DIRECTX_RESOURCE_DESC_TYPE::DX_RTV,
				gpu_texture,
				handle
			);
			handle.Offset(1, rtv_descriptor_size);
		}
	}
		break;
	case gpu_shader_resource::SHADER_RESOURCE_TYPE::SHADER_RESOURCE_TYPE_RENDER_DEPTH_STENCIL_GROUP:
	{
		auto gpu_table = static_cast<directx_sr_depth_stencil_group*>(in_out_table);
		if (gpu_table->dsv_heap)
		{
			gpu_table->dsv_heap->Release();
			//release()
		}

		create_descriptor_heap(
			gpu_table->dsv_heap,
			DIRECTX_RESOURCE_DESC_TYPE::DX_DSV,
			(UINT)in_texture_group.size());

		CD3DX12_CPU_DESCRIPTOR_HANDLE handle(gpu_table->dsv_heap->GetCPUDescriptorHandleForHeapStart());


		for (int i = 0; i < in_texture_group.size(); i++)
		{
			auto gpu_texture = static_cast<const directx_sr_texture*>(in_texture_group[i]);

			load_descriptor_into_heap(
				DIRECTX_RESOURCE_DESC_TYPE::DX_DSV,
				gpu_texture,
				handle
			);
			handle.Offset(1, dsv_descriptor_size);
		}
	}
		break;
	}

}


void directx_render::create_gpu_texture(gpu_shader_resource* in_out_gpu_texture)
{
	CD3DX12_RESOURCE_DESC resourceDesc;
	ZeroMemory(&resourceDesc, sizeof(resourceDesc));
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	resourceDesc.Alignment = 0;
	resourceDesc.SampleDesc.Count = MSAAX4_STATE ? 4 : 1;
	resourceDesc.SampleDesc.Quality = MSAAX4_STATE ? (MSAAX4_QUALITY - 1) : 0;
	resourceDesc.MipLevels = 1;
	resourceDesc.DepthOrArraySize = 1;
	resourceDesc.Width = CLIENT_WIDTH;
	resourceDesc.Height = CLIENT_HEIGHT;
	resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	

	if (in_out_gpu_texture->shader_resource_type == gpu_shader_resource::SHADER_RESOURCE_TYPE_RENDER_TARGET)
	{
		auto gpu_texture = static_cast<directx_sr_render_target*>(in_out_gpu_texture);
		
		resourceDesc.Format = gpu_texture->dx_format;
		resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;

		D3D12_CLEAR_VALUE clearVal;
		clearVal.Color[0] = clear_color[0];
		clearVal.Color[1] = clear_color[1];
		clearVal.Color[2] = clear_color[2];
		clearVal.Color[3] = clear_color[3];
		clearVal.Format = gpu_texture->dx_format;

		CD3DX12_HEAP_PROPERTIES heapproperties(D3D12_HEAP_TYPE_DEFAULT);

		create_gpu_memory(
			gpu_texture->dx_resource,
			heapproperties,
			resourceDesc,
			default_resource_states,
			&clearVal);
	}
	else if (in_out_gpu_texture->shader_resource_type == gpu_shader_resource::SHADER_RESOURCE_TYPE_RENDER_DEPTH_STENCIL)
	{
		auto gpu_texture = static_cast<directx_sr_depth_stencil*>(in_out_gpu_texture);

		resourceDesc.Format = gpu_texture->dx_format;
		resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

		D3D12_CLEAR_VALUE optClear;
		optClear.Format = gpu_texture->dx_format;
		optClear.DepthStencil.Depth = 1.0f;
		optClear.DepthStencil.Stencil = 0;

		CD3DX12_HEAP_PROPERTIES heapproperties(D3D12_HEAP_TYPE_DEFAULT);

		create_gpu_memory(
			gpu_texture->dx_resource,
			heapproperties,
			resourceDesc,
			default_resource_states,
			&optClear);
	}
}


/***
************************************************************
*
* Draw Call Function
*
************************************************************
*/


/// <summary>
/// 默认所有资源都准备好（渲染目标清理也需要手动先清理）
/// 要自己设定寄存器，包括mesh的资源
/// </summary>
/// <param name="in_pass"></param>
void directx_render::draw_pass(const s_pass* in_pass)
{
	auto* pass = static_cast<const directx_pass*>(in_pass->gpu_pass);

	command_list->SetGraphicsRootSignature(pass->rootsignature.Get());

	command_list->SetPipelineState(pass->pso.Get());

	command_list->RSSetViewports(1, &screen_view_port);
	command_list->RSSetScissorRects(1, &scissor_rect);

	Load_resource(in_pass->gpu_pass_resource_ptr,true);

	for (auto gpu_mesh : in_pass->gpu_mesh)
	{
		Load_resource(gpu_mesh.second.gpu_mesh_resource_ptr);

		draw_call(&gpu_mesh.second);
	}
}

void directx_render::Load_resource(
	const std::map < std::string, const gpu_shader_resource*>&  in_gpu_res_group,
	bool set_render_tager)
{
	D3D12_CPU_DESCRIPTOR_HANDLE rtv_descs;
	D3D12_CPU_DESCRIPTOR_HANDLE dsv_desc;
	UINT output_rt_size = 0;
	for (auto gpu_res : in_gpu_res_group)
	{
		switch (gpu_res.second->shader_resource_type)
		{
			//结构体
		case gpu_shader_resource::SHADER_RESOURCE_TYPE_CUSTOM_BUFFER:
		{
			auto* gpu_sr = static_cast<const directx_sr_custom_buffer*>(gpu_res.second);
			command_list->
				SetGraphicsRootConstantBufferView(
					gpu_res.second->register_index,
					gpu_sr->dx_resource.Get()->GetGPUVirtualAddress());
		}
		break;

		//结构体组
		case gpu_shader_resource::SHADER_RESOURCE_TYPE_CUSTOM_BUFFER_GROUP:
		{
			auto* gpu_sr = static_cast<const directx_sr_custom_buffer_group*>(gpu_res.second);
			command_list->
				SetGraphicsRootShaderResourceView(
					gpu_res.second->register_index,
					gpu_sr->dx_resource.Get()->GetGPUVirtualAddress());
		}
		break;

		//贴图
		case gpu_shader_resource::SHADER_RESOURCE_TYPE_TEXTURE:
		{
			auto* gpu_sr = static_cast<const directx_sr_texture*>(gpu_res.second);
			command_list->
				SetGraphicsRootShaderResourceView(
					gpu_res.second->register_index,
					gpu_sr->dx_resource.Get()->GetGPUVirtualAddress());
		}
		break;
		//贴图堆
		case gpu_shader_resource::SHADER_RESOURCE_TYPE_TEXTURE_GROUP:
		{
			auto* gpu_sr = static_cast<const directx_sr_texture_group*>(gpu_res.second);
			ID3D12DescriptorHeap* descriptorHeaps[] = { gpu_sr->srv_heap.Get() };
			command_list->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);
			command_list->SetGraphicsRootDescriptorTable(gpu_res.second->register_index, gpu_sr->srv_heap->GetGPUDescriptorHandleForHeapStart());
		}
		break;
		//RT
		case gpu_shader_resource::SHADER_RESOURCE_TYPE_RENDER_TARGET_GROUP:
		{
			//if (rtv_descs)
			//{
			//	//已经有渲染目标了！
			//}
			auto* gpu_sr = static_cast<const directx_sr_render_target_group*>(gpu_res.second);
			rtv_descs = gpu_sr->rtv_heap.Get()->GetCPUDescriptorHandleForHeapStart();
			output_rt_size = gpu_sr->rt_number;
		}
		break;
		//Depth
		case gpu_shader_resource::SHADER_RESOURCE_TYPE_RENDER_DEPTH_STENCIL:
		{
			auto* gpu_sr = static_cast<const directx_sr_depth_stencil_group*>(gpu_res.second);
			dsv_desc = gpu_sr->dsv_heap.Get()->GetCPUDescriptorHandleForHeapStart();
		}
		break;
		}
	}
	if (set_render_tager)
	{
		command_list->OMSetRenderTargets(output_rt_size,
			&rtv_descs,
			true,
			&dsv_desc);
	}
}

void directx_render::draw_call(
	const s_pass::gpu_mesh_resource* in_gpu_mesh)
{
	auto* vertex_buffer = static_cast<const directx_sr_custom_buffer*>(in_gpu_mesh->vertex);
	auto* index_buffer = static_cast<const directx_sr_custom_buffer*>(in_gpu_mesh->index);

	D3D12_VERTEX_BUFFER_VIEW Vertexbufferview;
	Vertexbufferview.BufferLocation = vertex_buffer->dx_resource->GetGPUVirtualAddress();
	Vertexbufferview.StrideInBytes = vertex_buffer->element_size;
	Vertexbufferview.SizeInBytes = vertex_buffer->element_count * vertex_buffer->element_size;

	D3D12_INDEX_BUFFER_VIEW Indexbufferview;
	Indexbufferview.BufferLocation = index_buffer->dx_resource->GetGPUVirtualAddress();
	Indexbufferview.SizeInBytes = index_buffer->element_count * index_buffer->element_size;
	switch (index_buffer->element_size)
	{
	case 8:
		Indexbufferview.Format = DXGI_FORMAT_R8_UINT;
	case 16:
		Indexbufferview.Format = DXGI_FORMAT_R16_UINT;
	case 32:
		Indexbufferview.Format = DXGI_FORMAT_R32_UINT;
	}
	//???只支持三角形
	command_list->IASetVertexBuffers(0, 1, &Vertexbufferview);
	command_list->IASetIndexBuffer(&Indexbufferview);
	command_list->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


	//找随子mesh索引变换的CB资源
	std::vector<const directx_sr_custom_buffer_group*> refresh_group;

	for (auto it : in_gpu_mesh->gpu_mesh_resource_ptr)
	{
		if(it.second->shader_resource_type == gpu_shader_resource::SHADER_RESOURCE_TYPE_CUSTOM_BUFFER_GROUP_FOLLOW_MESH)
		{
			refresh_group.push_back(static_cast<const directx_sr_custom_buffer_group*>(it.second));
		}
	}

	for (UINT i = 0; i < in_gpu_mesh->mesh_element_group.size(); i++)
	{
		for (auto it : refresh_group)
		{
			D3D12_GPU_VIRTUAL_ADDRESS objCBAddress = it->dx_resource->GetGPUVirtualAddress() + i * it->element_size;
			command_list->SetGraphicsRootConstantBufferView(0, objCBAddress);
		}

		command_list->DrawIndexedInstanced(
			in_gpu_mesh->mesh_element_group[i].index_number,
			1,
			in_gpu_mesh->mesh_element_group[i].index_start_offset,
			/*BaseVertexLocation*/ 0,
			0);
	}
}


void directx_render::set_swap_chain_buffer()
{

}

void directx_render::switch_fence()
{
	// Swap the back and front buffers
	ThrowIfFailed(swap_chain->Present(0, 0));
	current_back_buffer = (current_back_buffer + 1) % SWAP_CHAIN_BUFFER_COUNT;

	// Advance the fence value to mark commands up to this fence point.
	mCurrFrameResource->Fence = ++mCurrentFence;

	// Add an instruction to the command queue to set a new fence point. 
	// Because we are on the GPU timeline, the new fence point won't be 
	// set until the GPU finishes processing all the commands prior to this Signal().
	mCommandQueue->Signal(mFence.Get(), mCurrentFence);
}


/***
************************************************************
*
* Base Function
*
************************************************************
*/
//
void directx_render::create_descriptor_heap(
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> in_out_descheap,
	DIRECTX_RESOURCE_DESC_TYPE in_texture_desc_type,
	UINT in_desc_number)
{

	D3D12_DESCRIPTOR_HEAP_DESC HeapDesc = {};
	HeapDesc.NumDescriptors = in_desc_number;
	
	switch(in_texture_desc_type)
	{
	case DIRECTX_RESOURCE_DESC_TYPE::DX_CBV:
	case DIRECTX_RESOURCE_DESC_TYPE::DX_SRV:
	case DIRECTX_RESOURCE_DESC_TYPE::DX_UAV:
		HeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		HeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		break;
	case DIRECTX_RESOURCE_DESC_TYPE::DX_RTV:
		HeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		HeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		HeapDesc.NodeMask = 0;
		break;
	case DIRECTX_RESOURCE_DESC_TYPE::DX_DSV:
		HeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
		HeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		HeapDesc.NodeMask = 0;
		break;
	}


	ThrowIfFailed(d3d_device->CreateDescriptorHeap(
		&HeapDesc,
		IID_PPV_ARGS(in_out_descheap.GetAddressOf())));
}

//vertex 
//index 
//texture
//默认构建 read的缓冲区
void directx_render::create_gpu_memory(
	ComPtr<ID3D12Resource> in_out_resource,
	CD3DX12_HEAP_PROPERTIES in_heap_properties,
	CD3DX12_RESOURCE_DESC in_rescource_desc,
	D3D12_RESOURCE_STATES in_resource_states,
	D3D12_CLEAR_VALUE* clearValue)
{
	ThrowIfFailed(d3d_device->CreateCommittedResource(
		&in_heap_properties,
		D3D12_HEAP_FLAG_NONE,
		&in_rescource_desc,
		in_resource_states,
		clearValue,
		IID_PPV_ARGS(&in_out_resource)));
}

void directx_render::switch_gpu_resource_state(
	directx_shader_resource* in_gpu_res_elem,
	D3D12_RESOURCE_STATES in_new_resource_states)
{
	if (in_gpu_res_elem->dx_current_state != in_new_resource_states)
	{
		switch_gpu_memory_state(in_gpu_res_elem->dx_resource,
			in_gpu_res_elem->dx_current_state,
			in_new_resource_states);
		in_gpu_res_elem->dx_current_state = in_new_resource_states;
	}
}

void directx_render::switch_gpu_memory_state(
	ComPtr<ID3D12Resource> in_out_resource,
	D3D12_RESOURCE_STATES in_old_resource_states,
	D3D12_RESOURCE_STATES in_new_resource_states)
{
	auto resource_barrier = CD3DX12_RESOURCE_BARRIER::Transition(
		in_out_resource.Get(),
		in_old_resource_states,
		in_new_resource_states);

	command_list->ResourceBarrier(1, &resource_barrier);
}

//DSV UAV undo!!!
///only for table
// low level of expose
void directx_render::create_descriptor(
	DIRECTX_RESOURCE_DESC_TYPE in_texture_desc_type,
	directx_shader_resource* in_gpu_res_elem)
{

	switch (in_texture_desc_type)
	{
		case DIRECTX_RESOURCE_DESC_TYPE::DX_CBV:
		{
			D3D12_CONSTANT_BUFFER_VIEW_DESC desc;
			ZeroMemory(&desc, sizeof(desc));
			desc.BufferLocation = in_gpu_res_elem->dx_resource->GetGPUVirtualAddress();
			desc.SizeInBytes = in_gpu_res_elem->element_count * in_gpu_res_elem->element_size;
			in_gpu_res_elem->dx_cbv = desc;
		}
			break;
		case DIRECTX_RESOURCE_DESC_TYPE::DX_SRV:
		{
			//!!! 如何对应贴图
			D3D12_SHADER_RESOURCE_VIEW_DESC desc;
			ZeroMemory(&desc, sizeof(desc));
			desc.Texture2D.MipLevels = 1;
			desc.Texture2D.MostDetailedMip = 0;
			desc.Format = in_gpu_res_elem->dx_format;
			desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
			desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
			in_gpu_res_elem->dx_srv = desc;
		}
			break;
		case DIRECTX_RESOURCE_DESC_TYPE::DX_RTV:
		{
			D3D12_RENDER_TARGET_VIEW_DESC desc;
			ZeroMemory(&desc, sizeof(desc));
			desc.Texture2D.MipSlice = 0;
			desc.Texture2D.PlaneSlice = 0;
			desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
			desc.Format = in_gpu_res_elem->dx_format;
			in_gpu_res_elem->dx_rtv = desc;
		}
			break;
		case DIRECTX_RESOURCE_DESC_TYPE::DX_DSV:
		{
			D3D12_DEPTH_STENCIL_VIEW_DESC desc;
			desc.Flags = D3D12_DSV_FLAG_NONE;
			desc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
			desc.Format = in_gpu_res_elem->dx_format;
			desc.Texture2D.MipSlice = 0;
			in_gpu_res_elem->dx_dsv = desc;
		}
			break;
		//	DX_UAV
	}

}

//DSV UAV undo!!!
void directx_render::load_descriptor_into_heap(
	const DIRECTX_RESOURCE_DESC_TYPE in_texture_desc_type,
	const directx_shader_resource* in_gpu_res_elem,
	CD3DX12_CPU_DESCRIPTOR_HANDLE in_out_dest_descriptor
	)
{
	switch (in_texture_desc_type)
	{
	case DIRECTX_RESOURCE_DESC_TYPE::DX_CBV:
		//???
		break;
	case DIRECTX_RESOURCE_DESC_TYPE::DX_DSV:
		d3d_device->CreateDepthStencilView(
			in_gpu_res_elem->dx_resource.Get(),
			&in_gpu_res_elem->dx_dsv,
			in_out_dest_descriptor);
		break;
	case DIRECTX_RESOURCE_DESC_TYPE::DX_RTV:
		d3d_device->CreateRenderTargetView(
			in_gpu_res_elem->dx_resource.Get(),
			&in_gpu_res_elem->dx_rtv,
			in_out_dest_descriptor);
		break;
	case DIRECTX_RESOURCE_DESC_TYPE::DX_SRV:
		d3d_device->CreateShaderResourceView(
			in_gpu_res_elem->dx_resource.Get(),
			&in_gpu_res_elem->dx_srv,
			in_out_dest_descriptor);
		break;
	case DIRECTX_RESOURCE_DESC_TYPE::DX_UAV:
		//???
		break;
	}
}

void directx_render::create_rootsignature(
	CD3DX12_ROOT_SIGNATURE_DESC & in_rootsig_desc,
	ComPtr<ID3D12RootSignature> in_out_rootsignature)
{
	ComPtr<ID3DBlob> serializedRootSig = nullptr;
	ComPtr<ID3DBlob> errorBlob = nullptr;
	HRESULT hr = D3D12SerializeRootSignature(&in_rootsig_desc, D3D_ROOT_SIGNATURE_VERSION_1,
		serializedRootSig.GetAddressOf(), errorBlob.GetAddressOf());

	if (errorBlob != nullptr)
	{
		::OutputDebugStringA((char*)errorBlob->GetBufferPointer());
	}
	ThrowIfFailed(hr);

	ThrowIfFailed(d3d_device->CreateRootSignature(
		0,
		serializedRootSig->GetBufferPointer(),
		serializedRootSig->GetBufferSize(),
		IID_PPV_ARGS(in_out_rootsignature.GetAddressOf())));
}

ID3DBlob* directx_render::complie_shader(
	const std::wstring& file_name,
	const D3D_SHADER_MACRO* defines,
	const std::string& entry_point,
	const std::string& target)
{
	UINT compileFlags = 0;

	compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;


	HRESULT hr = S_OK;

	ID3DBlob* byteCode = nullptr;
	ID3DBlob* errors;
	hr = D3DCompileFromFile(file_name.c_str(), defines, D3D_COMPILE_STANDARD_FILE_INCLUDE,
		entry_point.c_str(), target.c_str(), compileFlags, 0, &byteCode, &errors);

	if (errors != nullptr)
		OutputDebugStringA((char*)errors->GetBufferPointer());
	ThrowIfFailed(hr);

	return byteCode;
}

void directx_render::create_pso(
	D3D12_GRAPHICS_PIPELINE_STATE_DESC &in_pso_desc,
	ComPtr<ID3D12PipelineState> in_pso)
{
	ThrowIfFailed(d3d_device->CreateGraphicsPipelineState(
		&in_pso_desc,
		IID_PPV_ARGS(&in_pso)));

}


void directx_render::init(HWND in_main_wnd, UINT in_width, UINT in_height)
{
	//factory & adapter & device & fance
	{
		ThrowIfFailed(CreateDXGIFactory1(IID_PPV_ARGS(&dxgi_factory)));

		// Try to create hardware device.
		HRESULT hard_ware_result = D3D12CreateDevice(
			nullptr,             // default adapter
			D3D_FEATURE_LEVEL_11_0,
			IID_PPV_ARGS(&d3d_device));


		// Fallback to WARP device.
		if (FAILED(hard_ware_result))
		{
			Microsoft::WRL::ComPtr<IDXGIAdapter> warp_adapter;
			ThrowIfFailed(dxgi_factory->EnumWarpAdapter(IID_PPV_ARGS(&warp_adapter)));

			ThrowIfFailed(D3D12CreateDevice(
				warp_adapter.Get(),
				D3D_FEATURE_LEVEL_11_0,
				IID_PPV_ARGS(&d3d_device)));
		}

		ThrowIfFailed(d3d_device->CreateFence(0, D3D12_FENCE_FLAG_NONE,
			IID_PPV_ARGS(&fence)));
	}

	msaa_configuration();

	//commandqueue & allocator & commandlist
	{
		D3D12_COMMAND_QUEUE_DESC queueDesc = {};
		queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		ThrowIfFailed(d3d_device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&command_queue)));

		ThrowIfFailed(d3d_device->CreateCommandAllocator(
			D3D12_COMMAND_LIST_TYPE_DIRECT,
			IID_PPV_ARGS(direct_cmdlist_alloc.GetAddressOf())));

		ThrowIfFailed(d3d_device->CreateCommandList(
			0,
			D3D12_COMMAND_LIST_TYPE_DIRECT,
			direct_cmdlist_alloc.Get(), // Associated command allocator
			nullptr,                   // Initial PipelineStateObject
			IID_PPV_ARGS(command_list.GetAddressOf())));

		// Start off in a closed state.  This is because the first time we refer 
		// to the command list we will Reset it, and it needs to be closed before
		// calling Reset.
		command_list->Close();
	}

	//swapchain
	{
		swap_chain.Reset();

		DXGI_SWAP_CHAIN_DESC sd;
		sd.BufferDesc.Width = in_width;
		sd.BufferDesc.Height = in_height;
		sd.BufferDesc.RefreshRate.Numerator = 60;
		sd.BufferDesc.RefreshRate.Denominator = 1;
		sd.BufferDesc.Format = back_buffer_format;
		sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		sd.SampleDesc.Count = MSAAX4_STATE ? 4 : 1;
		sd.SampleDesc.Quality = MSAAX4_STATE ? (MSAAX4_QUALITY - 1) : 0;
		sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		sd.BufferCount = SWAP_CHAIN_BUFFER_COUNT;
		sd.OutputWindow = in_main_wnd;
		sd.Windowed = true;
		sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

		// Note: Swap chain uses queue to perform flush.
		ThrowIfFailed(dxgi_factory->CreateSwapChain(
			command_queue.Get(),
			&sd,
			swap_chain.GetAddressOf()));
	}


	ScreenViewportResize(in_width, in_height);

	//descriptor size
	{
		rtv_descriptor_size = d3d_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		dsv_descriptor_size = d3d_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
		cbv_srv_uav_descriptor_size = d3d_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	}
}


void directx_render::msaa_configuration()
{
	D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS msQualityLevels;
	msQualityLevels.Format = back_buffer_format;
	msQualityLevels.SampleCount = 4;
	msQualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
	msQualityLevels.NumQualityLevels = 0;
	ThrowIfFailed(d3d_device->CheckFeatureSupport(
		D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS,
		&msQualityLevels,
		sizeof(msQualityLevels)));

	MSAAX4_QUALITY = msQualityLevels.NumQualityLevels;
	assert(MSAAX4_QUALITY > 0 && "Unexpected MSAA quality level.");
}

void directx_render::ScreenViewportResize(int in_width, int in_height)
{
	// Update the viewport transform to cover the client area.
	screen_view_port.TopLeftX = 0;
	screen_view_port.TopLeftY = 0;
	screen_view_port.Width = static_cast<float>(in_width);
	screen_view_port.Height = static_cast<float>(in_height);
	screen_view_port.MinDepth = 0.0f;
	screen_view_port.MaxDepth = 1.0f;

	scissor_rect = { 0, 0, in_width, in_height };
}

void directx_render::screen_vertexs_and_indexes_input()
{
	struct ScreenQuadVertex
	{
		DirectX::XMFLOAT4 position;
		DirectX::XMFLOAT2 texcoord;
	};

	ScreenQuadVertex QuadVerts[] =
	{
		{ { -1.0f,1.0f, 0.0f,1.0f },{ 0.0f,0.0f } },
		{ { 1.0f, 1.0f, 0.0f,1.0f }, {1.0f,0.0f } },
		{ { -1.0f, -1.0f, 0.0f,1.0f },{ 0.0f,1.0f } },
	{ { 1.0f, -1.0f, 0.0f,1.0f },{ 1.0f,1.0f } }
	};


	CD3DX12_RESOURCE_DESC resourceDesc;//??? CD3D12_RESOURCE_DESC & CD3DX12_RESOURCE_DESC
	ZeroMemory(&resourceDesc, sizeof(resourceDesc));
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resourceDesc.Alignment = 0;
	resourceDesc.SampleDesc.Count = 1;
	resourceDesc.SampleDesc.Quality = 0;
	resourceDesc.MipLevels = 1;
	resourceDesc.Format = DXGI_FORMAT_UNKNOWN;
	resourceDesc.DepthOrArraySize = 1;
	resourceDesc.Width = sizeof(QuadVerts);
	resourceDesc.Height = 1;
	resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	CD3DX12_HEAP_PROPERTIES heapproperties(D3D12_HEAP_TYPE_UPLOAD);

	create_gpu_memory(
		screen_vertex_gpu_resource,
		heapproperties,
		resourceDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ);

	screen_vertex_view.BufferLocation = screen_vertex_gpu_resource->GetGPUVirtualAddress();
	screen_vertex_view.StrideInBytes = sizeof(ScreenQuadVertex);
	screen_vertex_view.SizeInBytes = sizeof(QuadVerts);
}
