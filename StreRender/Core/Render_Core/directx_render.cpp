#ifdef  DLL_GRAPHICS_API
#else
#define DLL_GRAPHICS_API _declspec(dllexport)
#endif

#include "directx_render.h"
#include "stre_render.h"
#include <WindowsX.h>
#include "Core/Exception/exception.h"
#include <array>
#include <string>
#include "Core/Memory/s_memory.h"
#include <d3d12sdklayers.h>

namespace stre_exception
{
	DLL_GRAPHICS_API std::list<std::string> exception_output_str_group;
	DLL_GRAPHICS_API int fence; //字符串数组上一次获取的位置
};


/***
************************************************************
*
* Pass Function
*
************************************************************
*/

gpu_pass* directx_render::allocate_pass()
{
	return new directx_render::directx_pass();
}

bool directx_render::create_rootsignature(gpu_pass* in_gpu_pass)
{
	//DEBUG

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

	//按照
	UINT input_cb_number = 0;
	UINT input_texture_number = 0;
	std::vector<CD3DX12_ROOT_PARAMETER> slotRootParameter;
	std::vector<CD3DX12_DESCRIPTOR_RANGE*> texture_table_group;
	//用反射得到的数据自动构建根签名
	for (auto it : in_gpu_pass->pass_res_group)
	{
		switch (it.second.type)//反射出来的种类
		{
		case gpu_shader_resource::SHADER_RESOURCE_TYPE_CUSTOM_BUFFER:
		{
			CD3DX12_ROOT_PARAMETER r_p;
			r_p.InitAsConstantBufferView(it.second.bind_point);
			slotRootParameter.push_back(r_p);
		}
		break;
		case gpu_shader_resource::SHADER_RESOURCE_TYPE_CUSTOM_BUFFER_GROUP:

		{
			CD3DX12_ROOT_PARAMETER r_p;
			r_p.InitAsShaderResourceView(it.second.bind_point);
			slotRootParameter.push_back(r_p);
		}
		break;
		case gpu_shader_resource::SHADER_RESOURCE_TYPE_TEXTURE_GROUP:
		{
			//局部状态下在根签名构造完成前不能被销毁
			CD3DX12_DESCRIPTOR_RANGE* texTable = new CD3DX12_DESCRIPTOR_RANGE();
			texture_table_group.push_back(texTable);
			texTable->Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, it.second.bind_count, it.second.bind_point, it.second.register_space);
			CD3DX12_ROOT_PARAMETER texture_rp;
			//table的初始化要求参数一直存在，不能被提前释放 所以要用堆指针存储
			texture_rp.InitAsDescriptorTable(1, texTable, D3D12_SHADER_VISIBILITY_ALL);
			slotRootParameter.push_back(texture_rp);
		}
		break;
		default:
			break;
		}
	}

	auto staticSamplers = smaples_group;

	CD3DX12_ROOT_PARAMETER* ptr = slotRootParameter.data();

	CD3DX12_ROOT_SIGNATURE_DESC rootsig_desc((UINT)slotRootParameter.size(), ptr,
		(UINT)staticSamplers.size(), staticSamplers.data(),
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	auto root_ptr = static_cast<directx_pass*>(in_gpu_pass);


	create_rootsignature(
		rootsig_desc,
		root_ptr->rootsignature);

	for (auto it : texture_table_group)
	{
		delete(it);
	}

	return true;
}

