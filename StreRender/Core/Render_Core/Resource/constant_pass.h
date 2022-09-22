#pragma once
#include <vector>
#include <unordered_map>
#include <string>
#include <wrl.h>
#include "Core/Render_Core/Render_API/d3dx12.h"
#include "gpu_resource.h"
#include "stre_render.h"
using Microsoft::WRL::ComPtr;



//���������ͼ
//��ʲô��Դ
//�������
class constant_pass
{
public: 
	enum PASS_TYPE
	{
		MESH_PASS,//�������
		SCREEN_PASS//��Ļ�ռ�
	};

	enum PASS_RESOURCE_TYPE
	{
		PASS_RES_MESH,
		PASS_RES_TEXTURE,
		PASS_RES_BUFFER
	};

	struct pass_layout
	{
		//��Ļ�ռ仹�Ƕ������
		PASS_TYPE pass_type;
		std::vector<s_object::SENCE_OBJECT_TYPE> input_object_type;
		std::vector<std::string> input_gpu_resource;
		std::vector<std::string> output_gpu_resource;
	};

public:
	std::string pass_name;
};



class directx_constant_pass : public constant_pass
{
public:
	ComPtr<ID3D12DescriptorHeap> srv_heap = nullptr;
	ComPtr<ID3D12DescriptorHeap> rtv_heap = nullptr;
	ComPtr<ID3D12DescriptorHeap> dsv_heap = nullptr;
	ComPtr<ID3D12DescriptorHeap> cbv_heap = nullptr;
	ComPtr<ID3D12DescriptorHeap> uav_heap = nullptr;
	ComPtr<ID3D12RootSignature> rootsignature = nullptr;

	std::unordered_map<std::string, ComPtr<ID3DBlob>> shader_group;
	std::unordered_map<std::string, ComPtr<ID3D12PipelineState>> pso_group;

	std::vector<D3D12_INPUT_ELEMENT_DESC> input_layout;

};