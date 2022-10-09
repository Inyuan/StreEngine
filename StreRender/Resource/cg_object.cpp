#include "cg_object.h"

//�����resource������
//�Լ������GPU��Դ����

void calculate_element_group(
	std::uint32_t* const in_mat_index_offset_group_ptr,
	std::uint32_t const in_material_size,
	std::vector<UINT>& out_elem_group_number)
{
	out_elem_group_number.assign(in_material_size, 0);
	for(std::uint32_t i = 0; i < in_material_size;i++)
	{
		out_elem_group_number[i] = in_mat_index_offset_group_ptr[i];
	}
}

cg_object::cg_object(cg_resource* in_resource)
{
	resource = in_resource;
	object_data = (cpu_object_data*)in_resource->data_ptr;
	object_type = SENCE_OBJECT_TYPE::OBJ_EMPTY_OBJECT;
	resource->resource_type = s_resource::RESOURCE_TYPE::RES_EMPTY_OBJECT;

	//�߿�vertex������

	//objectcb
	{
		resource->resource_gpu_layout[GPU_RESOURCE_LAYOUT::GPU_RESOURCE_TYPE::GPU_RES_BUFFER]
			.push_back(GPU_RESOURCE_LAYOUT(
				"objectCB",
				reinterpret_cast<void**>(&(object_data)),
				sizeof(cpu_object_data),
				1,
				{},
				GPU_RESOURCE_LAYOUT::GPU_RESOURCE_TYPE::GPU_RES_BUFFER,
				GPU_RESOURCE_LAYOUT::GPU_RESOURCE_STATE::GPU_RES_UPLOAD));
	}
}


//shader input vertex
//shader input index (need offset) 
// mesh b0-> objcb buffer0 Ϊ obj cb
// mesh t0-> mat  buffer1 Ϊ mat
// mesh t1-> obj texture
cg_static_object::cg_static_object(cg_resource* in_resource)
{
	resource = in_resource;
	static_mesh_data = (cpu_static_mesh_data*)in_resource->data_ptr;
	object_type = SENCE_OBJECT_TYPE::OBJ_STATIC_OBJECT;
	resource->resource_type = s_resource::RESOURCE_TYPE::RES_STATIC_OBJECT;

	//vertex
	{
		resource->resource_gpu_layout[GPU_RESOURCE_LAYOUT::GPU_RESOURCE_TYPE::GPU_RES_VERTEX]
			.push_back(GPU_RESOURCE_LAYOUT(
				"vertex",
				reinterpret_cast<void**>(&(static_mesh_data->vertex_group_ptr)),
				sizeof(s_vertex),
				static_mesh_data->vertex_group_size,
				{},
				GPU_RESOURCE_LAYOUT::GPU_RESOURCE_TYPE::GPU_RES_VERTEX,
				GPU_RESOURCE_LAYOUT::GPU_RESOURCE_STATE::GPU_RES_CONSTANT));
	}
	//index
	{
		std::vector<UINT> index_offset;
		calculate_element_group(
			static_mesh_data->material_index_offset_group_ptr,
			static_mesh_data->material_size,
			index_offset);


		resource->resource_gpu_layout[GPU_RESOURCE_LAYOUT::GPU_RESOURCE_TYPE::GPU_RES_INDEX]
			.push_back(GPU_RESOURCE_LAYOUT(
				"index",
				reinterpret_cast<void**>(&(static_mesh_data->index_group_ptr)),
				sizeof(s_index),
				static_mesh_data->index_group_size,
				index_offset,
				GPU_RESOURCE_LAYOUT::GPU_RESOURCE_TYPE::GPU_RES_INDEX,
				GPU_RESOURCE_LAYOUT::GPU_RESOURCE_STATE::GPU_RES_CONSTANT));
	}
	//objectcb
	{
		resource->resource_gpu_layout[GPU_RESOURCE_LAYOUT::GPU_RESOURCE_TYPE::GPU_RES_BUFFER]
			.push_back(GPU_RESOURCE_LAYOUT(
				"objectCB",
				reinterpret_cast<void**>(&(static_mesh_data->object_constant_data)),
				sizeof(static_mesh_data->object_constant_data),
				1,
				{},
				GPU_RESOURCE_LAYOUT::GPU_RESOURCE_TYPE::GPU_RES_BUFFER,
				GPU_RESOURCE_LAYOUT::GPU_RESOURCE_STATE::GPU_RES_UPLOAD));
	}

	//����mat group & ???Ĭ��texture 
	{
		
		std::vector<cg_texture*> texture_group;
		
		auto gpu_mat_group = static_mesh_data->material_data_buffer_ptr;

		for (uint32_t i = 0; i < static_mesh_data->material_size; i++)
		{
			
			auto mat = static_mesh_data->material_group_ptr[i];
			auto textures = mat->material_data->textrue;
			
			for (int i = 0; i < mat->material_data->textrue_group_size;i++)
			{
				auto texture = textures[i];
				bool had_texture = false;
				for (uint32_t j = 0; j < texture_group.size(); j++)
				{
					if (texture_group[j] == texture)
					{
						had_texture = true;
						break;

					}
				}
				if (!had_texture)
				{
					texture_group.push_back(texture);
					//���¶�Ӧ��ͼ������
					gpu_mat_group[i].mat_index = texture_group.size() - 1;

					//����ֱ�Ӿ���ģ�ʹ�����ͼ ֻ��Ĭ����ͼ
					resource->resource_gpu_layout[GPU_RESOURCE_LAYOUT::GPU_RESOURCE_TYPE::GPU_RES_TEXTURE]
						.push_back(GPU_RESOURCE_LAYOUT(
							"texture",
							reinterpret_cast<void**>(&(texture->texture_data->data)),
							sizeof(texture->texture_data->width * texture->texture_data->height),
							1,
							{},
							GPU_RESOURCE_LAYOUT::GPU_RESOURCE_TYPE::GPU_RES_TEXTURE,
							GPU_RESOURCE_LAYOUT::GPU_RESOURCE_STATE::GPU_RES_CONSTANT));//???
				}
			}
		}
		//����ֱ�Ӿ���ģ�ʹ������
		resource->resource_gpu_layout[GPU_RESOURCE_LAYOUT::GPU_RESOURCE_TYPE::GPU_RES_BUFFER]
			.push_back(GPU_RESOURCE_LAYOUT(
				"material_group",
				reinterpret_cast<void**>(&(static_mesh_data->material_data_buffer_ptr)),
				sizeof(gpu_material_data),
				static_mesh_data->material_size,
				{},
				GPU_RESOURCE_LAYOUT::GPU_RESOURCE_TYPE::GPU_RES_BUFFER,
				GPU_RESOURCE_LAYOUT::GPU_RESOURCE_STATE::GPU_RES_UPLOAD));//???

	}
	//û����ͼ ��Ҫ��ˢ�µ���ͼ����
	//...
}


