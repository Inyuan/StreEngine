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
#include "Render_API/d3dx12.h"
#include "stre_render.h"
#include "render.h"
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
    //??? 有必要暴露在外面吗
    void create_descriptor_heap(
        D3D12_DESCRIPTOR_HEAP_DESC& in_dx_descheap_desc,
        Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> in_out_descheap);
    
    void create_gpu_memory(
        ComPtr<ID3D12Resource> in_out_resource,
        CD3DX12_HEAP_PROPERTIES in_heap_properties,
        CD3DX12_RESOURCE_DESC in_rescource_desc,
        D3D12_RESOURCE_STATES in_resource_states = D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_COMMON);

    void switch_gpu_memory_state(
        ComPtr<ID3D12Resource> in_out_resource,
        D3D12_RESOURCE_STATES in_old_resource_states,
        D3D12_RESOURCE_STATES in_new_resource_states);

    void create_gpu_memory_view(
        DIRECTX_RESOURCE_DESC_TYPE in_texture_desc_type,
        directx_gpu_resource_element* in_gpu_resource,
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

    virtual gpu_resource_element* allocate_gpu_memory(GPU_RESOURCE_LAYOUT in_resource_layout) override;

    //virtual gpu_resource* update_gpu_memory(cg_resource* in_resource) override;

    virtual gpu_resource* create_gpu_texture(std::string in_gpu_texture_name) override;

public:

    virtual void draw_call() override;

    virtual void allocate_pass() override;

    virtual void init(HWND in_main_wnd) override;

    virtual void over() override;

};

class directx_gpu_resource_element : public gpu_resource_element
{
public:
    ComPtr<ID3D12Resource> dx_resource;

    D3D12_SHADER_RESOURCE_VIEW_DESC dx_srv;
    D3D12_RENDER_TARGET_VIEW_DESC dx_rtv;
    D3D12_DEPTH_STENCIL_VIEW_DESC dx_dsv;
    D3D12_CONSTANT_BUFFER_VIEW_DESC dx_csv;
    D3D12_UNORDERED_ACCESS_VIEW_DESC dx_uav;

    DXGI_FORMAT dx_format;
};


template<typename T>
gpu_resource* allocate_upload_resource(directx_render* in_render, cg_resource* in_resource)
{
    UINT Elementbytesize = sizeof(T);
    UINT Memorysize = Elementbytesize * IelementCount;

    if (IsConstantbuffer)
        Elementbytesize = MathHelper::CalcConstantBufferByteSize(sizeof(T));

    CD3DX12_HEAP_PROPERTIES Heapproperties(D3D12_HEAP_TYPE_UPLOAD);
    CD3DX12_RESOURCE_DESC Resourcebuffer = CD3DX12_RESOURCE_DESC::Buffer(Elementbytesize * IelementCount);

    ThrowIfFailed(Idevice->CreateCommittedResource(
        &Heapproperties,
        D3D12_HEAP_FLAG_NONE,
        &Resourcebuffer,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&Uploadresource)));

    ThrowIfFailed(Uploadresource->Map(0, nullptr, reinterpret_cast<void**>(&Mappeddata)));

}