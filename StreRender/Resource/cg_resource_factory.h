#pragma once
#include "stre_render.h"
#include"Core/Memory/s_memory.h"
#include"cg_object.h"

//按规格分配cpu内存
//解析resource cpu数据内存
class cg_resource_factory
{
public:
	
	cg_mesh_object::cpu_mesh_data* read_resource_as_mesh(cg_resource* in_resource);
	
	cg_mesh_object::cpu_mesh_data* allocate_mesh(
		cg_resource* in_resource,
		std::size_t in_vertex_number,
		std::size_t in_index_number,
		std::size_t in_material_numder);




};