//dynamic_object

cg_dynamic_object::cg_dynamic_object(cg_resource* in_resource)
{
	resource = in_resource;
	dynamic_mesh_data = (cpu_dynamic_mesh_data*)in_resource->data_ptr;
	object_type = SENCE_OBJECT_TYPE::OBJ_DYNAMIC_OBJECT;
	resource->resource_type = s_resource::RESOURCE_TYPE::RES_DYNAMIC_OBJECT;

	//...
}


//camera 

cg_camera::cg_camera(cg_resource* in_resource)
{
	resource = in_resource;
	camera_data = (cpu_camera_data*)in_resource->data_ptr;
	object_type = SENCE_OBJECT_TYPE::OBJ_CAMERA;
	resource->resource_type = s_resource::RESOURCE_TYPE::RES_CAMERA;
	
	//������������ڳ�����һ���ϴ�
}

//light

cg_light::cg_light(cg_resource* in_resource)
{
	resource = in_resource;
	light_data = (cpu_light_data*)in_resource->data_ptr;
	object_type = SENCE_OBJECT_TYPE::OBJ_LIGHT;
	resource->resource_type = s_resource::RESOURCE_TYPE::RES_LIGHT;

	//�Ʊ������ڳ�����һ���ϴ���GPU
}

//material

cg_material::cg_material(cg_resource* in_resource)
{
	resource = in_resource;
	material_data = (cpu_material_data*)in_resource->data_ptr;
	resource->resource_type = s_resource::RESOURCE_TYPE::RES_MATERIAL;

	//���ʱ�������ģ����һ���ϴ���GPU
}

//texture

cg_texture::cg_texture(cg_resource* in_resource)
{
	resource = in_resource;
	texture_data = (cpu_texture_data*)in_resource->data_ptr;
	resource->resource_type = s_resource::RESOURCE_TYPE::RES_TEXTURE;

	{//texture ??? Ҳ�ܰ�������·�ϴ���
		resource->resource_gpu_layout[GPU_RESOURCE_LAYOUT::GPU_RESOURCE_TYPE::GPU_RES_TEXTURE]
			.push_back(GPU_RESOURCE_LAYOUT(
				"texture",
				reinterpret_cast<void**>(&(texture_data->data)),
				sizeof(texture_data->width * texture_data->height),
				1,
				{},
				GPU_RESOURCE_LAYOUT::GPU_RESOURCE_TYPE::GPU_RES_TEXTURE,
				GPU_RESOURCE_LAYOUT::GPU_RESOURCE_STATE::GPU_RES_CONSTANT));//??? �����ǳ���
	}
}

cg_sence::cg_sence(cg_resource* in_resource)
{
	resource = in_resource;
	sence_data = (cpu_sence_data*)in_resource->data_ptr;
	resource->resource_type = s_resource::RESOURCE_TYPE::RES_SENCE;

	//camera
	{
		resource->resource_gpu_layout[GPU_RESOURCE_LAYOUT::GPU_RESOURCE_TYPE::GPU_RES_BUFFER]
			.push_back(GPU_RESOURCE_LAYOUT(
				"cameraCB",
				reinterpret_cast<void**>(&(sence_data->cpu_camera_ptr)),
				sizeof(cg_camera::cpu_camera_data),
				1,
				{},
				GPU_RESOURCE_LAYOUT::GPU_RESOURCE_TYPE::GPU_RES_BUFFER,
				GPU_RESOURCE_LAYOUT::GPU_RESOURCE_STATE::GPU_RES_UPLOAD));
	}
	//lights ֱ��������д���˶��ٸ���
	{
		resource->resource_gpu_layout[GPU_RESOURCE_LAYOUT::GPU_RESOURCE_TYPE::GPU_RES_BUFFER]
			.push_back(GPU_RESOURCE_LAYOUT(
				"lights",
				reinterpret_cast<void**>(&(sence_data->cpu_light_group_ptr)),
				sizeof(cg_light::cpu_light_data),
				sence_data->cpu_light_group_size,
				{},
				GPU_RESOURCE_LAYOUT::GPU_RESOURCE_TYPE::GPU_RES_BUFFER,
				GPU_RESOURCE_LAYOUT::GPU_RESOURCE_STATE::GPU_RES_UPLOAD));//???
	}
}