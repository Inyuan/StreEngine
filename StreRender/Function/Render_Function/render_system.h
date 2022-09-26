#pragma once
#include "Core/Render_Core/render.h"
#include "Core/Memory/s_memory.h"
#include "Core/Process/BProcess.h"
#include "Core/Render_Core/Resource/constant_pass.h"
#include "Core/Render_Core/Resource/gpu_resource.h"
#include "stre_render.h"
#include "Resource/cg_object.h"

#include <map>

class render_system
{
protected:
	s_window* render_window = nullptr;
	render* renderer = nullptr;
	s_memory* memory_allocater = nullptr;
	//BProcess* render_window_process;

	//gpu_resource_factory* gpu_res_factory;

	REDNER_API render_api;

	enum SENCE_ELEMENT_TYPE
	{
		SENCE_CAMERA = 0,
		SENCE_LIGHT = 1,
		SENCE_OBJECT = 2,
		SENCE_ELEMENT_TYPE_NUMBER = 3
	};

	std::vector<gpu_resource*> sence_gpu_resource_group[SENCE_ELEMENT_TYPE::SENCE_ELEMENT_TYPE_NUMBER];

public:

	constant_pass* allocate_pass(constant_pass::pass_layout in_constant_pass_layout);

	gpu_resource* allocate_gpu_memory(cg_resource* in_resource);

	gpu_resource* create_gpu_texture(
		std::string in_gpu_texture_name,
		GPU_RESOURCE_LAYOUT::GPU_RESOURCE_TYPE in_resoure_type);

	void clear_gpu_texture(gpu_resource * in_gpu_resource);

	void init(HINSTANCE in_instance,REDNER_API in_render_api);
	void over();
};