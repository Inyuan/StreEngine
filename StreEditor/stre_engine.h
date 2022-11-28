#pragma once
#include <map>
#include "stre_render.h"

using std::map;
using std::string;

class stre_engine
{
public :

	static stre_engine* get_instance()
	{
		//程序结束时才自动释放
		static stre_engine* instance = new stre_engine();

		return instance;
	}

protected:

	resource_manager_factory* res_manager_fy = nullptr;
	pass_factory* pass_fy = nullptr;

	s_render_system* render_system_instance = nullptr;



	s_mesh_manager* mesh_manager = nullptr;
	s_texture_manager* texture_manager = nullptr;
	s_material_manager* material_manager = nullptr;

	s_custom_manager<cpu_vertex>* vertex_manager = nullptr;
	s_custom_manager<cpu_index>* index_manager = nullptr;
	s_custom_manager<cpu_camera>* camera_manager = nullptr;
	s_custom_manager<cpu_light>* light_manager = nullptr;



protected:
	stre_engine() 
	{
	
		pass_fy = new pass_factory();

		res_manager_fy = new resource_manager_factory();
		mesh_manager = res_manager_fy->create_mesh_manager();
		texture_manager = res_manager_fy->create_texture_manager();
		material_manager = res_manager_fy->create_material_manager();

		vertex_manager = res_manager_fy->create_manager<cpu_vertex>();
		index_manager = res_manager_fy->create_manager<cpu_index>();
		camera_manager = res_manager_fy->create_manager<cpu_camera>();
		light_manager = res_manager_fy->create_manager<cpu_light>();
	
	};

	//不许复制
	stre_engine(const stre_engine&) {};
	//不许赋值
	stre_engine& operator=(const stre_engine&) {};
	~stre_engine()
	{

	};


public:

	void render_system_init(HWND in_HWND, UINT in_width, UINT in_height);

	void update_gpu_memory();

	void draw_pass(s_pass* in_pass);

	void execute_command();
	
	//一大堆被调用的功能函数 

	cpu_texture* create_texture(gpu_shader_resource::SHADER_RESOURCE_TYPE in_sr_type);

	cpu_mesh* create_viewport_mesh();

	cpu_mesh* create_mesh_from_fbx(std::wstring path);

	s_pass* create_pass();

	template<class t_cpu_resource>
	bool pass_add_shader_resource(s_pass* in_out_pass,const t_cpu_resource * in_sr)
	{
		return pass_fy->add_shader_resource<t_cpu_resource>(in_out_pass, in_sr);
	}

	template<class t_cpu_resource>
	bool pass_remove_shader_resource(s_pass* in_out_pass, const t_cpu_resource* in_sr)
	{
		return pass_fy->remove_shader_resource<t_cpu_resource>(in_out_pass, in_sr);
	}

	bool pass_add_render_target(s_pass* in_out_pass, cpu_texture* in_texture);

	bool pass_remove_render_target(s_pass* in_out_pass, cpu_texture* in_texture);

	bool pass_add_mesh(s_pass* in_out_pass, cpu_mesh* in_mesh);

	bool pass_remove_mesh(s_pass* in_out_pass, cpu_mesh* in_mesh);
	
	bool pass_set_shader_layout(s_pass* in_out_pass, shader_layout& in_shader_layout);

	bool pass_remove_shader_layout(s_pass* in_out_pass);


	void package_textures(std::vector<cpu_texture*> in_textures, cpu_texture* in_out_package_container);

	bool allocate_pass(s_pass* in_out_pass);

	bool check_pass(s_pass* in_out_pass);

};