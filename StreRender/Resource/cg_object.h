#include "stre_render.h"
#include "Core/Memory/s_memory.h"
#include<map>
#include<string>
#include<vector>

//����cg_resource �ٹ������� �ٹ���gpu��Դ����


//Ҫ��ÿ���������Ͷ����Լ��Ĺ��캯��
//���ڹ��캯������gpu��Դ����


struct GPU_RESOURCE_LAYOUT
{
	enum GPU_RESOURCE_TYPE
	{
		GPU_RES_VERTEX = 0, //���Ᵽ���С������
		GPU_RES_INDEX = 1,  //���Ᵽ���С������
		GPU_RES_TEXTURE = 2, //�Զ�����ͼgpu_resourceֻ����һ��gpu_resource_element
		GPU_RES_BUFFER = 3,  // mesh ��0 Ϊ objcb  1Ϊ mat
		GPU_RES_RENDER_TARGET = 4,
		GPU_RES_DEPTH_STENCIL = 5,
		GPU_RES_TYPE_NUMBER = 6
	};

	enum GPU_RESOURCE_STATE
	{
		GPU_RES_UPLOAD,
		GPU_RES_CONSTANT
	};

	std::string gpu_resource_name;
	//ָ��cg_resource��data_ptr
	void** cpu_data; 
	// һ��Ԫ�صĴ�С
	UINT cpu_data_size; 
	// Ԫ�ص�����
	UINT cpu_data_number;
	//�����ٸ�Ԫ��Ϊһ��
	std::vector<UINT> element_group_number;

	GPU_RESOURCE_TYPE gpu_resource_type = GPU_RESOURCE_TYPE::GPU_RES_BUFFER;
	GPU_RESOURCE_STATE gpu_resource_state = GPU_RESOURCE_STATE::GPU_RES_CONSTANT;

	GPU_RESOURCE_LAYOUT() = default;

	GPU_RESOURCE_LAYOUT(
		std::string in_name, 
		void **in_cpu_data,
		UINT in_cpu_data_size,
		UINT in_cpu_data_number = 1,
		std::vector<UINT> in_element_group_number = {},
		GPU_RESOURCE_TYPE in_type = GPU_RESOURCE_TYPE::GPU_RES_BUFFER,
		GPU_RESOURCE_STATE in_state = GPU_RESOURCE_STATE::GPU_RES_CONSTANT)
		: gpu_resource_name(in_name),
		cpu_data(in_cpu_data),
		cpu_data_size(in_cpu_data_size),
		cpu_data_number(in_cpu_data_number),
		element_group_number(in_element_group_number),
		gpu_resource_type(in_type),
		gpu_resource_state(in_state)
	{};

	//??? =
};


//��Դ
class cg_resource : public s_resource
{
public:
	//���ڹ��� ����Դ gpu_resource_element
	std::vector<GPU_RESOURCE_LAYOUT> resource_gpu_layout[GPU_RESOURCE_LAYOUT::GPU_RESOURCE_TYPE::GPU_RES_TYPE_NUMBER];


public:
	RESOURCE_TYPE resource_type = RESOURCE_TYPE::RES_BUFFER;
	//�Ƿ��ڱ��ش���
	bool is_local = false;

	char local_path[256] = { '\0' };

	void* data_ptr;

	char name[256] = { '\0' };

public:
	virtual bool is_in_local() override { return is_local; } ;
	virtual char* get_path() override { return local_path; };
	virtual char* get_name() override { return name; };
	virtual RESOURCE_TYPE get_type() override { return resource_type; };

};



//////////////////////////////////////////////
////////���廯/////////////////////////////////

//����
class cg_object : public s_object
{

public:
	SENCE_OBJECT_TYPE object_type = SENCE_OBJECT_TYPE::OBJ_EMPTY_OBJECT;
	//�����

	cg_resource* resource;

	struct bound_box
	{
		s_float3 min_position;
		s_float3 max_position;
	};

	struct cpu_object_data
	{
		s_float4x4 world_position = s_float4x4::identity4x4();
		s_float4x4 tex_transform = s_float4x4::identity4x4();
		bound_box object_bound_box;
		s_float material_index = 0;
	};
	
	struct gpu_material_data
	{
		//...
		std::uint32_t mat_index;//??? Ŀǰֻ֧��һ����ͼ
	};


	cpu_object_data* object_data = nullptr;

protected:
	cg_object();

public:
	
	cg_object(cg_resource* in_resource);

	virtual s_resource* get_resource() override { return resource; };

};



//��̬����
class cg_static_object : public s_static_object, cg_object
{
public:

