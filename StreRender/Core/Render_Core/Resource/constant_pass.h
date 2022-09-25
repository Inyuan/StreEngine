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
	//��Ļ�ռ仹�Ƕ������
	enum PASS_TYPE
	{
		MESH_PASS,//�������
		SCREEN_PASS//��Ļ�ռ�
	};

	enum PASS_RESOURCE_TYPE
	{
		PASS_RES_TEXTURE,
		PASS_RES_BUFFER,
		PASS_RES_MULT_BUFFER
	};

	//Mesh��������Ҫ�������
	//ֻ����RT��CB������
	struct input_layout
	{
		PASS_RESOURCE_TYPE input_resource_type = PASS_RESOURCE_TYPE::PASS_RES_BUFFER;
		UINT input_resource_number = 1;
		gpu_resource* input_gpu_resource = nullptr;
	};

	//��Դ���밴˳�򰲷�
	struct pass_layout
	{
		PASS_TYPE pass_type;

		std::vector<input_layout> input_gpu_resource_layout;
		std::vector<gpu_resource*> output_gpu_resource;
	};

public:
	std::string pass_name;

	constant_pass() = delete;

	constant_pass(std::string* in_pass_name)
	{
		pass_name = *in_pass_name;
	}
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