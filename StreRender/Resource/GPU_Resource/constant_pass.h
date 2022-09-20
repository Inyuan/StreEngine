#pragma once
#include <vector>
#include <unordered_map>
#include <string>
#include <wrl.h>
#include "Core/Render_Core/d3dx12.h"

using Microsoft::WRL::ComPtr;

class constant_pass
{
public:
	ComPtr<ID3D12DescriptorHeap> SRVHeap = nullptr;
	ComPtr<ID3D12DescriptorHeap> RTVHeap = nullptr;
	ComPtr<ID3D12DescriptorHeap> DSVHeap = nullptr;
	ComPtr<ID3D12DescriptorHeap> CBVHeap = nullptr;
	ComPtr<ID3D12DescriptorHeap> UAVHeap = nullptr;
	ComPtr<ID3D12DescriptorHeap> RootSignature = nullptr;

	std::unordered_map<std::string, ComPtr<ID3DBlob>> mShaders;
	std::unordered_map<std::string, ComPtr<ID3D12PipelineState>> PSOs;

	std::vector<D3D12_INPUT_ELEMENT_DESC> InputLayout;

};