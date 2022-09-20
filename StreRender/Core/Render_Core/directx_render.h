#pragma once

#include <windows.h>
#include <wrl.h>
#include <dxgi1_4.h>
#include <d3d12.h>
#include <D3Dcompiler.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <DirectXColors.h>
#include <DirectXCollision.h>
#include <string>
#include <memory>
#include <algorithm>
#include <vector>
#include <array>
#include <unordered_map>
#include <cstdint>
#include <fstream>
#include <sstream>
#include <cassert>
#include "d3dx12.h"
#include "stre_render.h"
#include "render.h"
#include "Core/Window/s_window.h"
#include "Resource/GPU_Resource/gpu_resource.h"
#define SWAP_CHAIN_BUFFER_COUNT 2

using Microsoft::WRL::ComPtr;

enum DIRECTX_RESOURCE_DESC_TYPE
{
    DX_SRV,
    DX_RTV,
    DX_DSV,
    DX_CBV,
    DX_UAV
};

enum DIRECTX_RESOURCE_TYPE
{

};

class directx_render : public render 
{
protected:
    Microsoft::WRL::ComPtr<IDXGIFactory4> dxgi_factory;
    Microsoft::WRL::ComPtr<IDXGISwapChain> swap_chain;
    Microsoft::WRL::ComPtr<ID3D12Device> d3d_device;

    Microsoft::WRL::ComPtr<ID3D12Fence> fence;

    Microsoft::WRL::ComPtr<ID3D12CommandQueue> command_queue;
    Microsoft::WRL::ComPtr<ID3D12CommandAllocator> direct_cmdlist_alloc;
    Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> command_list;

private:
    DXGI_FORMAT back_buffer_format = DXGI_FORMAT_R8G8B8A8_UNORM;//???


private:
    void msaa_configuration();//???

public:

    void create_descriptor_heap(
        D3D12_DESCRIPTOR_HEAP_DESC& in_dx_descheap_desc,
        Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> in_out_descheap);
    void create_gpu_memory();

    void create_gpu_memory_view(
        DIRECTX_RESOURCE_DESC_TYPE in_texture_desc_type,
        gpu_resource* in_gpu_resource,
        D3D12_CPU_DESCRIPTOR_HANDLE in_out_dest_descriptor);

    void create_rootsignature(
        CD3DX12_ROOT_PARAMETER& in_slot_root_parameter,
        CD3DX12_ROOT_SIGNATURE_DESC& in_rootsig_desc,
        ComPtr<ID3D12RootSignature> in_out_rootsignature);

    ID3DBlob* complie_shader(
        const std::wstring& filename,
        const D3D_SHADER_MACRO* defines,
        const std::string& entrypoint,
        const std::string& target);

    void create_pso(
        D3D12_GRAPHICS_PIPELINE_STATE_DESC& in_pso_desc,
        ComPtr<ID3D12PipelineState> in_pso);
    

public:

    virtual void draw_call() override;



    virtual void init(HWND in_main_wnd) override;

    

    virtual void over() override;

};