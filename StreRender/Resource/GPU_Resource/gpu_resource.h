#pragma once
#include <wrl.h>
#include "Core/Render_Core/d3dx12.h"
using Microsoft::WRL::ComPtr;

class gpu_resource
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