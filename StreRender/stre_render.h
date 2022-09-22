#pragma once

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

enum REDNER_API
{
	DIRECTX_RENDER
};

///

//��Ⱦ��
// ��Ⱦ���̵Ŀ���
class s_rendering
{
private:

public:
	enum class RENDERING_TYPE
	{
		DEFERRED_RENDERING = 0
	};
public:

	//��ʼ����Ⱦ���̣�����PASS��ע���׼��
	virtual void init(HINSTANCE in_instance, REDNER_API in_render_api) = 0;
	//���Ƴ���
	virtual void draw(s_sence* in_sence) = 0;

	virtual void over() = 0;
};

//��Դ������
// ����������Դ�����ɾ��
// ��Դ��CPU��GPU�ڴ����
class s_resource_manager
{
public:
	virtual s_object* add_object(s_sence * in_out_sence,s_object::SENCE_OBJECT_TYPE in_object_type) = 0;

	virtual void delete_object(s_sence* in_out_sence, s_object* in_object) = 0;

};


//��Դ
class s_resource
{
public:
	enum class RESOURCE_TYPE
	{
		RES_EMPTY_OBJECT,
		RES_STATIC_OBJECT,
		RES_DYNAMIC_OBJECT,
		RES_CAMERA,
		RES_LIGHT,
		RES_MATERIAL,
		RES_TEXTURE
	};
	RESOURCE_TYPE resource_type = RESOURCE_TYPE::RES_EMPTY_OBJECT;
	//�Ƿ��ڱ��ش���
	bool is_local = false;

	char local_path[256] = {'\0'};

	void* data_ptr;


};

//����
class s_object : public s_resource
{
public:
	enum class SENCE_OBJECT_TYPE
	{
		OBJ_EMPTY_OBJECT,
		OBJ_STATIC_OBJECT,
		OBJ_DYNAMIC_OBJECT,
		OBJ_CAMERA,
		OBJ_LIGHT
	};
	SENCE_OBJECT_TYPE object_type = SENCE_OBJECT_TYPE::OBJ_EMPTY_OBJECT;
	//�����
};

class s_mesh_object : public s_object
{
public:
	char mesh_type[32] = {'\0'};
	s_material* material_group;
};

//��̬����
class s_static_object : public s_mesh_object
{

};

//��̬����
class s_dynamic_object : public s_mesh_object
{

};

//���
class s_camera : public s_object
{
public:
	//�費��Ҫ����
	bool is_recording = true;
};

//�ƹ�
class s_light : public s_object
{

};

//����
class s_material : public s_resource
{
	s_texture* textrue_group;
};

//��ͼ
class s_texture : public s_resource
{
	
};

//����
class s_sence : public s_resource
{
private://???
	s_object* empty_object_group;
	s_static_object* static_object_group;
	s_dynamic_object* dynamic_object_group;
	s_camera* camera_group;
	s_light* light_group;

};