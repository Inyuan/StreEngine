#pragma once
#include "base_type.h"
#include <vector>
#include <string>
#include <memory>
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
		SHADER_RESOURCE_TYPE_CUSTOM_BUFFER_FOLLOW_MESH,//CSV ��directx_sr_custom_buffer_group,��mesh������ѡ�������ڶ�Ӧ��csv����
		SHADER_RESOURCE_TYPE_TEXTURE, //SRV
		SHADER_RESOURCE_TYPE_TEXTURE_GROUP, //TABLE //��ͼ�����ͱ�������package_textures���ܹ���
		SHADER_RESOURCE_TYPE_RENDER_TARGET, //SRV
		SHADER_RESOURCE_TYPE_RENDER_TARGET_GROUP, // TABLE //��ͼ�����ͱ�������package_textures���ܹ���
		SHADER_RESOURCE_TYPE_RENDER_DEPTH_STENCIL, // DSV 
		SHADER_RESOURCE_TYPE_RENDER_DEPTH_STENCIL_GROUP, //��ͼ�����ͱ�������package_textures���ܹ���
		SHADER_RESOURCE_TYPE_SAMPLER,
		SHADER_RESOURCE_TYPE_UNORDERED_BUFFER,
		SHADER_RESOURCE_TYPE_NONE
	
	} shader_resource_type;

	//ʹ�õļĴ������
	//-1pass������
	//���pass��Ҫ��¼���������λ�ã���Ȼ�ᱬը
	std::map<std::string, int> register_index_map;


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
	bool is_output = false;
	bool is_translate = false;
	bool is_depth_check = false;
	UINT rt_number = 0;

	struct pass_resource
	{
		std::string name;
		UINT bind_point = 0;
		UINT bind_count = 0;
		UINT register_space = 0;
		gpu_shader_resource::SHADER_RESOURCE_TYPE type = gpu_shader_resource::SHADER_RESOURCE_TYPE_CUSTOM_BUFFER;

	};

	std::map<std::string,pass_resource> pass_res_group;

	virtual ~gpu_pass()
	{

	}
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
	s_uid uid;
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
	bool is_translate = false;
	bool is_depth_check = false;

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
	std::map < std::string, std::shared_ptr<gpu_shader_resource>> gpu_rt_texture_ptr;

	~s_pass()
	{
		if (gpu_pass_ptr)delete(gpu_pass_ptr);
		gpu_pass_ptr = nullptr;
	}
};

//api�ṩ���ټ��㷽��
struct camera_cal_helper
{
	// Camera coordinate system with coordinates relative to world space.
	s_float3 world_position = { 0.0f, 0.0f, 0.0f };
	s_float3 right_axi = { 1.0f, 0.0f, 0.0f };
	s_float3 up_axi = { 0.0f, 1.0f, 0.0f };
	s_float3 look_axi = { 0.0f, 0.0f, 1.0f };

	// Cache frustum properties.
	float near_z = 0.0f;
	float far_z = 0.0f;
	float aspect = 0.0f;
	float fov_y = 0.0f;
	float near_window_height = 0.0f;
	float far_window_height = 0.0f;
	// Cache View/Proj matrices.
	s_float4x4 view_matrix = s_float4x4::identity4x4();
	s_float4x4 project_matrix = s_float4x4::identity4x4();
};