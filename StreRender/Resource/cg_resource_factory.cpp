#include "cg_resource_factory.h"

s_memory_allocater_register resoucre_memory_allocater("resoucre_memory_allocater");

cg_mesh_object::cpu_mesh_data* cg_resource_factory::allocate_mesh(
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
		////为后面的指针留储存空间
		//sizeof(cg_mesh_object::cpu_mesh_data::material_index_offset_group_ptr) +
		//sizeof(cg_mesh_object::cpu_mesh_data::vertex_group_ptr) +
		//sizeof(cg_mesh_object::cpu_mesh_data::index_group_ptr) +
		sizeof(cg_mesh_object::cpu_mesh_data) + 
		in_material_numder * sizeof(cg_material*) +
		in_material_numder * sizeof(std::uint32_t) +
		in_vertex_number * sizeof(s_vertex) +
		in_index_number * sizeof(s_index));

	cg_mesh_object::cpu_mesh_data* result = (cg_mesh_object::cpu_mesh_data*)in_resource->data_ptr;

	result->vertex_group_size = in_vertex_number;
	result->index_group_size = in_index_number;
	result->material_size = in_material_numder;
	//修正指针
	return read_resource_as_mesh(in_resource);
}


//自己是头部
cg_mesh_object::cpu_mesh_data* cg_resource_factory::read_resource_as_mesh(cg_resource* in_resource)
{
	cg_mesh_object::cpu_mesh_data* mesh_data = (cg_mesh_object::cpu_mesh_data*)in_resource->data_ptr;

	//std::uint32_t* size_data_offset = (std::uint32_t*)(in_resource->data_ptr);
	////获取顶点数量
	//mesh_data->vertex_group_size = *size_data_offset;
	//size_data_offset += 1;
	////获取索引数量
	//mesh_data->index_group_size = *size_data_offset;
	//size_data_offset += 1;
	////获取材质数量
	//mesh_data->material_size = *size_data_offset;
	//size_data_offset += 1;
	////物体常量
	//cg_mesh_object::cpu_object_data* object_data_offset = (cg_mesh_object::cpu_object_data*)size_data_offset;
	//mesh_data->object_constant_data = *object_data_offset;
	//object_data_offset += 1;

	//材质指针
	cg_material* material_group_offset = (cg_material*)(mesh_data + 1);
	mesh_data->material_group_ptr = material_group_offset;
	material_group_offset+= mesh_data->material_size;

	//材质偏移数组
	std::uint32_t* material_index_offset_group_offset = (std::uint32_t*)material_group_offset;
	mesh_data->material_index_offset_group_ptr = material_index_offset_group_offset;
	material_index_offset_group_offset += mesh_data->material_size;

	//顶点
	s_vertex* vertex_data_offset = (s_vertex*)material_index_offset_group_offset;
	mesh_data->vertex_group_ptr = vertex_data_offset;
	vertex_data_offset += mesh_data->vertex_group_size;

	//索引
	s_index* index_data_offset = (s_index*)vertex_data_offset;
	mesh_data->index_group_ptr = index_data_offset;

	return mesh_data;
}
