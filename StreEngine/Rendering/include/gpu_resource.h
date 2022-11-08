#pragma once
#include "base_type.h"
#include <vector>
#include <string>
#include <map>
//���߳̿�����С��λ

//����ˢ�£����߳��������ݿ�
//custom buffer
//index
//vertex

//Ӧ����ģ�壿ֻΪ��Ӧ��api�ṩ��Ӧ����Ϣ��

/// <summary>
/// GPU��Դ�������ͣ�����GPU��ȡ����ö�٣�����Դָ����Ϣ����ͼ��API�̳в���װ���廯���ӿڲ���GPU�Ļ�����λ��
/// </summary>
struct gpu_shader_resource
{
	enum SHADER_RESOURCE_TYPE
	{
		SHADER_RESOURCE_TYPE_CUSTOM_BUFFER, // CSV
		SHADER_RESOURCE_TYPE_CUSTOM_BUFFER_GROUP, //SRV
		//����mesh������˳���ȡ��CBV group ex:material
		SHADER_RESOURCE_TYPE_CUSTOM_BUFFER_GROUP_FOLLOW_MESH,
		SHADER_RESOURCE_TYPE_TEXTURE, //SRV
		SHADER_RESOURCE_TYPE_TEXTURE_GROUP, //TABLE //��ͼ�����ͱ�������package_textures���ܹ���
		SHADER_RESOURCE_TYPE_RENDER_TARGET, //SRV
		SHADER_RESOURCE_TYPE_RENDER_TARGET_GROUP, // TABLE //��ͼ�����ͱ�������package_textures���ܹ���
		SHADER_RESOURCE_TYPE_RENDER_DEPTH_STENCIL, // DSV 
		SHADER_RESOURCE_TYPE_RENDER_DEPTH_STENCIL_GROUP //��ͼ�����ͱ�������package_textures���ܹ���
	} shader_resource_type;


	uint32_t register_index = 0; //ʹ�õļĴ������

	std::string name;

	uint32_t element_size = 0;
	uint32_t element_count = 0;
	//...
};

/// <summary>
/// GPU���ƹ������ͣ���ͼ��API�̳в���װ���廯,�ӿڲ���GPU�Ļ�����λ
/// </summary>
struct gpu_pass
{
	//??? ���е���ɫ��������
	s_uid uid;
};

/// <summary>
/// ��ɫ���������ͣ��ǿ��ƻ��ƹ��̵ı�Ҫ�������ӿڲ���GPU�Ļ�����λ��
/// </summary>
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
			size(in_size) {}
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

/// <summary>
/// ģ����Դ���������ͣ��������� ���� ���ʵȵ���ģ�Ͱ󶨵���Դ��Ҳ������Ϊ��Ļ�ռ���Ƶ��������ͣ�ֻ����4������������������κζ������ݣ���
/// </summary>
struct mesh_shader_resource
{

	std::vector<const gpu_shader_resource*> sr_res;
};

// ���룺
// vertex layout
// rootsignature resource
// �����
// texutre resource
//

/// <summary>
/// ���ƹ������ͣ��ӿڿ���GPU���ƵĻ�����λ������װ��GPU��Դ�ͱ�ִ�л��ơ�
/// </summary>
struct s_pass
{
public:
	//??? ���е���ɫ��������
	s_uid uid;

	bool is_output = false;

	//��ͬ��Ⱦ����Ӧ��ͬ��gpu pass����
	gpu_pass* gpu_pass_ptr = nullptr;

	shader_layout gpu_shader_layout;


	//������Դ һһ��Ӧʵ��
	//
	struct gpu_mesh_resource
	{
		const gpu_shader_resource* vertex;
		const gpu_shader_resource* index;

		struct mesh_element
		{
			unsigned int index_start_offset = 0;
			unsigned int index_number = 0;
		};
		std::vector<mesh_element> mesh_element_group;

		std::map < std::string, const gpu_shader_resource*> gpu_mesh_resource_ptr;
	};

	std::map < std::string, gpu_mesh_resource> gpu_mesh;

	//SRV��Դ�� buffer texture
	std::map < std::string, const gpu_shader_resource*> gpu_pass_resource_ptr;
	//�����Դ 
	std::map < std::string,gpu_shader_resource*> gpu_rt_texture_ptr;
};