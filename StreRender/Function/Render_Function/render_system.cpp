#include "render_system.h"
#include "Core/Render_Core/directx_render.h"
s_memory_allocater_register render_memory_allocater("render_memory_allocater");

void render_system::init(HINSTANCE in_instance, REDNER_API in_render_api)
{
	memory_allocater = memory_allocater_group["render_memory_allocater"];

	//gpu_res_factory = memory_allocater->allocate<gpu_resource_factory>();

	render_window = memory_allocater->allocate<s_window>();

	render_window->init(in_instance, CLIENT_WIDTH, CLIENT_HEIGHT);

	switch (in_render_api)
	{
	case DIRECTX_RENDER:
		renderer = (render*)memory_allocater->allocate<directx_render>();
		renderer->init(render_window->get_hwnd());
		break;
	default://DirectX
		renderer = (render*)memory_allocater->allocate<directx_render>();
		renderer->init(render_window->get_hwnd());
		break;
	}
}


//ʹ�ûص�����??? �з���ֵ�����Ƕ��߳��첽��ȡ
constant_pass* render_system::allocate_pass(constant_pass::pass_layout in_constant_pass_layout)
{
	return renderer->allocate_pass(in_constant_pass_layout);
}



//ʹ�ûص�����??? �з���ֵ�����Ƕ��߳��첽��ȡ
//����GPU_RESOURCE_TYPE
//�������CPU��Դ���Ƶ�GPU��
//ÿ��type���Լ�����Դ����
gpu_resource* render_system::allocate_gpu_memory(cg_resource* in_resource)
{
	return renderer->allocate_gpu_resource(in_resource);
}

//������ RT ����Depth
gpu_resource* render_system::create_gpu_texture(
	std::string in_gpu_texture_name, 
	GPU_RESOURCE_LAYOUT::GPU_RESOURCE_TYPE in_resoure_type)
{
	typedef GPU_RESOURCE_LAYOUT::GPU_RESOURCE_TYPE GPU_RES_TYPE;

	switch (in_resoure_type)
	{
	case GPU_RES_TYPE::GPU_RES_RENDER_TARGET:
	case GPU_RES_TYPE::GPU_RES_DEPTH_STENCIL:

		auto allocater = memory_allocater_group["gpu_resource_ptr_allocater"];

		gpu_resource* gpu_resource_ptr = (gpu_resource*)allocater->allocate<gpu_resource>();
		gpu_resource_ptr->name = in_gpu_texture_name;

		gpu_resource_ptr->gpu_resource_group[in_resoure_type].push_back(renderer->create_gpu_texture(in_gpu_texture_name, in_resoure_type));
		return gpu_resource_ptr;
		break;
	}

	return nullptr;

}

//��������ˢ�º�����ʹ�ûص�����
void render_system::add_gpu_update_process(
	cg_resource* in_cpu_resource,
	gpu_resource* in_gpu_resource,
	update_gpu_res_callback& in_update_gpu_res_callback_function)//??? ��֪���ǲ�������
{
	//ʹ��ʾ������������ĸ�����Դ����λ��д
	//auto u_gpu_res_cb = [](cg_resource* in_resource, gpu_resource* in_out_gpu_resouce,render* in_renderer)
	//{
	//	typedef GPU_RESOURCE_LAYOUT::GPU_RESOURCE_STATE GPU_RES_STATE;
	//	//�����Ǳ��������ڲ���ˢ�� 
	//	auto gpu_resource_ptr = (directx_gpu_resource*)in_out_gpu_resouce;
	//	for (int i = 0; i < GPU_RESOURCE_LAYOUT::GPU_RESOURCE_TYPE::GPU_RES_TYPE_NUMBER; i++)
	//	{
	//		for (int j = 0; j < in_resource->resource_gpu_layout[i].size(); j++)
	//		{
	//			auto cpu_elem = in_resource->resource_gpu_layout[i][j];
	//			if (cpu_elem.need_update)
	//			{
	//				auto gpu_res_type = in_resource->resource_gpu_layout[i][j].gpu_resource_type;
	//				auto gpu_elem = gpu_resource_ptr->gpu_resource_group[gpu_res_type][j];
	//				//����һ���µ���Դ
	//				//�����ǳ�������
	//				if (&cpu_elem != gpu_elem->cpu_resource_layout
	//					|| cpu_elem.gpu_resource_state == GPU_RES_STATE::GPU_RES_CONSTANT)
	//				{
	//					//??? !!!... ж�ؾ���Դ
	//					//���·��� 
	//					//gpu_elem = allocate_gpu_memory(cpu_elem);
	//				}
	//				//�ϴ��Ѹ���
	//				else if (cpu_elem.gpu_resource_state == GPU_RES_STATE::GPU_RES_UPLOAD)
	//				{
	//					in_renderer->update_gpu_memory(cpu_elem, gpu_elem);
	//				}
	//				cpu_elem.need_update = false;
	//			}
	//		}
	//	}
	//};

	render_system_update_callback update_callback;
	update_callback.parameter.cpu_resource = in_cpu_resource;
	update_callback.parameter.gpu_resource = in_gpu_resource;
	update_callback.update_funtion = in_update_gpu_res_callback_function;
	
	update_gpu_callback_queue.push(update_callback);
}


//��������ˢ�º���
void render_system::update_gpu_memory()
{
	while (!update_gpu_callback_queue.empty())
	{
		auto& update_function = update_gpu_callback_queue.front();
		//ִ��ˢ�º���
		update_function.update_funtion(
			update_function.parameter.cpu_resource, 
			update_function.parameter.gpu_resource,
			renderer);

		update_gpu_callback_queue.pop();
	}
}



void render_system::clear_gpu_texture(gpu_resource* in_gpu_resource)
{
	typedef GPU_RESOURCE_LAYOUT::GPU_RESOURCE_TYPE GPU_RES_TYPE;

	for (auto it : in_gpu_resource->gpu_resource_group[GPU_RES_TYPE::GPU_RES_RENDER_TARGET])
	{

	}

	for (auto it : in_gpu_resource->gpu_resource_group[GPU_RES_TYPE::GPU_RES_DEPTH_STENCIL])
	{

	}
}