bool directx_render::create_pso(
	shader_layout& in_shader_layout,
	gpu_pass* in_gpu_pass)
{
	typedef shader_layout::SHADER_TYPE SHADER_TYPE;
	typedef shader_layout::shader_input::INPUT_ELEMENT_SIZE INPUT_ELEMENT_SIZE;

	{
		//TODO TRANSLATE & DEPTH
		auto pass = static_cast<directx_pass*>(in_gpu_pass);

		auto rt_number = in_gpu_pass->rt_number;

		CD3DX12_RASTERIZER_DESC RastDesc(D3D12_DEFAULT);
		RastDesc.CullMode = D3D12_CULL_MODE_NONE;
		//RastDesc.DepthClipEnable = in_gpu_pass->is_depth_check;//???

		D3D12_GRAPHICS_PIPELINE_STATE_DESC PsoDesc;
		ZeroMemory(&PsoDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
		PsoDesc.InputLayout = { pass->input_layout.data(), (UINT)pass->input_layout.size() };
		PsoDesc.pRootSignature = pass->rootsignature.Get();
		PsoDesc.RasterizerState = RastDesc;
		PsoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
		PsoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
		PsoDesc.DepthStencilState.DepthEnable = in_gpu_pass->is_depth_check;

		PsoDesc.SampleMask = UINT_MAX;
		PsoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		PsoDesc.SampleDesc.Count = MSAAX4_STATE ? 4 : 1;
		PsoDesc.SampleDesc.Quality = MSAAX4_STATE ? (MSAAX4_QUALITY - 1) : 0;
		PsoDesc.DSVFormat = depth_stencil_format;
		if (in_gpu_pass->is_output)
		{
			PsoDesc.NumRenderTargets = 1;
			PsoDesc.RTVFormats[0] = back_buffer_format;
		}
		else
		{
			PsoDesc.NumRenderTargets = rt_number;
			for (UINT i = 0; i < rt_number; i++)
				PsoDesc.RTVFormats[i] = render_target_format;
		}

		if (in_shader_layout.shader_vaild[SHADER_TYPE::VS])
		{
			if (!pass->shader_group[std::string(pass->uid.name) + "VS"])
			{
				return false;
			}
			PsoDesc.VS =
			{
				reinterpret_cast<BYTE*>(
					pass->shader_group[std::string(pass->uid.name) + "VS"]->GetBufferPointer()),
				pass->shader_group[std::string(pass->uid.name) + "VS"]->GetBufferSize()
			};
		}
		if (in_shader_layout.shader_vaild[SHADER_TYPE::DS])
		{
			if (!pass->shader_group[std::string(pass->uid.name) + "DS"])
			{
				return false;
			}
			PsoDesc.DS =
			{
				reinterpret_cast<BYTE*>(
					pass->shader_group[std::string(pass->uid.name) + "DS"]->GetBufferPointer()),
				pass->shader_group[std::string(pass->uid.name) + "DS"]->GetBufferSize()
			};
		}
		if (in_shader_layout.shader_vaild[SHADER_TYPE::HS])
		{
			if (!pass->shader_group[std::string(pass->uid.name) + "HS"])
			{
				return false;
			}
			PsoDesc.HS =
			{
				reinterpret_cast<BYTE*>(
					pass->shader_group[std::string(pass->uid.name) + "HS"]->GetBufferPointer()),
				pass->shader_group[std::string(pass->uid.name) + "HS"]->GetBufferSize()
			};
		}
		if (in_shader_layout.shader_vaild[SHADER_TYPE::GS])
		{
			if (!pass->shader_group[std::string(pass->uid.name) + "GS"])
			{
				return false;
			}
			PsoDesc.GS =
			{
				reinterpret_cast<BYTE*>(
					pass->shader_group[std::string(pass->uid.name) + "GS"]->GetBufferPointer()),
				pass->shader_group[std::string(pass->uid.name) + "GS"]->GetBufferSize()
			};
		}
		if (in_shader_layout.shader_vaild[SHADER_TYPE::PS])
		{
			if (!pass->shader_group[std::string(pass->uid.name) + "PS"])
			{
				return false;
			}
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
	return true;
}

bool directx_render::complie_shader(
	shader_layout& in_shader_layout,
	gpu_pass* in_gpu_pass)
{
	typedef shader_layout::SHADER_TYPE SHADER_TYPE;
	typedef shader_layout::shader_input::INPUT_ELEMENT_SIZE INPUT_ELEMENT_SIZE;
	auto& shader_layout = in_shader_layout;
	auto pass = static_cast<directx_pass*>(in_gpu_pass);
	bool execute_result = true;

	if (!in_gpu_pass)
	{
		//!!!出问题了
		stre_exception::exception_output_str_group.push_back("directx_render::complie_shader in_gpu_pass is nullptr");
		return false;
	}

	//检查着色器输入是否正确
	bool valable = false;
	for (auto it : shader_layout.shader_vaild)
	{
		valable |= it;
	}
	if (!valable)
	{
		//!!!出问题了
		stre_exception::exception_output_str_group.push_back("directx_render::complie_shader none of Shader Function is valuable");
		return false;
	}


	if (in_shader_layout.shader_vaild[SHADER_TYPE::VS])
	{
		pass->shader_group[std::string(pass->uid.name) + "VS"]
			= complie_shader(
				shader_layout.shader_path[SHADER_TYPE::VS],
				nullptr, "VS", "vs_5_1", execute_result);
		if (!execute_result) return execute_result;
	}
	if (in_shader_layout.shader_vaild[SHADER_TYPE::DS])
	{
		pass->shader_group[std::string(pass->uid.name) + "DS"]
			= complie_shader(
				shader_layout.shader_path[SHADER_TYPE::DS],
				nullptr, "DS", "ds_5_1", execute_result);
		if (!execute_result) return execute_result;
	}
	if (in_shader_layout.shader_vaild[SHADER_TYPE::HS])
	{
		pass->shader_group[std::string(pass->uid.name) + "HS"]
			= complie_shader(
				shader_layout.shader_path[SHADER_TYPE::HS],
				nullptr, "HS", "hs_5_1", execute_result);
		if (!execute_result) return execute_result;
	}
	if (in_shader_layout.shader_vaild[SHADER_TYPE::GS])
	{
		pass->shader_group[std::string(pass->uid.name) + "GS"]
			= complie_shader(
				shader_layout.shader_path[SHADER_TYPE::GS],
				nullptr, "GS", "gs_5_1", execute_result);
		if (!execute_result) return execute_result;
	}
	if (in_shader_layout.shader_vaild[SHADER_TYPE::PS])
	{
		pass->shader_group[std::string(pass->uid.name) + "PS"]
			= complie_shader(
				shader_layout.shader_path[SHADER_TYPE::PS],
				nullptr, "PS", "ps_5_1", execute_result);
		if (!execute_result) return execute_result;
	}

	UINT elem_size_offset = 0;
	for (int i = 0; i < shader_layout.shader_input_group.size(); i++)
	{
		auto& it = shader_layout.shader_input_group[i];
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

	
	//反射获取输入资源

	for (auto it : pass->shader_group)
	{
		reflect_shader(it.second, pass->pass_res_group);
	}

	return execute_result;
}

/***
************************************************************
*
* Shader Resource Function
*
************************************************************
*/

//s_memory_allocater_register gpu_shader_res_allocater("gpu_shader_res_allocater");

/// <summary>
/// 自动构建帧资源和描述符
/// GPU的texture会申请空间
/// </summary>
/// <param name="in_shader_res_type"></param>
/// <returns></returns>
std::shared_ptr<gpu_shader_resource> directx_render::allocate_shader_resource(gpu_shader_resource::SHADER_RESOURCE_TYPE in_shader_res_type)
{
	//auto gpu_shader_res_allocater = memory_allocater_group["gpu_shader_res_allocater"];
	switch (in_shader_res_type)
	{
		//结构体
	case gpu_shader_resource::SHADER_RESOURCE_TYPE_CUSTOM_BUFFER:
	{
		std::shared_ptr<directx_frame_resource> frame_res(new directx_frame_resource());
		frame_res->shader_resource_type = in_shader_res_type;
		for (int i = 0; i < FRAME_BUFFER_COUNT; i++)
		{
			frame_res->frame_resource_group[i] = std::make_shared<directx_sr_custom_buffer>();
			//分配空间时才提供CSV/SRV描述符
		}
		return std::dynamic_pointer_cast<gpu_shader_resource>(frame_res);
	}
	break;

	//结构体组 SRV
	case gpu_shader_resource::SHADER_RESOURCE_TYPE_CUSTOM_BUFFER_FOLLOW_MESH:
	case gpu_shader_resource::SHADER_RESOURCE_TYPE_CUSTOM_BUFFER_GROUP:
	{
		std::shared_ptr<directx_frame_resource> frame_res( new directx_frame_resource());
		frame_res->shader_resource_type = in_shader_res_type;
		for (int i = 0; i < FRAME_BUFFER_COUNT; i++)
		{
			frame_res->frame_resource_group[i] = std::make_shared <directx_sr_custom_buffer_group>();
			//分配空间时才提供CSV/SRV描述符
		}
		return std::dynamic_pointer_cast<gpu_shader_resource>(frame_res);;
	}
	break;

	//贴图 SRV
	case gpu_shader_resource::SHADER_RESOURCE_TYPE_TEXTURE:
	{
		std::shared_ptr<directx_texture_resource> texture_res(new directx_texture_resource());
		texture_res->shader_resource_type = in_shader_res_type;
		texture_res->resource = std::make_shared <directx_sr_texture>();
		std::shared_ptr<directx_sr_texture> gpu_ptr = std::static_pointer_cast<directx_sr_texture>(texture_res->resource);

		//不应该让外面控制精度吗？？？
		gpu_ptr->dx_format = back_buffer_format;

		//建了也只有被集中进table时才有用
		create_descriptor(
			DIRECTX_RESOURCE_DESC_TYPE::DX_SRV,
			gpu_ptr);

		return std::dynamic_pointer_cast<gpu_shader_resource>(texture_res);
	}
	break;
	//贴图堆
	case gpu_shader_resource::SHADER_RESOURCE_TYPE_TEXTURE_GROUP:
	{
		std::shared_ptr < directx_texture_resource> texture_res(new directx_texture_resource());
		texture_res->shader_resource_type = in_shader_res_type;
		texture_res->resource = std::make_shared<directx_sr_texture_group>();

		return std::dynamic_pointer_cast<gpu_shader_resource>(texture_res);
	}
	break;
	//RT
	case gpu_shader_resource::SHADER_RESOURCE_TYPE_RENDER_TARGET:
	{
		std::shared_ptr < directx_texture_resource> texture_res(new directx_texture_resource());
		texture_res->shader_resource_type = in_shader_res_type;
		texture_res->resource = std::make_shared<directx_sr_render_target>();
		std::shared_ptr < directx_sr_render_target> gpu_ptr = std::static_pointer_cast<directx_sr_render_target>(texture_res->resource);

		gpu_ptr->dx_format = render_target_format;
		create_gpu_texture(gpu_ptr, in_shader_res_type);

		create_descriptor(
			DIRECTX_RESOURCE_DESC_TYPE::DX_SRV,
			gpu_ptr);

		create_descriptor(
			DIRECTX_RESOURCE_DESC_TYPE::DX_RTV,
			gpu_ptr);

		return std::dynamic_pointer_cast<gpu_shader_resource>(texture_res);
	}
	break;
	//RT堆
	case gpu_shader_resource::SHADER_RESOURCE_TYPE_RENDER_TARGET_GROUP:
	{
		std::shared_ptr<directx_texture_resource> texture_res(new directx_texture_resource());
		texture_res->shader_resource_type = in_shader_res_type;
		texture_res->resource = std::make_shared<directx_sr_render_target_group>();

		return std::dynamic_pointer_cast<gpu_shader_resource>(texture_res);
	}
	break;
	//Depth
	case gpu_shader_resource::SHADER_RESOURCE_TYPE_RENDER_DEPTH_STENCIL:
	{
		std::shared_ptr < directx_texture_resource> texture_res(new directx_texture_resource());
		texture_res->shader_resource_type = in_shader_res_type;
		texture_res->resource = std::make_shared <directx_sr_depth_stencil>();
		std::shared_ptr < directx_sr_depth_stencil> gpu_ptr = std::static_pointer_cast<directx_sr_depth_stencil>(texture_res->resource);

		gpu_ptr->dx_format = depth_stencil_format;
		create_gpu_texture(gpu_ptr, in_shader_res_type);

		create_descriptor(
			DIRECTX_RESOURCE_DESC_TYPE::DX_SRV,
			gpu_ptr);

		create_descriptor(
			DIRECTX_RESOURCE_DESC_TYPE::DX_DSV,
			gpu_ptr);
		return std::dynamic_pointer_cast<gpu_shader_resource>(texture_res);
	}
	//Depth堆
	case gpu_shader_resource::SHADER_RESOURCE_TYPE_RENDER_DEPTH_STENCIL_GROUP:
	{
		std::shared_ptr < directx_texture_resource> texture_res(new directx_texture_resource());
		texture_res->shader_resource_type = in_shader_res_type;
		texture_res->resource = std::make_shared<directx_sr_depth_stencil_group>();

		return std::dynamic_pointer_cast<gpu_shader_resource>(texture_res);
	}
	break;
	}
	return nullptr;
}

bool directx_render::allocate_default_resource(
	std::shared_ptr<gpu_shader_resource> in_res,
	UINT in_elem_size,
	UINT in_number,
	void* in_cpu_data)
{
	std::shared_ptr<directx_frame_resource> frame_res = std::static_pointer_cast<directx_frame_resource>(in_res);

	for (int i = 0; i < FRAME_BUFFER_COUNT; i++)
	{
		auto in_res_elem = std::static_pointer_cast<directx_shader_resource>(frame_res->frame_resource_group[i]);
		UINT memory_size = in_elem_size * in_number;
		frame_res->element_size = in_elem_size;
		frame_res->element_count = in_number;
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
			dx_command_list.Get(),
			in_res_elem->dx_resource.Get(),
			upload_buffer.Get(),
			0, 0, 1, &subResourceData);

		switch_gpu_memory_state(
			in_res_elem->dx_resource,
			D3D12_RESOURCE_STATE_COPY_DEST,
			D3D12_RESOURCE_STATE_GENERIC_READ);

		//描述符建了也只有被集中进table时才有用
		create_descriptor(
			DIRECTX_RESOURCE_DESC_TYPE::DX_CBV,
			in_res_elem, memory_size);

		// Note: uploadBuffer has to be kept alive after the above function calls because
		// the command list has not been executed yet that performs the actual copy.
		// The caller can Release the uploadBuffer after it knows the copy has been executed.
	}
	return true;
}

//只有自定义能用
bool directx_render::allocate_upload_resource(
	std::shared_ptr<gpu_shader_resource> in_res,
	UINT in_elem_size,
	UINT in_number)
{
	std::shared_ptr < directx_frame_resource> frame_res = std::static_pointer_cast<directx_frame_resource>(in_res);

	for (int i = 0; i < FRAME_BUFFER_COUNT; i++)
	{

		auto in_res_elem = std::static_pointer_cast<directx_sr_custom_buffer>(frame_res->frame_resource_group[i]);
		UINT memory_size = in_elem_size * in_number;
		frame_res->element_size = in_elem_size;
		frame_res->element_count = in_number;
		CD3DX12_HEAP_PROPERTIES Heapproperties(D3D12_HEAP_TYPE_UPLOAD);
		CD3DX12_RESOURCE_DESC Resourcedesc = CD3DX12_RESOURCE_DESC::Buffer(memory_size);

		create_gpu_memory(
			in_res_elem->dx_resource,
			Heapproperties,
			Resourcedesc
		);

		//描述符建了也只有被集中进table时才有用
		//create_descriptor(
		//	DIRECTX_RESOURCE_DESC_TYPE::DX_CBV,
		//	in_res_elem, memory_size);

		ThrowIfFailed(in_res_elem->dx_resource->Map(0, nullptr, reinterpret_cast<void**>(&(in_res_elem->mapped_data))));

	}
	return true;
}


/// <summary>
/// 刷新自定义资源
/// 所有自定义资源被使用的都是同步的current_frame索引
/// </summary>
/// <param name="in_out_gpu_res">gpu资源指针</param>
/// <param name="in_data">数据块</param>
/// <param name="in_update_element_index">刷新元素起始位置</param>
/// <param name="int_update_element_count">刷新元素个数</param>
bool directx_render::update_gpu_resource(
	std::shared_ptr<gpu_shader_resource> in_out_gpu_res,
	const void* in_data, 
	UINT in_update_element_index,
	size_t int_update_element_count)
{
	//只允许自定义资源更新
	if (in_out_gpu_res->shader_resource_type != gpu_shader_resource::SHADER_RESOURCE_TYPE_CUSTOM_BUFFER
		&& in_out_gpu_res->shader_resource_type != gpu_shader_resource::SHADER_RESOURCE_TYPE_CUSTOM_BUFFER_GROUP
		&& in_out_gpu_res->shader_resource_type != gpu_shader_resource::SHADER_RESOURCE_TYPE_CUSTOM_BUFFER_FOLLOW_MESH)
	{
		stre_exception::exception_output_str_group.push_back("update_gpu_resource in_out_gpu_res->shader_resource_type is not suitable");
		return false;
	}
	std::shared_ptr<directx_frame_resource> frame_res = std::static_pointer_cast<directx_frame_resource>(in_out_gpu_res);

	//要将所有帧资源刷新
	//先刷新不是current_frame，最后刷新current_frame位置
	for (UINT i = 0; i < FRAME_BUFFER_COUNT; i++)
	{
		UINT update_index = (current_frame + i + 1) % FRAME_BUFFER_COUNT;
		auto update_frame_res = frame_res->frame_resource_group[update_index];

		//如果这个资源正在被GPU使用（大于栅栏值）
		//要等待signal()才能刷新
		//if (update_frame_res->fence != 0 && dx_fence->GetCompletedValue() < update_frame_res->fence)
		if (dx_fence->GetCompletedValue() < update_frame_res->fence)
		{
			HANDLE eventHandle = CreateEventEx(nullptr, NULL, false, EVENT_ALL_ACCESS);
			ThrowIfFailed(dx_fence->SetEventOnCompletion(update_frame_res->fence, eventHandle));
			WaitForSingleObject(eventHandle, INFINITE);
			CloseHandle(eventHandle);
		}

		update_frame_res->fence = current_fence;

		//刷新...
		auto gpu_res = std::static_pointer_cast<directx_sr_custom_buffer>(update_frame_res);

		memcpy(&(gpu_res->mapped_data[in_update_element_index]), in_data, in_out_gpu_res->element_size * int_update_element_count);

	}
	return true;
}



//注意 渲染目标 和 贴图 深度图等的匹配
//如果渲染目标 和 贴图 深度图不匹配,返回false
bool directx_render::package_textures(
	std::vector<std::shared_ptr<gpu_shader_resource>>& in_texture_group,
	std::shared_ptr<gpu_shader_resource> in_out_table)
{
	if (in_texture_group.empty() || !in_out_table.get())
	{
		//!!!出问题了
		stre_exception::exception_output_str_group.push_back("package_textures in_texture_group is empty or in_out_table is nullptr");
		return false;
	}
	

	auto texture_res = std::static_pointer_cast<directx_texture_resource>(in_out_table);
	
	switch (in_out_table->shader_resource_type)
	{
	case gpu_shader_resource::SHADER_RESOURCE_TYPE::SHADER_RESOURCE_TYPE_TEXTURE_GROUP:
	{
		auto gpu_table = std::static_pointer_cast<directx_sr_texture_group>(texture_res->resource);
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
			
			const directx_texture_resource* each_res = static_cast< const directx_texture_resource*>(in_texture_group[i].get());

			//贴图组不检查，允许所有类型贴图作为输入

			auto gpu_texture = static_cast<const directx_sr_texture*>(each_res->resource.get());

			load_descriptor_into_heap(
				DIRECTX_RESOURCE_DESC_TYPE::DX_SRV, // ???
				gpu_texture,
				handle
			);
			handle.Offset(1, cbv_srv_uav_descriptor_size);
		}

		texture_res->element_count = in_texture_group.size();
	}
	break;
	case gpu_shader_resource::SHADER_RESOURCE_TYPE::SHADER_RESOURCE_TYPE_RENDER_TARGET_GROUP:
	{
		auto gpu_table = std::static_pointer_cast<directx_sr_render_target_group>(texture_res->resource);
		if (gpu_table->rtv_heap)
		{
			gpu_table->rtv_heap->Release();
			//release()
		}

		create_descriptor_heap(
			gpu_table->srv_heap,
			DIRECTX_RESOURCE_DESC_TYPE::DX_SRV,
			(UINT)in_texture_group.size());

		create_descriptor_heap(
			gpu_table->rtv_heap,
			DIRECTX_RESOURCE_DESC_TYPE::DX_RTV,
			(UINT)in_texture_group.size());

		CD3DX12_CPU_DESCRIPTOR_HANDLE sr_handle(gpu_table->srv_heap->GetCPUDescriptorHandleForHeapStart());
		CD3DX12_CPU_DESCRIPTOR_HANDLE rt_handle(gpu_table->rtv_heap->GetCPUDescriptorHandleForHeapStart());

		gpu_table->rt_number = in_texture_group.size();
		for (int i = 0; i < in_texture_group.size(); i++)
		{
			auto each_res = std::static_pointer_cast<directx_texture_resource>(in_texture_group[i]);
			
			//组内必须全是渲染目标
			if (each_res->shader_resource_type != gpu_shader_resource::SHADER_RESOURCE_TYPE_RENDER_TARGET)
			{
				//!!!出问题了
				stre_exception::exception_output_str_group.push_back("packaged render taget group, textures most be render taget");
				return false;
			}

			gpu_table->rt_group.push_back(std::static_pointer_cast<directx_shader_resource>(each_res->resource));

			auto gpu_texture = static_cast<const directx_sr_render_target*>(each_res->resource.get());

			load_descriptor_into_heap(
				DIRECTX_RESOURCE_DESC_TYPE::DX_SRV,
				gpu_texture,
				sr_handle
			);

			load_descriptor_into_heap(
				DIRECTX_RESOURCE_DESC_TYPE::DX_RTV,
				gpu_texture,
				rt_handle
			);
			sr_handle.Offset(1, cbv_srv_uav_descriptor_size);
			rt_handle.Offset(1, rtv_descriptor_size);
		}

		texture_res->element_count = in_texture_group.size();
	}
	break;
	case gpu_shader_resource::SHADER_RESOURCE_TYPE::SHADER_RESOURCE_TYPE_RENDER_DEPTH_STENCIL_GROUP:
	{
		auto gpu_table = std::static_pointer_cast<directx_sr_depth_stencil_group>(texture_res->resource);
		if (gpu_table->dsv_heap)
		{
			gpu_table->dsv_heap->Release();
			//release()
		}
		create_descriptor_heap(
			gpu_table->srv_heap,
			DIRECTX_RESOURCE_DESC_TYPE::DX_SRV,
			(UINT)in_texture_group.size());

		create_descriptor_heap(
			gpu_table->dsv_heap,
			DIRECTX_RESOURCE_DESC_TYPE::DX_DSV,
			(UINT)in_texture_group.size());

		CD3DX12_CPU_DESCRIPTOR_HANDLE sr_handle(gpu_table->srv_heap->GetCPUDescriptorHandleForHeapStart());
		CD3DX12_CPU_DESCRIPTOR_HANDLE ds_handle(gpu_table->dsv_heap->GetCPUDescriptorHandleForHeapStart());

		//！！！！！！一般只有一张！！！！！！
		for (int i = 0; i < in_texture_group.size(); i++)
		{
			auto each_res = std::static_pointer_cast<directx_texture_resource>(in_texture_group[i]);
			gpu_table->ds_ptr = std::static_pointer_cast<directx_shader_resource>(each_res->resource);
			auto gpu_texture = static_cast<const directx_sr_depth_stencil*>(each_res->resource.get());

			//组内必须全是深度模板
			if (each_res->shader_resource_type != gpu_shader_resource::SHADER_RESOURCE_TYPE_RENDER_DEPTH_STENCIL)
			{
				//!!!出问题了
				stre_exception::exception_output_str_group.push_back("packaged depth stencil group, texture most be depth stencil");
				return false;
			}    

			load_descriptor_into_heap(
				DIRECTX_RESOURCE_DESC_TYPE::DX_SRV,
				gpu_texture,
				sr_handle
			);

			load_descriptor_into_heap(
				DIRECTX_RESOURCE_DESC_TYPE::DX_DSV,
				gpu_texture,
				ds_handle
			);

			sr_handle.Offset(1, cbv_srv_uav_descriptor_size);
			ds_handle.Offset(1, dsv_descriptor_size);
		}

		texture_res->element_count = in_texture_group.size();
	}
	break;
	}
	return true;
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
void directx_render::draw_pass(s_pass* in_pass)
{
	auto* pass = static_cast<const directx_pass*>(in_pass->gpu_pass_ptr);

	dx_command_list->SetGraphicsRootSignature(pass->rootsignature.Get());

	dx_command_list->SetPipelineState(pass->pso.Get());

	dx_command_list->RSSetViewports(1, &screen_view_port);
	dx_command_list->RSSetScissorRects(1, &scissor_rect);

	load_resource(in_pass->uid.name,in_pass->gpu_pass_resource_ptr);

	std::shared_ptr<directx_sr_render_target_group> render_targets_ptr = nullptr;
	std::shared_ptr < directx_sr_depth_stencil_group> depth_stencil_ptr = nullptr;

	set_render_target(in_pass->gpu_rt_texture_ptr, in_pass->is_output, render_targets_ptr, depth_stencil_ptr);

	for (auto gpu_mesh : in_pass->gpu_mesh)
	{
		//加载模型的贴图等总体数据，不加载SHADER_RESOURCE_TYPE_CUSTOM_BUFFER_GROUP_FOLLOW_MESH数据
		load_resource(in_pass->uid.name, gpu_mesh.second.gpu_mesh_resource_ptr);

		draw_call(in_pass->uid.name, &gpu_mesh.second);
	}

	if (render_targets_ptr)
	{

		//转换RT状态
		for (auto it : render_targets_ptr->rt_group)
		{
			switch_gpu_resource_state(it, D3D12_RESOURCE_STATE_GENERIC_READ);
		}
	}
	if (depth_stencil_ptr)
	{
		//转换DS状态
		switch_gpu_resource_state(depth_stencil_ptr->ds_ptr, D3D12_RESOURCE_STATE_GENERIC_READ);
	}
}

void directx_render::set_render_target(
	const std::map <std::string, std::shared_ptr < gpu_shader_resource>>& in_gpu_res_group,
	bool is_final_output,
	std::shared_ptr < directx_sr_render_target_group>& render_targets_ptr,
	std::shared_ptr < directx_sr_depth_stencil_group>& depth_stencil_ptr)
{
	for (auto gpu_res : in_gpu_res_group)
	{
		switch (gpu_res.second->shader_resource_type)
		{
			//RT
		case gpu_shader_resource::SHADER_RESOURCE_TYPE_RENDER_TARGET_GROUP:
		{
			//if (rtv_descs)
			//{
			//	//已经有渲染目标了！
			//}

			std::shared_ptr<directx_texture_resource> texture_res = std::static_pointer_cast<directx_texture_resource>(gpu_res.second);
			render_targets_ptr = std::static_pointer_cast<directx_sr_render_target_group>(texture_res->resource);

		}
		break;
		//Depth
		case gpu_shader_resource::SHADER_RESOURCE_TYPE_RENDER_DEPTH_STENCIL_GROUP:
		{

			std::shared_ptr<directx_texture_resource> texture_res = std::static_pointer_cast<directx_texture_resource>(gpu_res.second);
			depth_stencil_ptr = std::static_pointer_cast<directx_sr_depth_stencil_group>(texture_res->resource);

			break;
		}
		}
	}
	if (is_final_output)
	{
		render_targets_ptr = std::static_pointer_cast<directx_sr_render_target_group>(swap_chain_buffer_heap[current_back_buffer]);
	}

	D3D12_CPU_DESCRIPTOR_HANDLE rtv_descs;
	D3D12_CPU_DESCRIPTOR_HANDLE dsv_desc;
	UINT output_rt_size = 0;
	UINT output_ds_size = 0;
	if (render_targets_ptr.get() && render_targets_ptr->rtv_heap)
	{
		rtv_descs = render_targets_ptr->rtv_heap.Get()->GetCPUDescriptorHandleForHeapStart();
		output_rt_size = render_targets_ptr->rt_number;

		auto handle_c = CD3DX12_CPU_DESCRIPTOR_HANDLE(
			rtv_descs,
			0,
			rtv_descriptor_size);


		int i = 0;
		//转换RT状态
		for (auto it : render_targets_ptr->rt_group)
		{

			switch_gpu_resource_state(it, D3D12_RESOURCE_STATE_RENDER_TARGET);
			
			float clearValue[] = { 0.0f, 0.0f, 0.0f, 0.0f };
			dx_command_list->ClearRenderTargetView(handle_c, clearValue, 0, nullptr);

			
			handle_c.Offset(1, rtv_descriptor_size);
			i++;
		}
	}
	if (depth_stencil_ptr.get() && depth_stencil_ptr->dsv_heap)
	{
		output_ds_size = 1;
		dsv_desc = depth_stencil_ptr->dsv_heap.Get()->GetCPUDescriptorHandleForHeapStart();
		//转换DS状态
		switch_gpu_resource_state(depth_stencil_ptr->ds_ptr, D3D12_RESOURCE_STATE_DEPTH_WRITE);

		dx_command_list->ClearDepthStencilView(dsv_desc,
			D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL,
			1.0f,
			0,
			0,
			nullptr);
	}

	if (output_rt_size > 0)
	{

		if (depth_stencil_ptr.get() && depth_stencil_ptr->dsv_heap)
		{
			dx_command_list->OMSetRenderTargets(output_rt_size,
				&rtv_descs,
				true,
				&dsv_desc);
		}
		else
		{
			dx_command_list->OMSetRenderTargets(0,
				&rtv_descs,
				true,
				nullptr);
		}
	}
	else if(output_ds_size > 0)
	{
		if (depth_stencil_ptr.get() && depth_stencil_ptr->dsv_heap)
		dx_command_list->OMSetRenderTargets(0,
			nullptr,
			true,
			&dsv_desc);
		else
		{
			dx_command_list->OMSetRenderTargets(0,
				nullptr,
				true,
				nullptr);
		}
	}
}

//读取当前帧索引的帧资源
void directx_render::load_resource(
	std::string in_pass_uid,
	const std::map < std::string, const gpu_shader_resource*>&  in_gpu_res_group)
{
	D3D12_CPU_DESCRIPTOR_HANDLE rtv_descs;
	D3D12_CPU_DESCRIPTOR_HANDLE dsv_desc;
	UINT output_rt_size = 0;
	for (auto gpu_res : in_gpu_res_group)
	{
		//检查资源有没有被装载到当前pass中
		auto register_index_it = gpu_res.second->register_index_map.find(in_pass_uid);
		if (register_index_it == gpu_res.second->register_index_map.end())
		{
			continue;
		}
		int register_index = register_index_it->second;

		switch (gpu_res.second->shader_resource_type)
		{
			//结构体
		case gpu_shader_resource::SHADER_RESOURCE_TYPE_CUSTOM_BUFFER:
		{
			const directx_frame_resource* frame_res = static_cast<const directx_frame_resource*>(gpu_res.second);
			auto gpu_sr = static_cast<const directx_sr_custom_buffer*>(get_current_frame_resource(frame_res));
			
				//dx_command_list->
				//	SetGraphicsRootConstantBufferView(
				//		gpu_res.second->register_index,
				//		gpu_sr->dx_resource.Get()->GetGPUVirtualAddress());
			//!!! 没有用CBV
			dx_command_list->
				SetGraphicsRootShaderResourceView(
					register_index,
					gpu_sr->dx_resource.Get()->GetGPUVirtualAddress());

		
		}
		break;

		//结构体组 只有结构体组是SRV
		case gpu_shader_resource::SHADER_RESOURCE_TYPE_CUSTOM_BUFFER_GROUP:
		{
			const directx_frame_resource* frame_res = static_cast<const directx_frame_resource*>(gpu_res.second);
			auto gpu_sr = static_cast<const directx_sr_custom_buffer_group*>(get_current_frame_resource(frame_res));

				dx_command_list->
					SetGraphicsRootShaderResourceView(
						register_index,
						gpu_sr->dx_resource.Get()->GetGPUVirtualAddress());

		}
		break;

		////贴图不是SRV
		//case gpu_shader_resource::SHADER_RESOURCE_TYPE_TEXTURE:
		//{
		//	const directx_texture_resource* texture_res = static_cast<const directx_texture_resource*>(gpu_res.second);
		//	auto gpu_sr = static_cast<const directx_sr_texture*>(texture_res->resource.get());
		//	dx_command_list->
		//		SetGraphicsRootShaderResourceView(
		//			gpu_res.second->register_index,
		//			gpu_sr->dx_resource.Get()->GetGPUVirtualAddress());
		//}
		//break;
		//贴图堆
		case gpu_shader_resource::SHADER_RESOURCE_TYPE_TEXTURE_GROUP:
		{
			const directx_texture_resource* texture_res = static_cast<const directx_texture_resource*>(gpu_res.second);
			auto gpu_sr = static_cast<const directx_sr_texture_group*>(texture_res->resource.get());
			ID3D12DescriptorHeap* descriptorHeaps[] = { gpu_sr->srv_heap.Get() };
			dx_command_list->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);
			dx_command_list->SetGraphicsRootDescriptorTable(register_index, gpu_sr->srv_heap->GetGPUDescriptorHandleForHeapStart());
		}
		break;
		case gpu_shader_resource::SHADER_RESOURCE_TYPE_RENDER_TARGET_GROUP:
		{
			const directx_texture_resource* rt_res = static_cast<const directx_texture_resource*>(gpu_res.second);
			auto gpu_sr = static_cast<const directx_sr_render_target_group*>(rt_res->resource.get());
			ID3D12DescriptorHeap* descriptorHeaps[] = { gpu_sr->srv_heap.Get() };
			dx_command_list->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);
			dx_command_list->SetGraphicsRootDescriptorTable(register_index, gpu_sr->srv_heap->GetGPUDescriptorHandleForHeapStart());
		}
		break;
		case gpu_shader_resource::SHADER_RESOURCE_TYPE_RENDER_DEPTH_STENCIL_GROUP:
		{
			const directx_texture_resource* ds_res = static_cast<const directx_texture_resource*>(gpu_res.second);
			auto gpu_sr = static_cast<const directx_sr_depth_stencil_group*>(ds_res->resource.get());
			ID3D12DescriptorHeap* descriptorHeaps[] = { gpu_sr->srv_heap.Get() };
			dx_command_list->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);
			dx_command_list->SetGraphicsRootDescriptorTable(register_index, gpu_sr->srv_heap->GetGPUDescriptorHandleForHeapStart());
		}
		break;
		}
	}

}

