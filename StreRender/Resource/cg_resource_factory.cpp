#include "cg_resource_factory.h"

s_memory_allocater_register resoucre_memory_allocater("resoucre_memory_allocater");

cg_static_object::cpu_static_mesh_data* cg_resource_factory::allocate_static_mesh(
	cg_resource* in_resource, 
	std::size_t in_vertex_number,
	std::size_t in_index_number,
	std::size_t in_material_numder)
{
	auto allocater = memory_allocater_group["resoucre_memory_allocater"];

	in_resource->data_ptr = allocater->custom_allocate(
		//sizeof(cg_mesh_object::cpu_mesh_data::vertex_group_size) +
		//sizeof(cg_mesh_object::cpu_mesh_data::index_group_size) +
		//sizeof(cg_mesh_object::cpu_mesh_data::material_size) +
		//sizeof(cg_mesh_object::cpu_mesh_data::object_constant_data) +
		////Ϊ�����ָ��������ռ�
		//sizeof(cg_mesh_object::cpu_mesh_data::material_index_offset_group_ptr) +
		//sizeof(cg_mesh_object::cpu_mesh_data::vertex_group_ptr) +
		//sizeof(cg_mesh_object::cpu_mesh_data::index_group_ptr) +
		sizeof(cg_static_object::cpu_static_mesh_data) +
		in_material_numder * sizeof(cg_material*) +
		in_material_numder * sizeof(cg_object::gpu_material_data) +
		in_material_numder * sizeof(std::uint32_t) +
		in_vertex_number * sizeof(s_vertex) +
		in_index_number * sizeof(s_index));

	cg_static_object::cpu_static_mesh_data* result = (cg_static_object::cpu_static_mesh_data*)in_resource->data_ptr;

	result->vertex_group_size = in_vertex_number;
	result->index_group_size = in_index_number;
	result->material_size = in_material_numder;
	//����ָ��
	return read_resource_as_static_mesh(in_resource);
}


//�Լ���ͷ��
cg_static_object::cpu_static_mesh_data* cg_resource_factory::read_resource_as_static_mesh(cg_resource* in_resource)
{
	cg_static_object::cpu_static_mesh_data* mesh_data = (cg_static_object::cpu_static_mesh_data*)in_resource->data_ptr;

	//std::uint32_t* size_data_offset = (std::uint32_t*)(in_resource->data_ptr);
	////��ȡ��������
	//mesh_data->vertex_group_size = *size_data_offset;
	//size_data_offset += 1;
	////��ȡ��������
	//mesh_data->index_group_size = *size_data_offset;
	//size_data_offset += 1;
	////��ȡ��������
	//mesh_data->material_size = *size_data_offset;
	//size_data_offset += 1;
	////���峣��
	//cg_mesh_object::cpu_object_data* object_data_offset = (cg_mesh_object::cpu_object_data*)size_data_offset;
	//mesh_data->object_constant_data = *object_data_offset;
	//object_data_offset += 1;

	//����ָ������
	cg_material** material_group_offset = (cg_material**)(mesh_data + 1);
	mesh_data->material_group_ptr = material_group_offset;
	material_group_offset+= mesh_data->material_size;

	//GPU��������
	cg_object::gpu_material_data* gpu_mat_group_offset = (cg_object::gpu_material_data*)material_group_offset;
	mesh_data->material_data_buffer_ptr = gpu_mat_group_offset;
	gpu_mat_group_offset += mesh_data->material_size;

	//����ƫ������
	std::uint32_t* material_index_offset_group_offset = (std::uint32_t*)gpu_mat_group_offset;
	mesh_data->material_index_offset_group_ptr = material_index_offset_group_offset;
	material_index_offset_group_offset += mesh_data->material_size;

	//����
	s_vertex* vertex_data_offset = (s_vertex*)material_index_offset_group_offset;
	mesh_data->vertex_group_ptr = vertex_data_offset;
	vertex_data_offset += mesh_data->vertex_group_size;

	//����
	s_index* index_data_offset = (s_index*)vertex_data_offset;
	mesh_data->index_group_ptr = index_data_offset;

	return mesh_data;
}

//dynamic_mesh

cg_dynamic_object::cpu_dynamic_mesh_data* cg_resource_factory::read_resource_as_dynamic_mesh(cg_resource* in_resource)
{

}

cg_dynamic_object::cpu_dynamic_mesh_data* cg_resource_factory::allocate_dynamic_mesh(
	cg_resource* in_resource,
	std::size_t in_vertex_number,
	std::size_t in_index_number,
	std::size_t in_material_numder) 
{

}

//camera_data


cg_camera::cpu_camera_data* cg_resource_factory::read_resource_as_camera(cg_resource* in_resource)
{

}


cg_camera::cpu_camera_data* cg_resource_factory::allocate_camera(
	cg_resource* in_resource)
{

}

//light_data

cg_light::cpu_light_data* cg_resource_factory::read_resource_as_light(cg_resource* in_resource)
{

}


cg_light::cpu_light_data* cg_resource_factory::allocate_light(
	cg_resource* in_resource)
{

}

//material_data

cg_material::cpu_material_data* cg_resource_factory::read_resource_as_material(cg_resource* in_resource)
{

}


cg_material::cpu_material_data* cg_resource_factory::allocate_material(
	cg_resource* in_resource)
{

}

//texture_data

cg_texture::cpu_texture_data* cg_resource_factory::read_resource_as_texture(cg_resource* in_resource)
{

}


cg_texture::cpu_texture_data* cg_resource_factory::allocate_texture(
	cg_resource* in_resource)
{

}
