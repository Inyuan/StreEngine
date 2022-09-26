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

	enum PASS_INPUT_RESOURCE_TYPE
	{
		USE_CAMERA_CB = 0,
		USE_LIGHT_CB = 1,
		USE_CUSTOM_TEXTURE = 2,
		PASS_INPUT_RESOURCE_TYPE_NUMBER = 3
	};

	//enum PASS_RESOURCE_TYPE
	//{
	//	PASS_RES_CBV, // �̶���С��
	//	PASS_RES_SRV, // ���������ͼ
	//	PASS_RES_DESCRIPTOR_TBALE //������ͼ
	//};

	//Mesh��������Ҫ�������
	//ֻ����RT��CB������
	//struct input_layout
	//{
	//	PASS_INPUT_RESOURCE_TYPE input_resource_type = USE_CUSTOM_TEXTURE;
	//	UINT input_resource_number = 1;
	//	gpu_resource* input_gpu_resource = nullptr;
	//};

	struct shader_layout
	{
		struct shader_input
		{
			enum INPUT_ELEMENT_SIZE
			{
				INPUT_ELEMENT_SIZE_R32 = 4,
				INPUT_ELEMENT_SIZE_R32G32 = 8,
				INPUT_ELEMENT_SIZE_R32G32B32 = 12,
				INPUT_ELEMENT_SIZE_R32G32B32A32 = 16,
			};
			std::string name;
			//����Ϊ4������
			INPUT_ELEMENT_SIZE size;

			shader_input() = delete;

			shader_input(
				std::string in_name,
				INPUT_ELEMENT_SIZE in_size) 
				: name(in_name),
				size(in_size){}
		};
		enum SHADER_TYPE
		{
			VS = 0,
			DS = 1,
			HS = 2,
			GS = 3,
			PS = 4
		};
		bool shader_vaild[5] = { false };
		std::wstring shader_path[5];
		std::vector<shader_input> shader_input_group;
	};

	//��Դ���밴˳�򰲷�
	struct pass_layout
	{
		std::string pass_name;

		PASS_TYPE pass_type;

		shader_layout pass_shader_layout;
		bool use_resource_flag[PASS_INPUT_RESOURCE_TYPE::PASS_INPUT_RESOURCE_TYPE_NUMBER] = {false};
		std::vector<gpu_resource*> input_gpu_resource;
		std::vector<gpu_resource*> output_gpu_resource;
		gpu_resource* output_gpu_depth_stencil_resource;
	};

	pass_layout constant_pass_layout;

public:
	std::string pass_name;

	constant_pass() = delete;

	constant_pass(std::string* in_pass_name)
	{
		pass_name = *in_pass_name;
	}

};

