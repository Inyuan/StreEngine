#include "stre_render.h"
#include "Core/Memory/s_memory.h"


s_memory_allocater_register custom_allocater("custom_allocater");

template<typename t_cpu_res_type>
t_cpu_res_type* custom_manager<t_cpu_res_type>::create_resource()
{
	auto allocater = memory_allocater_group["custom_allocater"];

	return allocater->allocate<custom_manager<t_cpu_res_type>>();
}

template<typename t_cpu_res_type>
t_cpu_res_type* custom_manager<t_cpu_res_type>::load_resource(wchar_t* in_path)
{

}

template<typename t_cpu_res_type>
void custom_manager<t_cpu_res_type>::update_gpu(const t_cpu_res_type* in_cpu_data)
{

}

template<typename t_cpu_res_type>
t_cpu_res_type* custom_manager<t_cpu_res_type>::create_resource(size_t in_element_number)
{
	auto allocater = memory_allocater_group["custom_allocater"];

	t_cpu_res_type* cpu_ptr = create_resource();
	cpu_ptr->data = allocater->custom_allocate(in_element_number * sizeof(t_cpu_res_type));
	cpu_ptr->count = in_element_number;

	return cpu_ptr;
}


template<typename t_cpu_res_type>
template<class t_render>
void custom_manager<t_cpu_res_type>::allocate_gpu(
	const t_cpu_res_type* in_cpu_data,
	gpu_shader_resource::SHADER_RESOURCE_TYPE in_sr_type)
{
	auto labdma = [gpu_shader_resource::SHADER_RESOURCE_TYPE](
		t_render* render)
	{
		return render->allocate_shader_resource(in_sr_type);
		//...
	}
	//??? future ÆÚÍû
	in_cpu_data->gpu_sr_ptr = ..;

	render_command<t_render>::command_queue.push();
}