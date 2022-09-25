#pragma once
#include <vector>
#include <unordered_map>
#include <string>
#include <wrl.h>
#include "Core/Render_Core/Render_API/d3dx12.h"
#include "gpu_resource.h"
#include "stre_render.h"
using Microsoft::WRL::ComPtr;



//输入输出贴图
//用什么资源
//输出到哪
class constant_pass
{
public: 
	//屏幕空间还是顶点绘制
	enum PASS_TYPE
	{
		MESH_PASS,//顶点绘制
		SCREEN_PASS//屏幕空间
	};

	enum PASS_RESOURCE_TYPE
	{
		PASS_RES_CBV, // 固定大小的
		PASS_RES_SRV, // 数组类或贴图
		PASS_RES_DESCRIPTOR_TBALE //多张贴图
	};

	//Mesh的输入需要额外控制
	//只包含RT和CB的输入
	struct input_layout
	{
		PASS_RESOURCE_TYPE input_resource_type = PASS_RESOURCE_TYPE::PASS_RES_SRV;
		UINT input_resource_number = 1;
		gpu_resource* input_gpu_resource = nullptr;
	};

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
			//必须为4的整数
			INPUT_ELEMENT_SIZE size;
		};
		enum SHADER_TYPE
		{
			VS = 0,
			DS = 1,
			HS = 2,
			GS = 3,
			PS = 4
		};
		UINT shader_vaild[5] = { 0 };
		std::wstring shader_path[5];
		std::vector<shader_input> shader_input_group;
	};

	//资源必须按顺序安放
	struct pass_layout
	{
		std::string pass_name;

		PASS_TYPE pass_type;

		shader_layout pass_shader_layout;

		std::vector<input_layout> input_gpu_resource_layout;
		std::vector<gpu_resource*> output_gpu_resource;
		gpu_resource* output_gpu_depth_stencil_resource;
	};


public:
	std::string pass_name;

	constant_pass() = delete;

	constant_pass(std::string* in_pass_name)
	{
		pass_name = *in_pass_name;
	}

};