void directx_render::draw_call(
	std::string in_pass_uid,
	const s_pass::gpu_mesh_resource* in_gpu_mesh)
{
	auto* vertex_buffer = static_cast<const directx_sr_custom_buffer*>(get_current_frame_resource(in_gpu_mesh->vertex));
	auto* index_buffer = static_cast<const directx_sr_custom_buffer*>(get_current_frame_resource(in_gpu_mesh->index));

	D3D12_VERTEX_BUFFER_VIEW Vertexbufferview;
	Vertexbufferview.BufferLocation = vertex_buffer->dx_resource->GetGPUVirtualAddress();
	Vertexbufferview.StrideInBytes = in_gpu_mesh->vertex->element_size;
	Vertexbufferview.SizeInBytes = in_gpu_mesh->vertex->element_count * in_gpu_mesh->vertex->element_size;

	D3D12_INDEX_BUFFER_VIEW Indexbufferview;
	Indexbufferview.BufferLocation = index_buffer->dx_resource->GetGPUVirtualAddress();
	Indexbufferview.SizeInBytes = in_gpu_mesh->index->element_count * in_gpu_mesh->index->element_size;
	switch (in_gpu_mesh->index->element_size)
	{
	case 1:
		Indexbufferview.Format = DXGI_FORMAT_R8_UINT;
		break;
	case 2:
		Indexbufferview.Format = DXGI_FORMAT_R16_UINT;
		break;
	case 4:
		Indexbufferview.Format = DXGI_FORMAT_R32_UINT;
		break;
	}
	//???只支持三角形
	dx_command_list->IASetVertexBuffers(0, 1, &Vertexbufferview);
	dx_command_list->IASetIndexBuffer(&Indexbufferview);
	dx_command_list->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


	//找随子mesh索引变换的CB资源
	std::vector<const directx_frame_resource*> refresh_group;

	for (auto it : in_gpu_mesh->gpu_mesh_resource_ptr)
	{
		if(it.second->shader_resource_type == gpu_shader_resource::SHADER_RESOURCE_TYPE_CUSTOM_BUFFER_FOLLOW_MESH)
		{
			refresh_group.push_back(static_cast<const directx_frame_resource*>(it.second));
		}
	}

	for (UINT i = 0; i < in_gpu_mesh->mesh_element_group.size(); i++)
	{
		for (auto it : refresh_group)
		{
			//检查资源有没有被装载到当前pass中
			auto register_index_it = it->register_index_map.find(in_pass_uid);
			if (register_index_it == it->register_index_map.end())
			{
				continue;
			}
			int register_index = register_index_it->second;

			const directx_sr_custom_buffer_group* dx_sr_cb_group = static_cast<const directx_sr_custom_buffer_group*>(get_current_frame_resource(it));
			D3D12_GPU_VIRTUAL_ADDRESS objCBAddress = dx_sr_cb_group->dx_resource->GetGPUVirtualAddress() + i * it->element_size;
			//!!! 没有用CBV
			//dx_command_list->SetGraphicsRootConstantBufferView(it->register_index, objCBAddress);
			dx_command_list->SetGraphicsRootShaderResourceView(register_index, objCBAddress);

		}

		dx_command_list->DrawIndexedInstanced(
			in_gpu_mesh->mesh_element_group[i].index_number,
			1,
			in_gpu_mesh->mesh_element_group[i].index_start_offset,
			/*BaseVertexLocation*/ 0,
			0);
	}
}

