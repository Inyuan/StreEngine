#pragma once
#include "Core/Memory/s_memory.h"
#include "stre_render.h"
#include "gpu_resource.h"
#include "Function/Render_Function/render_functor.h"
#include "Core/Math/stre_math.h"
template<typename t_cpu_res_type>
struct custom_manager :public s_custom_manager<t_cpu_res_type>
{
	virtual t_cpu_res_type* create_resource() override
	{
		auto instance = new t_cpu_res_type();
		generate_unique_identifier<t_cpu_res_type>(instance->uid);
		return instance;
	}

	virtual t_cpu_res_type* create_resource(size_t in_element_number) override
	{
		t_cpu_res_type* cpu_ptr = create_resource();
		cpu_ptr->data = custom_allocate(in_element_number * sizeof(t_cpu_res_type));
		cpu_ptr->count = in_element_number;

		return cpu_ptr;
	}
	//??? ȱ��ȡ��Դ
	virtual t_cpu_res_type* load_resource(wchar_t* in_path) override
	{
		return nullptr;
	}
	virtual void update_gpu(t_cpu_res_type* in_cpu_data) override
	{

		dx_function sr_functor = [in_cpu_data](s_directx_render* in_render)
		{
			gpu_shader_resource* sr_ptr = in_cpu_data->gpu_sr_ptr;

			in_render->update_gpu_resource(sr_ptr, in_cpu_data->get_data(), 0, sr_ptr->element_count);
		};
		dx_shader_resource_command.command_queue.push(sr_functor);

	}

	virtual void dx_allocate_gpu_resource(t_cpu_res_type* in_cpu_data,
		gpu_shader_resource::SHADER_RESOURCE_TYPE in_sr_type) override
	{
		dx_function sr_functor = [in_cpu_data, in_sr_type](s_directx_render* in_render)
		{
			//����������
			in_cpu_data->gpu_sr_ptr = in_render->allocate_shader_resource(in_sr_type);
			//�����ڴ�
			if (in_cpu_data->can_update)
			{
				in_render->allocate_default_resource(
					in_cpu_data->gpu_sr_ptr,
					in_cpu_data->get_element_size(),
					in_cpu_data->get_element_count(),
					in_cpu_data->data);
			}
			else
			{
				in_render->allocate_upload_resource(
					in_cpu_data->gpu_sr_ptr,
					in_cpu_data->get_element_size(),
					in_cpu_data->get_element_count());
			}
		};

		dx_shader_resource_command.command_queue.push(sr_functor);
	}
};

struct material_manager : public s_material_manager
{
public:
	virtual cpu_material* create_resource() override
	{
		return custom_manager<cpu_material>().create_resource();
	}

	virtual cpu_material* create_resource(size_t in_element_number) override
	{
		return custom_manager<cpu_material>().create_resource(in_element_number);
	}

	//??? ȱ��ȡ��Դ
	virtual cpu_material* load_resource(wchar_t* in_path) override
	{
		return custom_manager<cpu_material>().load_resource(in_path);
	}

	virtual void update_gpu(cpu_material* in_cpu_data) override
	{
		custom_manager<cpu_material>().update_gpu(in_cpu_data);
	}

	virtual void dx_allocate_gpu_resource(cpu_material* in_cpu_data,
		gpu_shader_resource::SHADER_RESOURCE_TYPE in_sr_type) override
	{
		//��������ʶ�������GPU�ڴ�
		//custom_manager<cpu_material>().dx_allocate_gpu_resource(in_cpu_data, in_sr_type);
	}

	//����Ĵ�����
	//�ص��󶨹�ϵ
	virtual void change_texture() override;

	virtual void add_texture() override;

	virtual void release_texture() override;
};

struct texture_manager : public s_texture_manager
{
public:
	virtual cpu_texture* create_resource() override
	{
		return custom_manager<cpu_texture>().create_resource();
	}

	virtual cpu_texture* create_resource(size_t in_element_number) override
	{
		return custom_manager<cpu_texture>().create_resource(in_element_number);
	}

	//??? ȱ��ȡ��Դ
	virtual cpu_texture* load_resource(wchar_t* in_path) override
	{
		return custom_manager<cpu_texture>().load_resource(in_path);
	}

	virtual void update_gpu(cpu_texture* in_cpu_data) override
	{
		custom_manager<cpu_texture>().update_gpu(in_cpu_data);
	}

	virtual void dx_allocate_gpu_resource(cpu_texture* in_cpu_data,
		gpu_shader_resource::SHADER_RESOURCE_TYPE in_sr_type) override
	{
		custom_manager<cpu_texture>().dx_allocate_gpu_resource(in_cpu_data, in_sr_type);
	}

};

struct mesh_manager : public s_mesh_manager
{
public:
	virtual cpu_mesh* create_resource() override;

	//??? ȱ��ȡ��Դ
	virtual cpu_mesh* load_resource(wchar_t* in_path) override;


	virtual void update_gpu(cpu_mesh* in_cpu_data) override;


	virtual cpu_mesh* load_fbx(wchar_t* in_path) override;

	virtual void dx_allocate_gpu_resource(cpu_mesh* in_cpu_data) override;
};



struct sence_manager : public s_sence_manager
{
public:
	virtual cpu_sence* create_resource() override;

	//??? ȱ��ȡ��Դ
	virtual cpu_sence* load_resource(wchar_t* in_path) override;

	virtual void update_gpu(cpu_sence* in_cpu_data) override;

};

