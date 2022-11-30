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
#include <d3d12shader.h>
#include "stre_render_api.h"
#include "Core/Window/s_window.h"

#pragma comment(lib,"d3dcompiler.lib")
#pragma comment(lib, "D3D12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib")

#define SWAP_CHAIN_BUFFER_COUNT 2
#define FRAME_BUFFER_COUNT SWAP_CHAIN_BUFFER_COUNT

using Microsoft::WRL::ComPtr;
// WRL

enum DIRECTX_RESOURCE_DESC_TYPE
{
    DX_SRV,
    DX_RTV,
    DX_DSV,
    DX_CBV,
    DX_UAV
};

class directx_render : public s_directx_render
{
public:

    struct directx_resource
    {
        UINT64 fence = 0;
    };

    //自定义资源 包括顶点 索引 mat 等常刷新的数据，以多帧资源形式存在
    struct directx_frame_resource : public gpu_shader_resource
    {
        std::shared_ptr<directx_resource> frame_resource_group[FRAME_BUFFER_COUNT];
    };

    //贴图类型数据，单图 多图 ，只有一份的存在
    struct directx_texture_resource : public gpu_shader_resource
    {
        std::shared_ptr <directx_resource> resource;
    };

    struct directx_shader_resource : public directx_resource
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


    typedef directx_shader_resource directx_sr_texture;//srv
    typedef directx_shader_resource directx_sr_render_target;//srv rtv
    typedef directx_shader_resource directx_sr_depth_stencil;//srv dsv

    struct directx_sr_custom_buffer : public directx_shader_resource
    {
        //???
        unsigned char* mapped_data = nullptr;
    };
    typedef directx_sr_custom_buffer directx_sr_custom_buffer_group;//srv


    //贴图组类型必须再用package_textures才能构建

    struct directx_sr_texture_group : public directx_resource
    {
        ComPtr<ID3D12DescriptorHeap> srv_heap = nullptr;
    };

    struct directx_sr_render_target_group : public directx_sr_texture_group
    {
        //用于刷新状态
        std::vector< std::shared_ptr<directx_sr_render_target>> rt_group;
        UINT rt_number = 0;
        ComPtr<ID3D12DescriptorHeap> rtv_heap = nullptr;
    };

    struct directx_sr_depth_stencil_group : public directx_sr_texture_group
    {
        //用于刷新状态
        std::shared_ptr < directx_sr_depth_stencil> ds_ptr;
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

        virtual ~directx_pass() 
        {

        }
    };

protected:


    Microsoft::WRL::ComPtr<IDXGIFactory4> dx_factory;
    Microsoft::WRL::ComPtr<ID3D12Device> dx_device;
    Microsoft::WRL::ComPtr<ID3D12Fence> dx_fence;

    Microsoft::WRL::ComPtr<ID3D12CommandQueue> dx_command_queue;
    Microsoft::WRL::ComPtr<ID3D12CommandAllocator> dx_cmdlist_alloc;
    Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> dx_command_list;

    Microsoft::WRL::ComPtr<IDXGISwapChain> dx_swap_chain;

    Microsoft::WRL::ComPtr<ID3D12CommandAllocator> dx_frame_cmdlist_alloc[FRAME_BUFFER_COUNT];
    std::shared_ptr < directx_sr_render_target> swap_chain_buffer[SWAP_CHAIN_BUFFER_COUNT];
    std::shared_ptr < directx_sr_render_target_group> swap_chain_buffer_heap[SWAP_CHAIN_BUFFER_COUNT];
    UINT current_back_buffer = 0;
    UINT64 current_fence = 0;
    UINT current_frame = 0;



private:
    
    
    
    void set_render_target(
        const std::map < std::string, std::shared_ptr<gpu_shader_resource>>& in_gpu_res_group,
        bool is_final_output,
        std::shared_ptr < directx_sr_render_target_group>& render_targets_ptr,
        std::shared_ptr < directx_sr_depth_stencil_group>& depth_stencil_ptr);

    void load_resource(const std::map < std::string, const gpu_shader_resource*>& in_gpu_res_group);

    void draw_call(const s_pass::gpu_mesh_resource* in_gpu_mesh);

public:
    void init(HWND in_main_wnd, UINT in_width, UINT in_height);

    void over() {};

    void draw_pass(s_pass* in_pass);