	struct cpu_static_mesh_data
	{
		std::uint32_t vertex_group_size = 0;
		std::uint32_t index_group_size = 0;
		std::uint32_t material_size = 0;
		cpu_object_data object_constant_data;
		cg_material** material_group_ptr; //ָ������
		gpu_material_data* material_data_buffer_ptr = nullptr;//Ҫ����ɸ�����GPU��������ʽ
		std::uint32_t* material_index_offset_group_ptr = nullptr;;//����
		s_vertex* vertex_group_ptr = nullptr;//����
		s_index* index_group_ptr = nullptr;//����
		
	};
	cpu_static_mesh_data* static_mesh_data = nullptr;
public:
	cg_static_object() = delete;

	cg_static_object(cg_resource* in_resource);

	virtual s_resource* get_resource() override { return resource; };

};

//��̬����
class cg_dynamic_object : public s_dynamic_object, cg_object
{
public:

	struct cpu_dynamic_mesh_data
	{
		std::uint32_t vertex_group_size = 0;
		std::uint32_t index_group_size = 0;
		std::uint32_t material_size = 0;
		cpu_object_data object_constant_data;
		cg_material* material_group_ptr;
		std::uint32_t* material_index_offset_group_ptr;
		s_vertex* vertex_group_ptr = nullptr;
		s_index* index_group_ptr = nullptr;
		//animation data
	};

	cpu_dynamic_mesh_data* dynamic_mesh_data = nullptr;

public:
	cg_dynamic_object() = delete;

	cg_dynamic_object(cg_resource* in_resource);

	virtual s_resource* get_resource() override { return resource; };

};

//���
class cg_camera : public s_camera, cg_object
{
public:
	//�費��Ҫ����
	bool is_recording = true;

	cg_resource* resource;

	struct cpu_camera_data
	{
		s_float4x4 View = s_float4x4::identity4x4();
		s_float4x4 InvView = s_float4x4::identity4x4();
		s_float4x4 Proj = s_float4x4::identity4x4();
		s_float4x4 InvProj = s_float4x4::identity4x4();
		s_float4x4 ViewProj = s_float4x4::identity4x4();
		s_float4x4 InvViewProj = s_float4x4::identity4x4();
		s_float4x4 ShadowTransform = s_float4x4::identity4x4();
		s_float3 EyePosW = { 0.0f, 0.0f, 0.0f };
		s_float2 RenderTargetSize = { 0.0f, 0.0f };
		s_float2 InvRenderTargetSize = { 0.0f, 0.0f };
		s_float NearZ = 0.0f;
		s_float FarZ = 0.0f;

		s_float4 AmbientLight = { 0.0f, 0.0f, 0.0f, 1.0f };

	};

	cpu_camera_data * camera_data = nullptr;

public:
	cg_camera(cg_resource* in_resource);


	virtual s_resource* get_resource() override { return resource; };

};

//�ƹ� ���еƹ�Ϊһ��
class cg_light : public s_light, cg_object
{
public:

	cg_resource* resource;
	//ֻ�з���⣿
	struct cpu_light_data
	{
		s_float3 strength = { 10.0f, 10.0f, 10.0f };
		s_float fall_off_start = 1.0f;
		s_float3 Direction = { 0.0f, -1.0f, 0.0f };// directional/spot light only
		s_float fall_off_end = 10.0f;                           // point/spot light only
		s_float3 position = { 0.0f, 0.0f, 0.0f };  // point/spot light only
		s_float spot_power = 64.0f;
	};

	cpu_light_data* light_data = nullptr;

public:
	cg_light(cg_resource* in_resource);

	virtual s_resource* get_resource() override { return resource; };

};

//����
class cg_material : public s_material
{
public:

	cg_resource* resource;

	struct cpu_material_data
	{
		cg_texture* textrue;
	};

	struct gpu_material_data
	{
		std::uint32_t texture_index = 0;
	};

	cpu_material_data* material_data;

public:

	cg_material(cg_resource* in_resource);


	virtual s_resource* get_resource() override { return resource; };

};

//��ͼ
class cg_texture : public s_texture
{
public:

	cg_resource* resource;

	struct cpu_texture_data
	{
		std::uint32_t width = 1;
		std::uint32_t height = 1;
		void* data;
	};
	cpu_texture_data* texture_data;
public:

	cg_texture(cg_resource* in_resource);

	virtual s_resource* get_resource() override { return resource; };

};

//����
class cg_sence : public s_sence
{
private://???
	std::map<std::string, cg_object*> empty_object_group;
	std::map<std::string, cg_static_object*> static_object_group;
	std::map<std::string, cg_dynamic_object*> dynamic_object_group;
	std::map<std::string, cg_camera*> camera_group;
	std::map<std::string, cg_light*> light_group;

public:

	cg_resource* resource;

	struct cpu_sence_data
	{
		s_float TotalTime = 0.0f;
		s_float DeltaTime = 0.0f;
		//����ƺϳ�һ������
		std::uint32_t cpu_light_group_size;
		cg_light* cpu_light_group_ptr = nullptr;
		cg_camera* cpu_camera_ptr = nullptr;
	};
	cpu_sence_data* sence_data;
public:

	cg_sence(cg_resource* in_resource);

	virtual s_resource* get_resource() override { return resource; };

};