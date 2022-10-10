#pragma once
#include <Windows.h>

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

#define CLIENT_RENDER_API REDNER_API::DIRECTX_RENDER

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


/***
************************************************************
*
* Renderer
*
************************************************************
*/

///rendering type

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




/***
************************************************************
*
* Resource
*
************************************************************
*/

//资源
class s_resource
{
public:
	enum class RESOURCE_TYPE
	{
		RES_BUFFER,
		RES_EMPTY_OBJECT,
		RES_STATIC_OBJECT,
		RES_DYNAMIC_OBJECT,
		RES_CAMERA,
		RES_LIGHT,
		RES_MATERIAL,
		RES_TEXTURE,
		RES_SENCE
	};
public:
	virtual bool is_in_local() = 0;
	virtual char* get_path() = 0;
	virtual char* get_name() = 0;
	virtual RESOURCE_TYPE get_type() = 0;
};

//物体
class s_object 
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

public:
	virtual s_resource* get_resource() = 0;
};

//静态物体
class s_static_object : public s_object
{

};

//动态物体
class s_dynamic_object : public s_object
{

};

//相机
class s_camera : public s_object
{
public:

};

//灯光
class s_light : public s_object
{

};

//材质
class s_material 
{
public:

};

//贴图
class s_texture 
{
public:

};

//场景
class s_sence 
{
public:

};

/***
************************************************************
*
* Resource Manager
*
************************************************************
*/

//资源导入导出器 (内部新建的内存由静态memory_allocater管理)
// 管理场景的资源加入和删除
// 资源的CPU和GPU内存管理
class s_material_manager
{
public:
	//物体需要资源来构建
	s_material* create_material(s_resource* in_resource);

	//??? 缺默认的物体构建
	s_resource* create_material_resource();

	//??? 缺读取资源
	s_resource* load_local_resource(wchar_t* in_path);

	//回调绑定关系
	void change_texture();

	void add_texture();

	void release_texture();
};

class s_texture_manager
{
public:
	//物体需要资源来构建
	s_texture* create_texture(s_resource* in_resource);

	//??? 缺默认的物体构建
	s_resource* create_texture_resource();

	//??? 缺读取资源
	s_resource* load_local_resource(wchar_t* in_path);
};


class s_object_manager
{
public:
	//物体需要资源来构建
	s_object* create_object(s_resource* in_resource);

	s_static_object* create_static_object(s_resource* in_resource);

	s_dynamic_object* create_dynamic_object(s_resource* in_resource);

	//??? 缺默认的物体构建
	s_resource* create_object_resource();

	//??? 缺读取资源
	s_resource* load_local_resource(wchar_t* in_path);

	//fbx导入物体 ??? 缺动画
	s_resource* load_local_fbx(wchar_t* in_path);
};

class s_sence_manager
{
public:
	//物体需要资源来构建
	s_camera* create_camera(s_resource* in_resource);

	s_light* create_light(s_resource* in_resource);

	s_sence* create_sence(s_resource* in_resource);

	//??? 缺默认的物体构建
	s_resource* create_camera_resource();

	s_resource* create_light_resource();

	s_resource* create_sence_resource();

	//??? 缺读取资源
	s_resource* load_local_resource(wchar_t* in_path);

};


/***
************************************************************
*
* Base Type
*
************************************************************
*/

///base type

struct s_float
{
	float x = 0;
	s_float(float in_x = 0) : x(in_x) {};
};
struct s_float2
{
	float x = 0;
	float y = 0;
	s_float2(float in_x = 0, float in_y = 0) : x(in_x), y(in_y) {};
};
struct s_float3
{
	float x = 0;
	float y = 0;
	float z = 0;
	s_float3(float in_x = 0, float in_y = 0, float in_z = 0) : x(in_x), y(in_y),z(in_z) {};

};

struct s_float4
{
	float x = 0;
	float y = 0;
	float z = 0;
	float w = 0;
	s_float4(float in_x = 0, float in_y = 0, float in_z = 0, float in_w = 0) : x(in_x), y(in_y), z(in_z),w(in_w) {};
	
};

struct s_float4x4
{
	union
	{
		struct
		{
			float _11, _12, _13, _14;
			float _21, _22, _23, _24;
			float _31, _32, _33, _34;
			float _41, _42, _43, _44;
		};
		float m[4][4];
	};

	static s_float4x4 identity4x4()
	{
		static s_float4x4 I(
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f);

		return I;
	}

	s_float4x4(float m00, float m01, float m02, float m03,
		float m10, float m11, float m12, float m13,
		float m20, float m21, float m22, float m23,
		float m30, float m31, float m32, float m33)
		: _11(m00), _12(m01), _13(m02), _14(m03),
		_21(m10), _22(m11), _23(m12), _24(m13),
		_31(m20), _32(m21), _33(m22), _34(m23),
		_41(m30), _42(m31), _43(m32), _44(m33) {}
};



struct s_vertex
{
	s_float3 position{ 0,0,0 };
	s_float3 normal{ 0,0,0 };
	s_float2 texC{ 0,0 };
	s_float3 tangentU{ 0,0,0 };

	s_vertex()
	{
		position = { 0,0,0 };
		normal = { 0,0,0 };
		texC = { 0,0 };
		tangentU = { 0,0,0 };
	}

	s_vertex(
		const s_float3& p,
		const s_float3& n,
		const s_float3& t,
		const s_float2& uv,
		const s_float2& mid) :
		position(p),
		normal(n),
		tangentU(t),
		texC(uv){}
	s_vertex(
		float px, float py, float pz,
		float nx, float ny, float nz,
		float tx, float ty, float tz,
		float u, float v,
		int id) :
		position(px, py, pz),
		normal(nx, ny, nz),
		tangentU(tx, ty, tz),
		texC(u, v){}

};


typedef std::uint32_t s_index;

