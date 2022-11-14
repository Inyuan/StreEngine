#pragma once
#include <Windows.h>
#include "stre_configuration.h"
#include "cpu_resource.h"
#include "gpu_resource.h"
#include "stre_render_api.h"
#ifdef  DLL_GRAPHICS_API
#else
#define DLL_GRAPHICS_API _declspec(dllimport)
#endif


///rendering type
//��Ⱦ��

/// <summary>
/// ��Ⱦϵͳ���ⲿ����GPU�Ļ������ͣ������ܻ������̡�
/// </summary>
class s_render_system
{
public:

	virtual void draw_pass(s_pass* in_pass) = 0;

	//��������ˢ����
	virtual void update_gpu_memory() = 0;

	virtual void execute_command() = 0;

	virtual void init(HINSTANCE in_instance, UINT in_width, UINT in_height) = 0;

	virtual void over() = 0;
};


class DLL_GRAPHICS_API render_factory
{
public:
	template<class t_render>
	s_render_system* create_render_system();

	template<>
	s_render_system* create_render_system<s_directx_render>();
};

/***
************************************************************
*
* Resource Manager
*
************************************************************
*/

/// <summary>
/// �Զ�����Դ������
/// </summary>
/// <typeparam name="t_cpu_res_type">�Զ�����Դ</typeparam>
template<typename t_cpu_res_type>
struct s_custom_manager
{
	virtual t_cpu_res_type* create_resource() = 0;

	virtual t_cpu_res_type* create_resource(size_t in_element_number) = 0;

	//??? ȱ��ȡ��Դ
	virtual t_cpu_res_type* load_resource(wchar_t* in_path) = 0;

	virtual void update_gpu(t_cpu_res_type* in_cpu_data) = 0;

	virtual void dx_allocate_gpu_resource(t_cpu_res_type* in_cpu_data,
		gpu_shader_resource::SHADER_RESOURCE_TYPE in_sr_type) = 0;
};

/// <summary>
/// Ĭ�ϲ�����Դ������
/// </summary>
struct s_material_manager : public s_custom_manager<cpu_material>
{
public:
	//����Ĵ�����
	//�ص��󶨹�ϵ
	virtual void change_texture() = 0;

	virtual void add_texture() = 0;

	virtual void release_texture() = 0;
};



/// <summary>
/// Ĭ����ͼ��Դ������
/// </summary>
struct s_texture_manager : public s_custom_manager<cpu_texture>
{
public:
	virtual void package_textures(
		std::vector<cpu_texture*> in_texture_group,
		cpu_texture* in_out_table) = 0;
};


/// <summary>
/// Ĭ��ģ�Ϳ�����
/// </summary>
struct s_mesh_manager
{
public:
	virtual cpu_mesh* create_resource() = 0;

	virtual cpu_mesh* load_resource(wchar_t* in_path) = 0;

	virtual void update_gpu(cpu_mesh* in_cpu_data) = 0;
	//����Ĵ�����
	virtual cpu_mesh* load_fbx(wchar_t* in_path) = 0;

	virtual void dx_allocate_gpu_resource(cpu_mesh* in_cpu_data) = 0;
};

/// <summary>
/// Ĭ�ϳ���������
/// </summary>
/// <returns></returns>
struct s_sence_manager
{
public:
	virtual cpu_sence* create_resource() = 0;

	virtual cpu_sence* load_resource(wchar_t* in_path) = 0;

	virtual void update_gpu(cpu_sence* in_cpu_data) = 0;

	virtual void dx_allocate_gpu_resource(cpu_sence* in_cpu_data) = 0;
};

/// <summary>
/// ��Դ����������
/// </summary>
class DLL_GRAPHICS_API resource_manager_factory
{
public:
	template<typename t_cpu_res_type>
	s_custom_manager<t_cpu_res_type>* create_manager();
	
	//��Ҫ��ʾ����ģ�壬���򲻻���뵽����
	template<>
	s_custom_manager<cpu_vertex>* create_manager();

	template<>
	s_custom_manager<cpu_index>* create_manager();

	template<>
	s_custom_manager<cpu_camera>* create_manager();

	template<>
	s_custom_manager<cpu_light>* create_manager();

	//�����⺯���ľ�Ҫ������д
	s_material_manager* create_material_manager();

	s_texture_manager* create_texture_manager();

	s_mesh_manager* create_mesh_manager();

	s_sence_manager* create_sence_manager();
};



/***
************************************************************
*
* Pass Manager
*
************************************************************
*/

/// <summary>
/// ���ƹ��̹���
/// </summary>
class DLL_GRAPHICS_API pass_factory
{
public:
	//����pass
	s_pass* create_pass();
	void dx_allocate_gpu_pass(s_pass* in_out_pass);
	//??? ��ˢ��pass����

	void add_mesh(
		s_pass* in_out_pass,
		const cpu_mesh* in_mesh);

	//cpu_resource
	template<class t_cpu_resource>
	void add_shader_resource(
		s_pass* in_out_pass,
		const t_cpu_resource* in_gpu_sr);

	void add_render_target(
		s_pass* in_out_pass,
		const cpu_texture* in_gpu_rt);

	void remove_mesh(
		s_pass* in_out_pass,
		const cpu_mesh* in_mesh);

	template<class t_cpu_resource>
	void remove_shader_resource(
		s_pass* in_out_pass,
		const t_cpu_resource* in_gpu_sr);


	void remove_render_target(
		s_pass* in_out_pass,
		const cpu_texture* in_gpu_rt);


	void set_shader_layout(
		s_pass* in_out_pass,
		const shader_layout in_shade_layout);
};