    void excute_command_list();

    void reset_command_allocator();

    void switch_swap_chain();

    void flush_command_queue();
public:

    virtual gpu_pass* allocate_pass() override;

    virtual std::shared_ptr<gpu_shader_resource> allocate_shader_resource(
        gpu_shader_resource::SHADER_RESOURCE_TYPE in_shader_res_type) override;

    virtual bool update_gpu_resource(
        std::shared_ptr<gpu_shader_resource> in_out_gpu_res,
        const void* in_data,
        UINT in_update_element_index,
        size_t int_update_element_count) override;

    virtual bool allocate_upload_resource(
        std::shared_ptr<gpu_shader_resource> in_res_elem,
        UINT in_elem_size,
        UINT in_number) override;

    virtual bool allocate_default_resource(
        std::shared_ptr<gpu_shader_resource> in_res_elem,
        UINT in_elem_size,
        UINT in_number,
        void* in_cpu_data) override;

    virtual bool package_textures(
        std::vector<std::shared_ptr<gpu_shader_resource>>& in_texture_group,
        std::shared_ptr<gpu_shader_resource> in_out_table) override;

    //virtual void load_rootparpameter(
    //    std::vector<CD3DX12_ROOT_PARAMETER> & in_out_root_parameter,
    //    const gpu_shader_resource* in_gpu_sr) override;

    virtual bool create_rootsignature(gpu_pass* in_gpu_pass) override;

    virtual bool complie_shader(
        shader_layout& in_shader_layout,
        gpu_pass* in_gpu_pass) override;

    virtual bool create_pso(
        shader_layout& in_shader_layout,
        gpu_pass* in_gpu_pass) override;


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

    //DS作为SR时按R24_UNORM_X8_TYPELESS解析
    DXGI_FORMAT depth_stencil_as_shader_resource_format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
    
    D3D12_RESOURCE_STATES default_resource_states = D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_GENERIC_READ;

    float clear_color[4] = { 0.0,0.0f,0.0f,0.0f };



private:
    void msaa_configuration();//???

    void ScreenViewportResize(int in_width, int in_height);
private:

    const directx_resource* get_current_frame_resource(const gpu_shader_resource* in_gpu_sr);
    const directx_resource* get_current_frame_resource(const directx_frame_resource* in_gpu_sr);

    //构建RT 和DS
    void create_gpu_texture(std::shared_ptr < directx_resource> in_out_gpu_texture,
        gpu_shader_resource::SHADER_RESOURCE_TYPE in_shader_res_type);


    void create_descriptor_heap(
        ComPtr<ID3D12DescriptorHeap>& in_out_descheap,
        DIRECTX_RESOURCE_DESC_TYPE in_texture_desc_type,
        UINT in_desc_number);
    
    void create_gpu_memory(
        ComPtr<ID3D12Resource>& in_out_resource,
        CD3DX12_HEAP_PROPERTIES in_heap_properties,
        CD3DX12_RESOURCE_DESC in_rescource_desc,
        D3D12_RESOURCE_STATES in_resource_states = D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_GENERIC_READ,
        D3D12_CLEAR_VALUE* clearValue = nullptr);


    void switch_gpu_memory_state(
        ComPtr<ID3D12Resource>& in_out_resource,
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
        std::shared_ptr < directx_shader_resource> in_gpu_res_elem,
        UINT in_memory_size = 0);

    void create_rootsignature(
        CD3DX12_ROOT_SIGNATURE_DESC& in_rootsig_desc,
        ComPtr<ID3D12RootSignature>& in_out_rootsignature);

    ID3DBlob* complie_shader(
        const std::wstring& filename,
        const D3D_SHADER_MACRO* defines,
        const std::string& entrypoint,
        const std::string& target,
        bool& out_successed);

    void reflect_shader(ComPtr<ID3DBlob>&  in_shader_data, std::vector<gpu_pass::pass_resource>& out_res_group);

    void create_pso(
        D3D12_GRAPHICS_PIPELINE_STATE_DESC& in_pso_desc,
        ComPtr<ID3D12PipelineState>& in_pso);
    
    void switch_gpu_resource_state(
        std::shared_ptr < directx_shader_resource> in_gpu_res_elem,
        D3D12_RESOURCE_STATES in_new_resource_states);

    void screen_vertexs_and_indexes_input();
};

