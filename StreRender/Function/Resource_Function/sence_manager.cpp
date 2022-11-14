#include "resource_factory.h"

cpu_sence* sence_manager::create_resource()
{
	auto instance = new cpu_sence();
	generate_unique_identifier<cpu_sence>(instance->uid);
	return instance;
}


cpu_sence* sence_manager::load_resource(wchar_t* in_path)
{
	return nullptr;
}


void sence_manager::update_gpu(cpu_sence* in_cpu_data)
{
	auto light_manager = custom_manager<cpu_light>();
	for (auto it : in_cpu_data->light_group)
	{
		light_manager.update_gpu(it);
	}

	auto camera_manager = custom_manager<cpu_camera>();
	for (auto it : in_cpu_data->camera_group)
	{
		camera_manager.update_gpu(it);
	}

	custom_manager<cpu_sence_constant>().update_gpu(in_cpu_data->sence_constant);
}

void sence_manager::dx_allocate_gpu_resource(cpu_sence* in_cpu_data)
{
	auto light_manager = custom_manager<cpu_light>();
	for (auto it : in_cpu_data->light_group)
	{
		light_manager.dx_allocate_gpu_resource(it, gpu_shader_resource::SHADER_RESOURCE_TYPE_CUSTOM_BUFFER);
	}

	auto camera_manager = custom_manager<cpu_camera>();
	for (auto it : in_cpu_data->camera_group)
	{
		camera_manager.dx_allocate_gpu_resource(it,gpu_shader_resource::SHADER_RESOURCE_TYPE_CUSTOM_BUFFER);
	}

	custom_manager<cpu_sence_constant>().dx_allocate_gpu_resource(in_cpu_data->sence_constant, gpu_shader_resource::SHADER_RESOURCE_TYPE_CUSTOM_BUFFER);
}