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

//渲染器
// 渲染流程的控制
class s_rendering
{
private:

public:
	enum class RENDERING_TYPE
	{
		DEFERRED_RENDERING = 0
	};
public:

	//初始化渲染流程，各种PASS的注册和准备
	virtual void init(HINSTANCE in_instance, REDNER_API in_render_api) = 0;
	//绘制场景
	virtual void draw(s_sence* in_sence) = 0;

	virtual void over() = 0;
};

//资源管理器
// 管理场景的资源加入和删除
// 资源的CPU和GPU内存管理
class s_resource_manager
{
public:
	virtual s_object* add_object(s_sence * in_out_sence,s_object::SENCE_OBJECT_TYPE in_object_type) = 0;

	virtual void delete_object(s_sence* in_out_sence, s_object* in_object) = 0;

};


//资源
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
	//是否在本地存在
	bool is_local = false;

	char local_path[256] = {'\0'};

	void* data_ptr;


};

//物体
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
	//坐标等
};

class s_mesh_object : public s_object
{
public:
	char mesh_type[32] = {'\0'};
	s_material* material_group;
};

//静态物体
class s_static_object : public s_mesh_object
{

};

//动态物体
class s_dynamic_object : public s_mesh_object
{

};

//相机
class s_camera : public s_object
{
public:
	//需不需要绘制
	bool is_recording = true;
};

//灯光
class s_light : public s_object
{

};

//材质
class s_material : public s_resource
{
	s_texture* textrue_group;
};

//贴图
class s_texture : public s_resource
{
	
};

//场景
class s_sence : public s_resource
{
private://???
	s_object* empty_object_group;
	s_static_object* static_object_group;
	s_dynamic_object* dynamic_object_group;
	s_camera* camera_group;
	s_light* light_group;

};