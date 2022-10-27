#include "stre_render.h"
#include "Core/Memory/s_memory.h"

/***
************************************************************
*
* Create & Load Function
*
************************************************************
*/


s_memory_allocater_register custom_allocater("custom_allocater");

template<typename t_cpu_res_type, class t_render>
t_cpu_res_type* custom_manager<t_cpu_res_type,t_render>::create_resource()
{
	auto allocater = memory_allocater_group["custom_allocater"];

	return allocater->allocate<custom_manager<t_cpu_res_type>>();
}


template<typename t_cpu_res_type, class t_render>
t_cpu_res_type* custom_manager<t_cpu_res_type, t_render>::create_resource(size_t in_element_number)
{
	auto allocater = memory_allocater_group["custom_allocater"];

	t_cpu_res_type* cpu_ptr = create_resource();
	cpu_ptr->data = allocater->custom_allocate(in_element_number * sizeof(t_cpu_res_type));
	cpu_ptr->count = in_element_number;

	return cpu_ptr;
}

//只复制内存，不构建描述符和描述符堆
template<typename t_cpu_res_type, class t_render>
void custom_manager<t_cpu_res_type, t_render>::allocate_gpu(
	t_cpu_res_type* in_cpu_data,
	gpu_shader_resource::SHADER_RESOURCE_TYPE in_sr_type,
	std::vector<UINT>& elem_group_number)
{

	dx_shader_resource_function sr_functor =
		[in_cpu_data, elem_group_number, in_sr_type]
	(directx_render* in_render)
	{
		in_cpu_data->gpu_sr_ptr = in_render->allocate_shader_resource(in_sr_type);

		if (in_cpu_data->can_update)
		{
			in_render->allocate_default_resource(
				in_cpu_data->gpu_sr_ptr,
				in_cpu_data->get_element_size(),
				in_cpu_data->get_element_count(),
				in_cpu_data->data,
				elem_group_number);
		}
		else
		{
			in_render->allocate_upload_resource(
				in_cpu_data->gpu_sr_ptr,
				in_cpu_data->get_element_size(),
				in_cpu_data->get_element_count(),
				elem_group_number);
		}
	};
}

//void directx_render::update_elem_upload_resource(
//	void* data,
//	int element_index,
//	directx_shader_resource* in_res_elem)
//{
//	memcpy(&(in_res_elem->mapped_data[element_index * in_res_elem->element_byte_size]), &data, in_res_elem->element_byte_size);
//}


template<typename t_cpu_res_type, class t_render>
t_cpu_res_type* custom_manager<t_cpu_res_type,t_render>::load_resource(wchar_t* in_path)
{
	
}


/***
************************************************************
*
* Update Function
*
************************************************************
*/

template<typename t_cpu_res_type, class t_render>
void custom_manager<t_cpu_res_type, t_render>::update_gpu(const t_cpu_res_type* in_cpu_data)
{
	gpu_shader_resource* sr_ptr = in_cpu_data->gpu_sr_ptr;
	size_t memory_size = sr_ptr->element_size * sr_ptr->element_count;

	memcpy(&(sr_ptr->mapped_data[0]), in_cpu_data->get_data(), memory_size);
}