/// <summary>
/// 命令队列执行
/// </summary>
void directx_render::excute_command_list()
{
	//将交换链资源置为present状态
	auto swap_ptr = std::static_pointer_cast<directx_sr_render_target_group>(swap_chain_buffer_heap[current_back_buffer]);
	switch_gpu_resource_state(swap_ptr->rt_group[0], D3D12_RESOURCE_STATE_PRESENT);

	// Done recording commands.
	ThrowIfFailed(dx_command_list->Close());

	// Add the command list to the queue for execution.
	ID3D12CommandList* cmdsLists[] = { dx_command_list.Get() };
	dx_command_queue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

}

void directx_render::reset_command_allocator()
{
	ThrowIfFailed(dx_frame_cmdlist_alloc[current_frame]->Reset());
	ThrowIfFailed(dx_command_list->Reset(dx_frame_cmdlist_alloc[current_frame].Get(), nullptr));
}

/// <summary>
/// 刷新程序的栅栏
/// </summary>
void directx_render::switch_swap_chain()
{
	// Swap the back and front buffers
	ThrowIfFailed(dx_swap_chain->Present(0, 0));
	current_back_buffer = (current_back_buffer + 1) % SWAP_CHAIN_BUFFER_COUNT;

	//开始下一帧，指向所有内存的下一帧资源
	current_frame = (current_frame + 1) % FRAME_BUFFER_COUNT;
	//栅栏值++
	current_fence++;

	//设置栅栏值
	dx_command_queue->Signal(dx_fence.Get(), current_fence);
	//通知可以刷新用完的上一帧的内存了
}

