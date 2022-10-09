#pragma once
#include"Core/Memory/s_memory.h"
#include"cg_object.h"

//按规格分配cpu内存
//解析resource cpu数据内存
class cg_resource_factory
{
public:
	
	cg_static_object::cpu_static_mesh_data* read_resource_as_static_mesh(cg_resource* in_resource);
	
	cg_static_object::cpu_static_mesh_data* allocate_static_mesh(
		cg_resource* in_resource,
		std::size_t in_vertex_number,
		std::size_t in_index_number,
		std::size_t in_material_numder);

	cg_dynamic_object::cpu_dynamic_mesh_data* read_resource_as_dynamic_mesh(cg_resource* in_resource);
	
	cg_dynamic_object::cpu_dynamic_mesh_data* allocate_dynamic_mesh(
		cg_resource* in_resource,
		std::size_t in_vertex_number,
		std::size_t in_index_number,
		std::size_t in_material_numder);

	cg_camera::cpu_camera_data* read_resource_as_camera(cg_resource* in_resource);

	cg_camera::cpu_camera_data* allocate_camera(
		cg_resource* in_resource);

	cg_light::cpu_light_data* read_resource_as_light(cg_resource* in_resource);

	cg_light::cpu_light_data* allocate_light(
		cg_resource* in_resource);

	cg_material::cpu_material_data* read_resource_as_material(cg_resource* in_resource);

	cg_material::cpu_material_data* allocate_material(
		cg_resource* in_resource);

	cg_texture::cpu_texture_data* read_resource_as_texture(cg_resource* in_resource);

	cg_texture::cpu_texture_data* allocate_texture(
		cg_resource* in_resource);

};