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
#include <array>
#include <unordered_map>
#include "d3dx12.h"
#include "stre_render_api.h"
#include "Core/Window/s_window.h"

#pragma comment(lib,"d3dcompiler.lib")
#pragma comment(lib, "D3D12.lib")
#pragma comment(lib, "dxgi.lib")


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

class directx_render : public directx_render_abstract
{
protected:


    Microsoft::WRL::ComPtr<IDXGIFactory4> dxgi_factory;
    Microsoft::WRL::ComPtr<IDXGISwapChain> swap_chain;
    Microsoft::WRL::ComPtr<ID3D12Device> d3d_device;

    Microsoft::WRL::ComPtr<ID3D12Fence> fence;

    Microsoft::WRL::ComPtr<ID3D12CommandQueue> command_queue;
    Microsoft::WRL::ComPtr<ID3D12CommandAllocator> direct_cmdlist_alloc;
    Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> command_list;

public:
    struct directx_shader_resource : public gpu_shader_resource
    {
        ComPtr<ID3D12Resource> dx_resource;
        D3D12_SHADER_RESOURCE_VIEW_DESC dx_srv;
        D3D12_RENDER_TARGET_VIEW_DESC dx_rtv;
        D3D12_DEPTH_STENCIL_VIEW_DESC dx_dsv;
        D3D12_CONSTANT_BUFFER_VIEW_DESC dx_cbv;
        D3D12_UNORDERED_ACCESS_VIEW_DESC dx_uav;
        D3D12_RESOURCE_STATES dx_current_state = D3D12_RESOURCE_STATE_GENERIC_READ;
        DXGI_FORMAT dx_format;
    };
    
    typedef directx_shader_resource directx_sr_custom_buffer;//csv
    typedef directx_shader_resource directx_sr_custom_buffer_group;//srv
    typedef directx_shader_resource directx_sr_texture;//srv
    typedef directx_shader_resource directx_sr_render_target;//srv rtv
    typedef directx_shader_resource directx_sr_depth_stencil;//srv dsv
    //模板？
    struct directx_sr_texture_group : public gpu_shader_resource
    {
        ComPtr<ID3D12DescriptorHeap> srv_heap = nullptr;
    };

    struct directx_sr_render_target_group : public gpu_shader_resource
    {
        UINT rt_number = 0;
        ComPtr<ID3D12DescriptorHeap> rtv_heap = nullptr;
    };

    struct directx_sr_depth_stencil_group : public directx_shader_resource
    {
        ComPtr<ID3D12DescriptorHeap> dsv_heap = nullptr;
    };

    //struct
    //{
    //    ComPtr<ID3D12Resource> dx_resource;
    //    D3D12_SHADER_RESOURCE_VIEW_DESC dx_srv;
    //    D3D12_RENDER_TARGET_VIEW_DESC dx_rtv;
    //    D3D12_DEPTH_STENCIL_VIEW_DESC dx_dsv;
    //    D3D12_CONSTANT_BUFFER_VIEW_DESC dx_csv;
    //    D3D12_UNORDERED_ACCESS_VIEW_DESC dx_uav;
    //    D3D12_RESOURCE_STATES dx_current_state = D3D12_RESOURCE_STATE_GENERIC_READ;
    //    DXGI_FORMAT dx_format;
    //    ComPtr<ID3D12DescriptorHeap> srv_heap = nullptr;
    //    ComPtr<ID3D12DescriptorHeap> rtv_heap = nullptr;
    //    ComPtr<ID3D12DescriptorHeap> dsv_heap = nullptr;
    //    ComPtr<ID3D12DescriptorHeap> uav_heap = nullptr;
    //};
    
    //...

    struct directx_pass : public gpu_pass
    {
    public:

        ComPtr<ID3D12RootSignature> rootsignature = nullptr;

        std::unordered_map<std::string, ComPtr<ID3DBlob>> shader_group;
        ComPtr<ID3D12PipelineState> pso;

        std::vector<D3D12_INPUT_ELEMENT_DESC> input_layout;
    };

private:
    void Load_resource(
        const std::map < std::string, const gpu_shader_resource*>& in_gpu_res_group,
        bool set_render_tager = false);

    void draw_call(const s_pass::gpu_mesh_resource* in_gpu_mesh);

public:
    void init(HWND in_main_wnd);

    void over() {};

    void draw_pass(const s_pass* in_pass);

public:

    virtual gpu_pass* allocate_pass() override;

    virtual gpu_shader_resource* allocate_shader_resource(
        gpu_shader_resource::SHADER_RESOURCE_TYPE in_shader_res_type) override;

    virtual void allocate_upload_resource(
        gpu_shader_resource* in_res_elem,
        UINT in_elem_size,
        UINT in_number) override;

    virtual void allocate_default_resource(
        gpu_shader_resource* in_res_elem,
        UINT in_elem_size,
        UINT in_number,
        void* in_cpu_data) override;

    virtual void create_gpu_texture(gpu_shader_resource* in_out_gpu_texture) override;

    virtual void pakeage_textures(
        std::vector<const gpu_shader_resource*>& in_texture_group,
        gpu_shader_resource* in_out_table) override;

    virtual void load_rootparpameter(
        std::vector<CD3DX12_ROOT_PARAMETER> & in_out_root_parameter,
        const gpu_shader_resource* in_gpu_sr) override;

    virtual void create_rootsignature(
        CD3DX12_ROOT_SIGNATURE_DESC& in_rootsig_desc, 
        gpu_pass* in_gpu_pass) override;

    virtual void create_pso(
        shader_layout in_shader_layout,
        gpu_pass* in_gpu_pass,
        UINT in_rt_number,
        bool is_translate = false) override;



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
private:

    void create_descriptor_heap(
        Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> in_out_descheap,
        DIRECTX_RESOURCE_DESC_TYPE in_texture_desc_type,
        UINT in_desc_number);
    
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

    /// <summary>
    /// 
    /// </summary>
    /// <param name="in_texture_desc_type"></param>
    /// <param name="in_gpu_res_elem"></param>
    /// <param name="in_out_dest_descriptor">handle位置</param>
    void load_descriptor_into_heap(
        const DIRECTX_RESOURCE_DESC_TYPE in_texture_desc_type,
        const directx_shader_resource* in_gpu_res_elem,
        CD3DX12_CPU_DESCRIPTOR_HANDLE in_out_dest_descriptor);

    void create_descriptor(
        DIRECTX_RESOURCE_DESC_TYPE in_texture_desc_type,
        directx_shader_resource* in_gpu_res_elem);

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
    
    void switch_gpu_resource_state(
        directx_shader_resource* in_gpu_res_elem,
        D3D12_RESOURCE_STATES in_new_resource_states);

    void screen_vertexs_and_indexes_input();
};