/// <summary>
/// 等待队列执行完毕，清空队列
/// </summary>
void directx_render::flush_command_queue()
{
	// Advance the fence value to mark commands up to this fence point.
	current_fence++;

	// Add an instruction to the command queue to set a new fence point.  Because we 
	// are on the GPU timeline, the new fence point won't be set until the GPU finishes
	// processing all the commands prior to this Signal().
	ThrowIfFailed(dx_command_queue->Signal(dx_fence.Get(), current_fence));

	// Wait until the GPU has completed commands up to this fence point.
	if (dx_fence->GetCompletedValue() < current_fence)
	{
		HANDLE eventHandle = CreateEventEx(nullptr, NULL, false, EVENT_ALL_ACCESS);

		// Fire event when GPU hits current fence.  
		ThrowIfFailed(dx_fence->SetEventOnCompletion(current_fence, eventHandle));

		// Wait until the GPU hits current fence event is fired.
		WaitForSingleObject(eventHandle, INFINITE);
		CloseHandle(eventHandle);
	}
}
/***
************************************************************
*
* Base Function
*
************************************************************
*/
//获取当前内存，并刷新栅栏值表示正在被使用
const directx_render::directx_resource* directx_render::get_current_frame_resource(const gpu_shader_resource* in_gpu_sr)
{
	const directx_frame_resource* frame_res = static_cast<const directx_render::directx_frame_resource*>(in_gpu_sr);
	frame_res->frame_resource_group[current_frame]->fence = current_fence;
	return frame_res->frame_resource_group[current_frame].get();
}

