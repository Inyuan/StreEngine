#pragma once
#include <wrl.h>
#include "Core/Render_Core/Render_API/d3dx12.h"
#include "stre_render.h"
#include <string>
using Microsoft::WRL::ComPtr;

class gpu_resource
{

public:
    std::string gpu_resource_name;

    enum GPU_RESOURCE_TYPE
    {
        GPU_RES_VERTEX,
        GPU_RES_INDEX,
        GPU_RES_TEXTURE,
        GPU_RES_BUFFER
    };

    GPU_RESOURCE_TYPE gpu_resource_type = GPU_RESOURCE_TYPE::GPU_RES_BUFFER;
    s_resource* resource;
};


class directx_gpu_resource : public gpu_resource
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