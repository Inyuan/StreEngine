#pragma once
#include "base_type.h"
#include <vector>
#include <string>

//���߳̿�����С��λ

//����ˢ�£����߳��������ݿ�
//custom buffer
//index
//vertex



//Ӧ����ģ�壿ֻΪ��Ӧ��api�ṩ��Ӧ����Ϣ��
struct gpu_shader_resource
{
	enum SHADER_RESOURCE_TYPE
	{
		SHADER_RESOURCE_TYPE_CUSTOM_BUFFER, // CSV
		SHADER_RESOURCE_TYPE_CUSTOM_BUFFER_GROUP, //SRV
		//����mesh������˳���ȡ��CBV group ex:material
		SHADER_RESOURCE_TYPE_CUSTOM_BUFFER_GROUP_FOLLOW_MESH,
		SHADER_RESOURCE_TYPE_TEXTURE, //SRV
		SHADER_RESOURCE_TYPE_TEXTURE_GROUP, //TABLE
		SHADER_RESOURCE_TYPE_RENDER_TARGET, //SRV
		SHADER_RESOURCE_TYPE_RENDER_TARGET_GROUP, // TABLE
		SHADER_RESOURCE_TYPE_RENDER_DEPTH_STENCIL, // DSV
		SHADER_RESOURCE_TYPE_RENDER_DEPTH_STENCIL_GROUP
	} shader_resource_type;


	size_t register_index = 0; //ʹ�õļĴ������

	std::string name;
	const bool can_update = false;
	//ֻ���ϴ�����Ч
	BYTE* mapped_data = nullptr;

	size_t element_size = 0;
	size_t element_count = 0;

	gpu_shader_resource(bool in_can_update = false) : can_update(in_can_update) {};
	//...
};




struct gpu_pass
{
	//??? ���е���ɫ��������
	s_uid uid;
};

struct gpu_rander_target
{
	//??? ���е���ȾĿ�������
	s_uid uid;

	gpu_shader_resource* gpu_sr_ptr;
};