//获取当前内存，并刷新栅栏值表示正在被使用
const directx_render::directx_resource* directx_render::get_current_frame_resource(const directx_frame_resource* in_gpu_sr)
{
	in_gpu_sr->frame_resource_group[current_frame]->fence = current_fence;
	return in_gpu_sr->frame_resource_group[current_frame].get();
}

void directx_render::create_gpu_texture(
	std::shared_ptr<directx_resource> in_out_gpu_texture, 
	gpu_shader_resource::SHADER_RESOURCE_TYPE in_shader_res_type)
{
	CD3DX12_RESOURCE_DESC resourceDesc;
	ZeroMemory(&resourceDesc, sizeof(resourceDesc));
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	resourceDesc.Alignment = 0;
	resourceDesc.SampleDesc.Count = MSAAX4_STATE ? 4 : 1;
	resourceDesc.SampleDesc.Quality = MSAAX4_STATE ? (MSAAX4_QUALITY - 1) : 0;
	resourceDesc.MipLevels = 1;
	resourceDesc.DepthOrArraySize = 1;
	resourceDesc.Width = client_width;
	resourceDesc.Height = client_height;
	resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;


	if (in_shader_res_type == gpu_shader_resource::SHADER_RESOURCE_TYPE_RENDER_TARGET)
	{
		auto gpu_texture = std::static_pointer_cast<directx_sr_render_target>(in_out_gpu_texture);

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
	else if (in_shader_res_type == gpu_shader_resource::SHADER_RESOURCE_TYPE_RENDER_DEPTH_STENCIL)
	{
		auto gpu_texture = std::static_pointer_cast<directx_sr_depth_stencil>(in_out_gpu_texture);

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


void directx_render::create_descriptor_heap(
	ComPtr<ID3D12DescriptorHeap>& in_out_descheap,
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


	ThrowIfFailed(dx_device->CreateDescriptorHeap(
		&HeapDesc,
		IID_PPV_ARGS(in_out_descheap.GetAddressOf())));
}

//vertex 
//index 
//texture
//默认构建 read的缓冲区
void directx_render::create_gpu_memory(
	ComPtr<ID3D12Resource>& in_out_resource,
	CD3DX12_HEAP_PROPERTIES in_heap_properties,
	CD3DX12_RESOURCE_DESC in_rescource_desc,
	D3D12_RESOURCE_STATES in_resource_states,
	D3D12_CLEAR_VALUE* clearValue)
{
	ThrowIfFailed(dx_device->CreateCommittedResource(
		&in_heap_properties,
		D3D12_HEAP_FLAG_NONE,
		&in_rescource_desc,
		in_resource_states,
		clearValue,
		IID_PPV_ARGS(&in_out_resource)));
}

void directx_render::switch_gpu_resource_state(
	std::shared_ptr<directx_shader_resource> in_gpu_res_elem,
	D3D12_RESOURCE_STATES in_new_resource_states)
{
	if(!in_gpu_res_elem.get())
	{
		return;
	}
	if (in_gpu_res_elem->dx_current_state != in_new_resource_states)
	{
		switch_gpu_memory_state(in_gpu_res_elem->dx_resource,
			in_gpu_res_elem->dx_current_state,
			in_new_resource_states);
		in_gpu_res_elem->dx_current_state = in_new_resource_states;
	}
}

void directx_render::switch_gpu_memory_state(
	ComPtr<ID3D12Resource>& in_out_resource,
	D3D12_RESOURCE_STATES in_old_resource_states,
	D3D12_RESOURCE_STATES in_new_resource_states)
{
	auto resource_barrier = CD3DX12_RESOURCE_BARRIER::Transition(
		in_out_resource.Get(),
		in_old_resource_states,
		in_new_resource_states);

	dx_command_list->ResourceBarrier(1, &resource_barrier);
}

//DSV UAV undo!!!
///only for table
// low level of expose

/// <summary>
/// 
/// </summary>
/// <param name="in_texture_desc_type"></param>
/// <param name="in_gpu_res_elem"></param>
/// <param name="in_memory_size">只在CSV有效</param>
void directx_render::create_descriptor(
	DIRECTX_RESOURCE_DESC_TYPE in_texture_desc_type,
	std::shared_ptr<directx_shader_resource> in_gpu_res_elem,
	UINT in_memory_size)
{

	switch (in_texture_desc_type)
	{
		case DIRECTX_RESOURCE_DESC_TYPE::DX_CBV:
		{
			D3D12_CONSTANT_BUFFER_VIEW_DESC desc;
			ZeroMemory(&desc, sizeof(desc));
			desc.BufferLocation = in_gpu_res_elem->dx_resource->GetGPUVirtualAddress();
			
			desc.SizeInBytes = in_memory_size;
			
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
			if (in_gpu_res_elem->dx_format == depth_stencil_format)
			{
				desc.Format = depth_stencil_as_shader_resource_format;
			}
			
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
		dx_device->CreateDepthStencilView(
			in_gpu_res_elem->dx_resource.Get(),
			&in_gpu_res_elem->dx_dsv,
			in_out_dest_descriptor);
		break;
	case DIRECTX_RESOURCE_DESC_TYPE::DX_RTV:
		dx_device->CreateRenderTargetView(
			in_gpu_res_elem->dx_resource.Get(),
			&in_gpu_res_elem->dx_rtv,
			in_out_dest_descriptor);
		break;
	case DIRECTX_RESOURCE_DESC_TYPE::DX_SRV:
		dx_device->CreateShaderResourceView(
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
	CD3DX12_ROOT_SIGNATURE_DESC& in_rootsig_desc,
	ComPtr<ID3D12RootSignature>& in_out_rootsignature)
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

	ThrowIfFailed(dx_device->CreateRootSignature(
		0,
		serializedRootSig->GetBufferPointer(),
		serializedRootSig->GetBufferSize(),
		IID_PPV_ARGS(in_out_rootsignature.GetAddressOf())));
}

ID3DBlob* directx_render::complie_shader(
	const std::wstring& file_name,
	const D3D_SHADER_MACRO* defines,
	const std::string& entry_point,
	const std::string& target,
	bool & out_successed)
{
	UINT compileFlags = 0;

	compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;


	HRESULT hr = S_OK;

	ID3DBlob* byteCode = nullptr;
	ID3DBlob* errors = nullptr;
	hr = D3DCompileFromFile(file_name.c_str(), defines, D3D_COMPILE_STANDARD_FILE_INCLUDE,
		entry_point.c_str(), target.c_str(), compileFlags, 0, &byteCode, &errors);

	if (errors)
	{
		OutputDebugStringA((char*)errors->GetBufferPointer());
		out_successed = false;
		stre_exception::exception_output_str_group.push_back((char*)errors->GetBufferPointer());
	}
	//不再动态报错，而是输出到消息列表
	//ThrowIfFailed(hr);

	return byteCode;
}

void directx_render::reflect_shader(
	ComPtr<ID3DBlob>& in_shader_data, 
	std::map<std::string,gpu_pass::pass_resource>& out_res_group)
{

	typedef gpu_shader_resource::SHADER_RESOURCE_TYPE SHADER_RES_TYPE;
	
	if(!in_shader_data)
	{
		return;
	}
	
	//反射生成对应接口
	ComPtr<ID3D12ShaderReflection> shader_reflector;
	ThrowIfFailed(
		D3DReflect(
			in_shader_data->GetBufferPointer(), 
			in_shader_data->GetBufferSize(), 
			IID_ID3D12ShaderReflection, 
			(void**)shader_reflector.GetAddressOf()));

	D3D12_SHADER_DESC shader_desc;
	ThrowIfFailed(shader_reflector->GetDesc(&shader_desc));

	for (int i = 0; i < shader_desc.BoundResources; i++)
	{
		D3D12_SHADER_INPUT_BIND_DESC  resource_desc;
		ThrowIfFailed(
			shader_reflector->GetResourceBindingDesc(i, &resource_desc));

		gpu_pass::pass_resource pass_res;

		pass_res.name = resource_desc.Name;
		pass_res.register_space = resource_desc.Space;
		auto resourceType = resource_desc.Type;
		pass_res.bind_point = resource_desc.BindPoint;
		pass_res.bind_count = resource_desc.BindCount;

		switch (resource_desc.Type)
		{
		case D3D_SIT_CBUFFER:
		case D3D_SIT_TBUFFER:
			pass_res.type = SHADER_RES_TYPE::SHADER_RESOURCE_TYPE_CUSTOM_BUFFER;
			out_res_group[pass_res.name] = pass_res;
			break;
		case D3D_SIT_STRUCTURED:
			pass_res.type = SHADER_RES_TYPE::SHADER_RESOURCE_TYPE_CUSTOM_BUFFER_GROUP;
			out_res_group[pass_res.name] = pass_res;
			break;
		case D3D_SIT_TEXTURE:
			pass_res.type = SHADER_RES_TYPE::SHADER_RESOURCE_TYPE_TEXTURE_GROUP;
			out_res_group[pass_res.name] = pass_res;
			break;
		case D3D_SIT_SAMPLER:
			//???
			//暂时全部采样都用上
			pass_res.type = SHADER_RES_TYPE::SHADER_RESOURCE_TYPE_SAMPLER;
			break;
		case D3D_SIT_UAV_RWTYPED:
		case D3D_SIT_UAV_RWSTRUCTURED:
			pass_res.type = SHADER_RES_TYPE::SHADER_RESOURCE_TYPE_UNORDERED_BUFFER;
			out_res_group[pass_res.name] = pass_res;
			break;
		default:
			pass_res.type = SHADER_RES_TYPE::SHADER_RESOURCE_TYPE_NONE;
			break;
		}
		
	}
}

void directx_render::create_pso(
	D3D12_GRAPHICS_PIPELINE_STATE_DESC &in_pso_desc,
	ComPtr<ID3D12PipelineState>& in_pso)
{
	ThrowIfFailed(dx_device->CreateGraphicsPipelineState(
		&in_pso_desc,
		IID_PPV_ARGS(&in_pso)));

}


void directx_render::init(HWND in_main_wnd, UINT in_width, UINT in_height)
{
	client_width = in_width;
	client_height = in_height;

	//启动调试层
	//启动了之后才会在出问题的时候输出错误提示
	ID3D12Debug* debug = nullptr;
	D3D12GetDebugInterface(IID_PPV_ARGS(&debug));
	if (debug)
	{
		debug->EnableDebugLayer();
		debug->Release();
		debug = nullptr;
	}

	//factory & adapter & device & fance
	{
		ThrowIfFailed(CreateDXGIFactory1(IID_PPV_ARGS(&dx_factory)));

		// Try to create hardware device.
		HRESULT hard_ware_result = D3D12CreateDevice(
			nullptr,             // default adapter
			D3D_FEATURE_LEVEL_11_0,
			IID_PPV_ARGS(&dx_device));


		// Fallback to WARP device.
		if (FAILED(hard_ware_result))
		{
			Microsoft::WRL::ComPtr<IDXGIAdapter> warp_adapter;
			ThrowIfFailed(dx_factory->EnumWarpAdapter(IID_PPV_ARGS(&warp_adapter)));

			ThrowIfFailed(D3D12CreateDevice(
				warp_adapter.Get(),
				D3D_FEATURE_LEVEL_11_0,
				IID_PPV_ARGS(&dx_device)));
		}

		ThrowIfFailed(dx_device->CreateFence(0, D3D12_FENCE_FLAG_NONE,
			IID_PPV_ARGS(&dx_fence)));
	}

	msaa_configuration();

	//commandqueue & allocator & commandlist
	{
		D3D12_COMMAND_QUEUE_DESC queueDesc = {};
		queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		ThrowIfFailed(dx_device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&dx_command_queue)));

		ThrowIfFailed(dx_device->CreateCommandAllocator(
			D3D12_COMMAND_LIST_TYPE_DIRECT,
			IID_PPV_ARGS(dx_cmdlist_alloc.GetAddressOf())));

		for (int i = 0; i < FRAME_BUFFER_COUNT; i++)
		{
			ThrowIfFailed(dx_device->CreateCommandAllocator(
				D3D12_COMMAND_LIST_TYPE_DIRECT,
				IID_PPV_ARGS(dx_frame_cmdlist_alloc[i].GetAddressOf())));
		}

		ThrowIfFailed(dx_device->CreateCommandList(
			0,
			D3D12_COMMAND_LIST_TYPE_DIRECT,
			dx_cmdlist_alloc.Get(), // Associated command allocator
			nullptr,                   // Initial PipelineStateObject
			IID_PPV_ARGS(dx_command_list.GetAddressOf())));

		// Start off in a closed state.  This is because the first time we refer 
		// to the command list we will Reset it, and it needs to be closed before
		// calling Reset.
		dx_command_list->Close();
	}

	//swapchain
	{
		dx_swap_chain.Reset();

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
		ThrowIfFailed(dx_factory->CreateSwapChain(
			dx_command_queue.Get(),
			&sd,
			dx_swap_chain.GetAddressOf()));
	}

	
	flush_command_queue();
	ThrowIfFailed(dx_command_list->Reset(dx_cmdlist_alloc.Get(), nullptr));


	ScreenViewportResize(in_width, in_height);

	//descriptor size
	{
		rtv_descriptor_size = dx_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		dsv_descriptor_size = dx_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
		cbv_srv_uav_descriptor_size = dx_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	}

	//构建back buffer
	{
		for (int i = 0; i < SWAP_CHAIN_BUFFER_COUNT; i++)
		{
			swap_chain_buffer[i] = std::make_shared<directx_sr_render_target>();
			swap_chain_buffer_heap[i] = std::make_shared<directx_sr_render_target_group>();

			create_descriptor_heap(
				swap_chain_buffer_heap[i]->rtv_heap,
				DIRECTX_RESOURCE_DESC_TYPE::DX_RTV,
				(UINT)1);
			CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHeapHandle(swap_chain_buffer_heap[i]->rtv_heap->GetCPUDescriptorHandleForHeapStart());
			
			ThrowIfFailed(dx_swap_chain->GetBuffer(i, IID_PPV_ARGS(&swap_chain_buffer[i]->dx_resource)));
			dx_device->CreateRenderTargetView(swap_chain_buffer[i]->dx_resource.Get(), nullptr, rtvHeapHandle);
			swap_chain_buffer_heap[i]->rt_group.push_back(swap_chain_buffer[i]);
			swap_chain_buffer_heap[i]->rt_number = 1;
		}
	}

	//RESIZE 确保交换缓存和深度图构建完成
	excute_command_list();
	flush_command_queue();

	reset_command_allocator();
}


void directx_render::msaa_configuration()
{
	D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS msQualityLevels;
	msQualityLevels.Format = back_buffer_format;
	msQualityLevels.SampleCount = 4;
	msQualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
	msQualityLevels.NumQualityLevels = 0;
	ThrowIfFailed(dx_device->CheckFeatureSupport(
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
