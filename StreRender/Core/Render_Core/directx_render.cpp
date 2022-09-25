#include "directx_render.h"
#include <WindowsX.h>
#include "Core/Exception/exception.h"
#include <array>


//
// 所有资源都已经装好在GPU后在这使用?
//
// input: GPU_object
// input: GPU_camera
// input: GPU_light
// input: GPU_mat
// input: GPU_texture
// input: GPU_gpu_map
// input: GPU_matrix
// output: GPU_gpu_map
//
void directx_render::draw_call()
{
	
}

std::array<const CD3DX12_STATIC_SAMPLER_DESC, 7Ui64> GetStaticSamplers()
{
	// Applications usually only need a handful of samplers.  So just define them all up front
	// and keep them available as part of the root signature.  

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

	return {
		pointWrap, pointClamp,
		linearWrap, linearClamp,
		anisotropicWrap, anisotropicClamp,
		shadow
	};
}

//BuildDescriptorHeaps ???
//CreateDescriptors ???

//pass creation
//BuildRootSignature
//BuildShadersAndInputLayout
//BuildPSO
s_memory_allocater_register pass_allocater("pass_allocater");

constant_pass* directx_render::allocate_pass(constant_pass::pass_layout in_constant_pass_layout)
{
	typedef constant_pass::PASS_RESOURCE_TYPE PASS_RES_TYPE;

	auto pass_allocater = memory_allocater_group["pass_allocater"];

	directx_constant_pass* pass = (directx_constant_pass*)pass_allocater->allocate<directx_constant_pass>();
	
	//RootSignature
	{
		auto input_resource = in_constant_pass_layout.input_gpu_resource_layout;
		UINT input_res_number = in_constant_pass_layout.input_gpu_resource_layout.size();
		std::vector<CD3DX12_ROOT_PARAMETER> slotRootParameter(input_res_number);

		for (UINT i = 0; i < input_res_number; i++)
		{
			switch (input_resource[i].input_resource_type)
			{
			case PASS_RES_TYPE::PASS_RES_CBV:
				slotRootParameter[i].InitAsConstantBufferView(i);
				break;
			case PASS_RES_TYPE::PASS_RES_SRV:
				slotRootParameter[i].InitAsShaderResourceView(i);
				break;
			case PASS_RES_TYPE::PASS_RES_DESCRIPTOR_TBALE:
				CD3DX12_DESCRIPTOR_RANGE texTable;
				texTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, input_resource[i].input_resource_number, i);

				slotRootParameter[i].InitAsDescriptorTable(1, &texTable, D3D12_SHADER_VISIBILITY_PIXEL);
				break;
			}
		}

		auto staticSamplers = GetStaticSamplers();

		// A root signature is an array of root parameters.
		CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc(input_res_number, slotRootParameter.data(),
			(UINT)staticSamplers.size(), staticSamplers.data(),
			D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

		create_rootsignature(
			rootSigDesc,
			pass->rootsignature);
	}

	//Shader Input layout
	{
		typedef constant_pass::shader_layout::SHADER_TYPE SHADER_TYPE;
		typedef constant_pass::shader_layout::shader_input::INPUT_ELEMENT_SIZE INPUT_ELEMENT_SIZE;
		auto shader_layout = in_constant_pass_layout.pass_shader_layout;
		if (shader_layout.shader_vaild[SHADER_TYPE::VS])
		{
			pass->shader_group[pass->pass_name + "VS"]
				= complie_shader(
					shader_layout.shader_path[SHADER_TYPE::VS],
					nullptr, "VS", "vs_5_1");
		}
		if (shader_layout.shader_vaild[SHADER_TYPE::DS])
		{
			pass->shader_group[pass->pass_name + "DS"]
				= complie_shader(
					shader_layout.shader_path[SHADER_TYPE::DS],
					nullptr, "DS", "ds_5_1");
		}
		if (shader_layout.shader_vaild[SHADER_TYPE::HS])
		{
			pass->shader_group[pass->pass_name + "HS"]
				= complie_shader(
					shader_layout.shader_path[SHADER_TYPE::HS],
					nullptr, "HS", "hs_5_1");
		}
		if (shader_layout.shader_vaild[SHADER_TYPE::GS])
		{
			pass->shader_group[pass->pass_name + "GS"]
				= complie_shader(
					shader_layout.shader_path[SHADER_TYPE::GS],
					nullptr, "GS", "gs_5_1");
		}
		if (shader_layout.shader_vaild[SHADER_TYPE::PS])
		{
			pass->shader_group[pass->pass_name + "PS"]
				= complie_shader(
					shader_layout.shader_path[SHADER_TYPE::PS],
					nullptr, "PS", "ps_5_1");
		}

		UINT elem_size_offset = 0;
		for (auto it : shader_layout.shader_input_group)
		{
			D3D12_INPUT_ELEMENT_DESC input_elem_desc;

			DXGI_FORMAT input_elem_format;
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
	}

	//??? 透明功能，很多功能都忽视了
	//PSO
	{
		
		typedef constant_pass::shader_layout::SHADER_TYPE SHADER_TYPE;
		auto shader_layout = in_constant_pass_layout.pass_shader_layout;

		auto rt_number = in_constant_pass_layout.output_gpu_resource.size();


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
		for (int i = 0; i < rt_number; i++)
			PsoDesc.RTVFormats[i] = back_buffer_format;


		

		if (shader_layout.shader_vaild[SHADER_TYPE::VS])
		{
			PsoDesc.VS =
			{
				reinterpret_cast<BYTE*>(
					pass->shader_group[pass->pass_name + "VS"]->GetBufferPointer()),
				pass->shader_group[pass->pass_name + "VS"]->GetBufferSize()
			};
		}
		if (shader_layout.shader_vaild[SHADER_TYPE::DS])
		{
			PsoDesc.DS =
			{
				reinterpret_cast<BYTE*>(
					pass->shader_group[pass->pass_name + "DS"]->GetBufferPointer()),
				pass->shader_group[pass->pass_name + "DS"]->GetBufferSize()
			};
		}
		if (shader_layout.shader_vaild[SHADER_TYPE::HS])
		{
			PsoDesc.HS =
			{
				reinterpret_cast<BYTE*>(
					pass->shader_group[pass->pass_name + "HS"]->GetBufferPointer()),
				pass->shader_group[pass->pass_name + "HS"]->GetBufferSize()
			};
		}
		if (shader_layout.shader_vaild[SHADER_TYPE::GS])
		{
			PsoDesc.GS =
			{
				reinterpret_cast<BYTE*>(
					pass->shader_group[pass->pass_name + "GS"]->GetBufferPointer()),
				pass->shader_group[pass->pass_name + "GS"]->GetBufferSize()
			};
		}
		if (shader_layout.shader_vaild[SHADER_TYPE::PS])
		{
			PsoDesc.PS =
			{
				reinterpret_cast<BYTE*>(
					pass->shader_group[pass->pass_name + "PS"]->GetBufferPointer()),
				pass->shader_group[pass->pass_name + "PS"]->GetBufferSize()
			};
		}

		create_pso(
			PsoDesc,
			pass->pso);
	}

}

void directx_render::allocate_upload_resource( 
	UINT in_elem_size, 
	UINT in_number, 
	directx_gpu_resource_element* in_res_elem)
{
	in_res_elem->element_byte_size = sizeof(in_elem_size);
	in_res_elem->memory_size = in_res_elem->element_byte_size * in_number;

	CD3DX12_HEAP_PROPERTIES Heapproperties(D3D12_HEAP_TYPE_UPLOAD);
	CD3DX12_RESOURCE_DESC Resourcedesc = CD3DX12_RESOURCE_DESC::Buffer(in_res_elem->element_byte_size * in_number);

	create_gpu_memory(
		in_res_elem->dx_resource,
		Heapproperties,
		Resourcedesc
	);

	ThrowIfFailed(in_res_elem->dx_resource->Map(0, nullptr, reinterpret_cast<void**>(&(in_res_elem->mapped_data))));

}

void directx_render::update_all_upload_resource(
	void* data,
	directx_gpu_resource_element* in_res_elem)
{
	memcpy(&(in_res_elem->mapped_data[0]), data, in_res_elem->memory_size);
}

void directx_render::update_elem_upload_resource(
	void* data,
	int element_index,
	directx_gpu_resource_element* in_res_elem)
{
	memcpy(&(in_res_elem->mapped_data[element_index * in_res_elem->element_byte_size]), &data, in_res_elem->element_byte_size);
}

void directx_render::allocate_default_resource(
	UINT in_buffer_size, 
	void* in_cpu_data, 
	directx_gpu_resource_element* in_res_elem)
{
	in_res_elem->memory_size = in_buffer_size;
	in_res_elem->element_byte_size = in_buffer_size;

	// Create the actual default buffer resource.
	CD3DX12_HEAP_PROPERTIES Heapproperties(D3D12_HEAP_TYPE_DEFAULT);
	CD3DX12_RESOURCE_DESC Resourcedesc = CD3DX12_RESOURCE_DESC::Buffer(in_buffer_size);

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
	Resourcedesc = CD3DX12_RESOURCE_DESC::Buffer(in_buffer_size);

	create_gpu_memory(
		upload_buffer,
		Heappropertiesup,
		Resourcedesc
	);

	// Describe the data we want to copy into the default buffer.
	D3D12_SUBRESOURCE_DATA subResourceData = {};
	subResourceData.pData = in_cpu_data;
	subResourceData.RowPitch = in_buffer_size;
	subResourceData.SlicePitch = subResourceData.RowPitch;

	// Schedule to copy the data to the default buffer resource.  At a high level, the helper function UpdateSubresources
	// will copy the CPU memory into the intermediate upload heap.  Then, using ID3D12CommandList::CopySubresourceRegion,
	// the intermediate upload heap data will be copied to mBuffer.
	switch_gpu_memory_state(
		in_res_elem->dx_resource, 
		D3D12_RESOURCE_STATE_COMMON, 
		D3D12_RESOURCE_STATE_COPY_DEST);

	UpdateSubresources<1>(
		command_list.Get(), 
		in_res_elem->dx_resource.Get(), 
		upload_buffer.Get(), 
		0, 0, 1, &subResourceData);

	switch_gpu_memory_state(
		in_res_elem->dx_resource, 
		D3D12_RESOURCE_STATE_COPY_DEST, 
		D3D12_RESOURCE_STATE_COMMON);


	// Note: uploadBuffer has to be kept alive after the above function calls because
	// the command list has not been executed yet that performs the actual copy.
	// The caller can Release the uploadBuffer after it knows the copy has been executed.
}



s_memory_allocater_register gpu_resource_element_ptr_allocater("gpu_resource_element_ptr_allocater");

gpu_resource_element* directx_render::allocate_gpu_memory(
	GPU_RESOURCE_LAYOUT in_resource_layout)
{
	typedef GPU_RESOURCE_LAYOUT::GPU_RESOURCE_TYPE GPU_RES_TYPE;
	typedef GPU_RESOURCE_LAYOUT::GPU_RESOURCE_STATE GPU_RES_STATE;
	auto allocater = memory_allocater_group["gpu_resource_element_ptr_allocater"];

	directx_gpu_resource_element* dx_gpu_res_elem = (directx_gpu_resource_element*)allocater->allocate<directx_gpu_resource_element>();
	dx_gpu_res_elem->name = in_resource_layout.gpu_resource_name;
	auto gpu_res_type = in_resource_layout.gpu_resource_type;
	auto gpu_res_state = in_resource_layout.gpu_resource_state;
	
	//switch (gpu_res_type)
	//{
	//case GPU_RES_TYPE::GPU_RES_BUFFER:
	//	break;
	//case GPU_RES_TYPE::GPU_RES_TEXTURE:
	//	break;
	//case GPU_RES_TYPE::GPU_RES_VERTEX:
	//	break;
	//case GPU_RES_TYPE::GPU_RES_INDEX:
	//	break;
	//}
	switch (gpu_res_state)
	{
	case GPU_RES_STATE::GPU_RES_CONSTANT:
		allocate_default_resource(
			in_resource_layout.cpu_data_size,
			in_resource_layout.cpu_data,
			dx_gpu_res_elem);
			break;
	case GPU_RES_STATE::GPU_RES_UPLOAD:
		allocate_upload_resource(
			in_resource_layout.cpu_data_size,
			in_resource_layout.cpu_data_number,
			dx_gpu_res_elem);
			break;
	}
	return dx_gpu_res_elem;
}




gpu_resource_element* directx_render::create_gpu_texture(
	std::string in_gpu_texture_name, 
	GPU_RESOURCE_LAYOUT::GPU_RESOURCE_TYPE in_resoure_type)
{
	typedef GPU_RESOURCE_LAYOUT::GPU_RESOURCE_TYPE GPU_RES_TYPE;
	typedef GPU_RESOURCE_LAYOUT::GPU_RESOURCE_STATE GPU_RES_STATE;
	auto allocater = memory_allocater_group["gpu_resource_element_ptr_allocater"];

	directx_gpu_resource_element* dx_gpu_res_elem = (directx_gpu_resource_element*)allocater->allocate<directx_gpu_resource_element>();
	dx_gpu_res_elem->name = in_gpu_texture_name;

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
	resourceDesc.Format = back_buffer_format;

	if (in_resoure_type == GPU_RES_TYPE::GPU_RES_RENDER_TARGET)
	{
		resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;

		D3D12_CLEAR_VALUE clearVal;
		clearVal.Color[0] = clear_color[0];
		clearVal.Color[1] = clear_color[1];
		clearVal.Color[2] = clear_color[2];
		clearVal.Color[3] = clear_color[3];
		clearVal.Format = back_buffer_format;

		CD3DX12_HEAP_PROPERTIES heapproperties(D3D12_HEAP_TYPE_DEFAULT);

		create_gpu_memory(
			dx_gpu_res_elem->dx_resource,
			heapproperties,
			resourceDesc,
			default_resource_states,
			&clearVal);

		D3D12_RENDER_TARGET_VIEW_DESC descRTV;
		ZeroMemory(&descRTV, sizeof(descRTV));
		descRTV.Texture2D.MipSlice = 0;
		descRTV.Texture2D.PlaneSlice = 0;
		descRTV.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
		descRTV.Format = back_buffer_format;

		D3D12_SHADER_RESOURCE_VIEW_DESC descSRV;
		ZeroMemory(&descSRV, sizeof(descSRV));
		descSRV.Texture2D.MipLevels = 1;
		descSRV.Texture2D.MostDetailedMip = 0;
		descSRV.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		descSRV.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		descSRV.Format = back_buffer_format;

		dx_gpu_res_elem->dx_rtv = descRTV;
		dx_gpu_res_elem->dx_srv = descSRV;
	}
	else if (in_resoure_type == GPU_RES_TYPE::GPU_RES_DEPTH_STENCIL)
	{
		resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

		D3D12_CLEAR_VALUE optClear;
		optClear.Format = depth_stencil_format;
		optClear.DepthStencil.Depth = 1.0f;
		optClear.DepthStencil.Stencil = 0;
		
		CD3DX12_HEAP_PROPERTIES heapproperties(D3D12_HEAP_TYPE_DEFAULT);

		create_gpu_memory(
			dx_gpu_res_elem->dx_resource,
			heapproperties,
			resourceDesc,
			default_resource_states,
			&optClear);

		// Create descriptor to mip level 0 of entire resource using the format of the resource.
		D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc;
		dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
		dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
		dsvDesc.Format = depth_stencil_format;
		dsvDesc.Texture2D.MipSlice = 0;

		D3D12_SHADER_RESOURCE_VIEW_DESC descSRV;
		ZeroMemory(&descSRV, sizeof(descSRV));
		descSRV.Texture2D.MipLevels = 1;
		descSRV.Texture2D.MostDetailedMip = 0;
		descSRV.Format = depth_stencil_format;
		descSRV.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		descSRV.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		
		dx_gpu_res_elem->dx_dsv = dsvDesc;
		dx_gpu_res_elem->dx_srv = descSRV;

	}
}

//
void directx_render::create_descriptor_heap(
	D3D12_DESCRIPTOR_HEAP_DESC & in_dx_descheap_desc, 
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> in_out_descheap)
{
	ThrowIfFailed(d3d_device->CreateDescriptorHeap(
		&in_dx_descheap_desc,
		IID_PPV_ARGS(in_out_descheap.GetAddressOf())));
}

//vertex 
//index 
//texture
//默认构建 common的缓冲区
void directx_render::create_gpu_memory(
	ComPtr<ID3D12Resource> in_out_resource,
	CD3DX12_HEAP_PROPERTIES in_heap_properties,
	CD3DX12_RESOURCE_DESC in_rescource_desc,
	D3D12_RESOURCE_STATES in_resource_states,
	D3D12_CLEAR_VALUE* clearValue = nullptr)
{
	ThrowIfFailed(d3d_device->CreateCommittedResource(
		&in_heap_properties,
		D3D12_HEAP_FLAG_NONE,
		&in_rescource_desc,
		in_resource_states,
		clearValue,
		IID_PPV_ARGS(&in_out_resource)));
}

void directx_render::switch_gpu_memory_state(
	ComPtr<ID3D12Resource> in_out_resource,
	D3D12_RESOURCE_STATES in_old_resource_states,
	D3D12_RESOURCE_STATES in_new_resource_states)
{
	command_list->ResourceBarrier(
		1, 
		&CD3DX12_RESOURCE_BARRIER::Transition(
			in_out_resource.Get(),
			in_old_resource_states,
			in_new_resource_states));
}

//CSV UAV undo!!!
void directx_render::create_gpu_memory_view(
	DIRECTX_RESOURCE_DESC_TYPE in_texture_desc_type,
	directx_gpu_resource_element* in_gpu_resource,
	D3D12_CPU_DESCRIPTOR_HANDLE in_out_dest_descriptor
	)
{
	switch (in_texture_desc_type)
	{
	case DIRECTX_RESOURCE_DESC_TYPE::DX_CBV:
		//???
		break;
	case DIRECTX_RESOURCE_DESC_TYPE::DX_DSV:
		d3d_device->CreateDepthStencilView(
			in_gpu_resource->dx_resource.Get(),
			&in_gpu_resource->dx_dsv,
			in_out_dest_descriptor);
		break;
	case DIRECTX_RESOURCE_DESC_TYPE::DX_RTV:
		d3d_device->CreateRenderTargetView(
			in_gpu_resource->dx_resource.Get(),
			&in_gpu_resource->dx_rtv, 
			in_out_dest_descriptor);
		break;
	case DIRECTX_RESOURCE_DESC_TYPE::DX_SRV:
		d3d_device->CreateShaderResourceView(
			in_gpu_resource->dx_resource.Get(),
			&in_gpu_resource->dx_srv,
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


void directx_render::init(HWND in_main_wnd)
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

	msaa_configuration();

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

	swap_chain.Reset();

	DXGI_SWAP_CHAIN_DESC sd;
	sd.BufferDesc.Width = CLIENT_WIDTH;
	sd.BufferDesc.Height = CLIENT_HEIGHT;
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