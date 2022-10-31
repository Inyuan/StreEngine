#pragma once
#include <Windows.h>
#include "cpu_resource.h"
#include "stre_pass.h"
/***
************************************************************
*
* Configuration
*
************************************************************
*/

///configuration
#define CLIENT_WIDTH s_render_configuration::client_width
#define CLIENT_HEIGHT s_render_configuration::client_height

#define MSAAX4_STATE s_render_configuration::msaax4_state
#define MSAAX4_QUALITY s_render_configuration::msaax4_quality


static class s_render_configuration
{
public:
	static unsigned int client_width;
	static unsigned int client_height;

	static bool msaax4_state;
	static unsigned int msaax4_quality;
};

unsigned int s_render_configuration::client_width = 800;
unsigned int s_render_configuration::client_height = 600;
bool s_render_configuration::msaax4_state = false;
unsigned int s_render_configuration::msaax4_quality = 0;

/***
************************************************************
*
* Renderer
*
************************************************************
*/

///rendering type
//��Ⱦ��



/***
************************************************************
*
* Resource Manager
*
************************************************************
*/

struct update_request_layout
{
	enum
	{
		UPDATE_TYPE_UPDATE,
		UPDATE_TYPE_ADD,
		UPDATE_TYPE_DELETE
	}update_type = UPDATE_TYPE_UPDATE;

	int offset = 0;
};


//��Դ���뵼���� (�ڲ��½����ڴ��ɾ�̬memory_allocater����)
//��Դ��CPU��GPU�ڴ����

//��Դ���� ����Ĵ�������factory�̳�

template<typename t_cpu_res_type,class t_render>
struct custom_manager
{
	static t_cpu_res_type* create_resource();

	static t_cpu_res_type* create_resource(size_t in_element_number);

	//??? ȱ��ȡ��Դ
	static t_cpu_res_type* load_resource(wchar_t* in_path);

	static void update_gpu(const t_cpu_res_type* in_cpu_data);

	//������Ĭ������ռ� ������뱨��
	//static void allocate_gpu(const t_cpu_res_type* in_cpu_data);

	static void allocate_gpu(t_cpu_res_type* in_cpu_data, 
		gpu_shader_resource::SHADER_RESOURCE_TYPE in_sr_type,
		std::vector<UINT>& elem_group_number);
};


//��ʾ����������͵Ĳ���

template<class t_render>
struct custom_manager<cpu_material, t_render>
{
	static cpu_material* create_resource();
	//??? ȱ��ȡ��Դ
	static cpu_material* load_resource(wchar_t* in_path);

	static void allocate_gpu(cpu_material* in_cpu_data);

	static void update_gpu(const cpu_material* in_cpu_data);

	static void release_gpu(cpu_material* in_cpu_data);

	//����Ĵ�����
	//�ص��󶨹�ϵ
	static void change_texture();

	static void add_texture();

	static void release_texture();
};

template<class t_render>
struct custom_manager<cpu_texture, t_render>
{
public:
	static cpu_texture* create_resource();

	//??? ȱ��ȡ��Դ
	static cpu_texture* load_resource(wchar_t* in_path);

	static void allocate_gpu(cpu_material* in_cpu_data);

	static void update_gpu(const cpu_texture* in_cpu_data);


};


template<class t_render>
struct custom_manager<cpu_mesh, t_render>
{
public:
	static cpu_mesh* create_resource();

	//??? ȱ��ȡ��Դ
	static cpu_mesh* load_resource(wchar_t* in_path);

	static void update_gpu(const cpu_mesh* in_cpu_data);

	static void allocate_gpu(cpu_mesh* in_cpu_data);

	//����Ĵ�����
	static cpu_mesh* load_fbx(wchar_t* in_path);


};

template<class t_render>
struct custom_manager<cpu_sence, t_render>
{
public:
	static cpu_sence* create_resource();

	//??? ȱ��ȡ��Դ
	static cpu_sence* load_resource(wchar_t* in_path);

	static void update_gpu(const cpu_sence* in_cpu_data);

};

//����ģʽ
template<
	typename t_cpu_res_type, 
	template<typename,class> class t_cpu_res_manager,
	class t_render>
class resource_factory : public t_cpu_res_manager<t_cpu_res_type,t_render>
{
public:

	//??? ȱĬ�ϵ����幹��
	virtual t_cpu_res_type* create_resource()
	{
		return t_cpu_res_manager<t_cpu_res_type,t_render>::create_resource();
	};

	//??? ȱ��ȡ��Դ
	virtual t_cpu_res_type* load_resource(wchar_t* in_path)
	{
		return t_cpu_res_manager<t_cpu_res_type, t_render>::load_resource(in_path);
	}

	virtual void allocate_gpu(t_cpu_res_type* in_cpu_data)
	{
		t_cpu_res_manager<t_cpu_res_type, t_render>::allocate_gpu(in_cpu_data);
	};

	virtual void update_gpu(const t_cpu_res_type* in_cpu_data)
	{
		t_cpu_res_manager<t_cpu_res_type, t_render>::update_gpu(in_cpu_data);
	};

};

/***
************************************************************
*
* Pass Manager
*
************************************************************
*/

class pass_factory
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
		const gpu_rander_target* in_gpu_rt);

	void remove_mesh(
		s_pass* in_out_pass,
		const cpu_mesh* in_mesh);

	template<class t_cpu_resource>
	void remove_shader_resource(
		s_pass* in_out_pass,
		const t_cpu_resource* in_gpu_sr);


	void remove_render_target(
		s_pass* in_out_pass,
		const gpu_rander_target* in_gpu_rt);
};

