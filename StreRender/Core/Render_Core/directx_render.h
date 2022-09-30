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
    //屏幕空间的顶点输入
    ComPtr<ID3D12Resource> screen_vertex_gpu_resource;
    D3D12_VERTEX_BUFFER_VIEW screen_vertex_view;

    D3D12_VIEWPORT screen_view_port;
    D3D12_RECT scissor_rect;

    UINT rtv_descriptor_size = 0;

    UINT dsv_descriptor_size = 0;

    UINT cbv_srv_uav_descriptor_size = 0;

    //??? 精度控制
    DXGI_FORMAT back_buffer_format = DXGI_FORMAT_R8G8B8A8_UNORM;
    
    DXGI_FORMAT depth_stencil_format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    
    D3D12_RESOURCE_STATES default_resource_states = D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_GENERIC_READ;

    float clear_color[4] = { 0.0,0.0f,0.0f,0.0f };

private:
    void msaa_configuration();//???

    void ScreenViewportResize(int in_width, int in_height);
public:

    //??? 有必要暴露在外面吗
    void create_descriptor_heap(
        D3D12_DESCRIPTOR_HEAP_DESC& in_dx_descheap_desc,
        Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> in_out_descheap);
    
    void create_gpu_memory(
        ComPtr<ID3D12Resource> in_out_resource,
        CD3DX12_HEAP_PROPERTIES in_heap_properties,
        CD3DX12_RESOURCE_DESC in_rescource_desc,
        D3D12_RESOURCE_STATES in_resource_states = D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_GENERIC_READ,
        D3D12_CLEAR_VALUE* clearValue = nullptr);


    void switch_gpu_memory_state(
        ComPtr<ID3D12Resource> in_out_resource,
        D3D12_RESOURCE_STATES in_old_resource_states,
        D3D12_RESOURCE_STATES in_new_resource_states);

    void create_gpu_memory_view(
        DIRECTX_RESOURCE_DESC_TYPE in_texture_desc_type,
        directx_gpu_resource_element* in_gpu_res_elem,
        D3D12_CPU_DESCRIPTOR_HANDLE in_out_dest_descriptor);

    void create_rootsignature(
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
    
    void allocate_upload_resource(
        directx_gpu_resource_element* in_res_elem,
        UINT in_elem_size,
        UINT in_number,
        std::vector<UINT> in_element_group_number = {});

    void allocate_default_resource(
        directx_gpu_resource_element* in_res_elem,
        UINT in_elem_size,
        UINT in_number,
        void* in_cpu_data,
        std::vector<UINT> in_element_group_number = {});

    void update_all_upload_resource(
        void* data,
        directx_gpu_resource_element* in_res_elem);

    void update_elem_upload_resource(
        void* data,
        int elementIndex,
        directx_gpu_resource_element* in_res_elem);

public:

    virtual gpu_resource_element* allocate_gpu_memory(GPU_RESOURCE_LAYOUT in_resource_layout) override;

    //virtual gpu_resource* update_gpu_memory(cg_resource* in_resource) override;

    virtual gpu_resource_element* create_gpu_texture(
        std::string in_gpu_texture_name,
        GPU_RESOURCE_LAYOUT::GPU_RESOURCE_TYPE in_resoure_type) override;

    void switch_gpu_resource_state(
        directx_gpu_resource_element* in_gpu_res_elem,
        D3D12_RESOURCE_STATES in_new_resource_states);

    void screen_vertexs_and_indexes_input();

public:

    virtual void draw_call(
        constant_pass* in_pass,
        std::vector<gpu_resource*>& in_object,
        gpu_resource* in_sence) override;

    virtual constant_pass* allocate_pass(constant_pass::pass_layout in_constant_pass_layout) override;

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

    D3D12_RESOURCE_STATES current_state = D3D12_RESOURCE_STATE_GENERIC_READ;

    DXGI_FORMAT dx_format;
};

class directx_gpu_resource : public gpu_resource
{
public:
    ComPtr<ID3D12DescriptorHeap> srv_heap = nullptr;
};

class directx_constant_pass : public constant_pass
{
public:
    ComPtr<ID3D12DescriptorHeap> srv_heap = nullptr;
    ComPtr<ID3D12DescriptorHeap> rtv_heap = nullptr;
    ComPtr<ID3D12DescriptorHeap> dsv_heap = nullptr;
    ComPtr<ID3D12DescriptorHeap> uav_heap = nullptr;

    ComPtr<ID3D12RootSignature> rootsignature = nullptr;

    std::unordered_map<std::string, ComPtr<ID3DBlob>> shader_group;
    ComPtr<ID3D12PipelineState> pso;

    std::vector<D3D12_INPUT_ELEMENT_DESC> input_layout;

};

