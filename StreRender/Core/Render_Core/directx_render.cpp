#include "directx_render.h"
#include <WindowsX.h>
#include "Core/Exception/exception.h"



//
// 所有资源都已经装好在GPU后在这里被使用
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

//
void directx_render::create_descriptor_heap(
	D3D12_DESCRIPTOR_HEAP_DESC & in_dx_descheap_desc, 
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> in_out_descheap)
{
	ThrowIfFailed(d3d_device->CreateDescriptorHeap(
		&in_dx_descheap_desc,
		IID_PPV_ARGS(in_out_descheap.GetAddressOf())));
}

void directx_render::create_gpu_memory()
{

}

//CSV UAV undo!!!
void directx_render::create_gpu_memory_view(
	DIRECTX_RESOURCE_DESC_TYPE in_texture_desc_type,
	gpu_resource* in_gpu_resource,
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
	CD3DX12_ROOT_PARAMETER & in_slot_root_parameter,
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
	assert(render_configuration::msaax4_quality > 0 && "Unexpected MSAA quality level.");
}