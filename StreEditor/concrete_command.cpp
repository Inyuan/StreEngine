#include "concrete_command.h"
#include "s_invoker.h"
#include <QLayout>

extern int pipeline_w_mouse_position_x;
extern int pipeline_w_mouse_position_y;

extern pipeline_window_invoker* pipeline_window_widget_ptr;
extern texture_component_invoker* current_texture_component_ptr;

//������ͼ ���
void s_create_texture_command::execute()
{
	//����ʵ��
	//DEBUG
	gpu_shader_resource::SHADER_RESOURCE_TYPE debug;
	debug = gpu_shader_resource::SHADER_RESOURCE_TYPE_RENDER_DEPTH_STENCIL;
	auto texture_ptr = stre_engine::get_instance()->create_texture(debug);

	//DEBUG

	//����ͼ����������ͼ
	current_texture_component_ptr->add_element(texture_ptr);
}

void s_create_texture_group_command::execute()
{
	//����ʵ��
	auto t_group = stre_engine::get_instance()->create_texture(gpu_shader_resource::SHADER_RESOURCE_TYPE_RENDER_DEPTH_STENCIL_GROUP);


	//������ͼ���
	pipeline_window_widget_ptr->texture_comp_group.push_back(new texture_component_invoker(pipeline_window_widget_ptr, t_group));
	pipeline_window_widget_ptr->texture_comp_group.back()->show();
}

//�������� ����CPU��Դ + �������
void s_create_mesh_command::execute()
{

	//����ʵ��
	auto mesh_ptr = stre_engine::get_instance()->create_viewport_mesh();


	//������ͼ���
	pipeline_window_widget_ptr->mesh_comp_group.push_back(new mesh_component_invoker(pipeline_window_widget_ptr, mesh_ptr));
	pipeline_window_widget_ptr->mesh_comp_group.back()->show();

}

void s_create_shader_command::execute()
{
	//Debug
	shader_layout debug_shader_layout;
	debug_shader_layout.shader_vaild[shader_layout::VS] = true;
	debug_shader_layout.shader_vaild[shader_layout::PS] = true;
	debug_shader_layout.shader_path[shader_layout::VS] = L"Shaders\\debug_pass.hlsl";
	debug_shader_layout.shader_path[shader_layout::PS] = L"Shaders\\debug_pass.hlsl";
	debug_shader_layout.shader_input_group.push_back(
		{ "POSITION",shader_layout::shader_input::INPUT_ELEMENT_SIZE_R32G32B32 });
	debug_shader_layout.shader_input_group.push_back(
		{ "NORMAL",shader_layout::shader_input::INPUT_ELEMENT_SIZE_R32G32B32 });
	debug_shader_layout.shader_input_group.push_back(
		{ "TEXCOORD",shader_layout::shader_input::INPUT_ELEMENT_SIZE_R32G32 });
	debug_shader_layout.shader_input_group.push_back(
		{ "TANGENT",shader_layout::shader_input::INPUT_ELEMENT_SIZE_R32G32B32 });




	//������ͼ���
	pipeline_window_widget_ptr->shader_comp_group.push_back(new shader_component_invoker(pipeline_window_widget_ptr, debug_shader_layout));
	pipeline_window_widget_ptr->shader_comp_group.back()->show();
}

void s_create_pass_command::execute()
{
	//����ʵ��
	
	auto pass_ptr = stre_engine::get_instance()->create_pass();
	pass_ptr->is_output = true;
	pass_ptr->is_depth_check = false;
	pass_ptr->is_translate = false;
	//������ͼ���
	//!!��Ӧ������״ָ��!!!ִ�������ȴ���
	pipeline_window_widget_ptr->
		pass_comp_group.push_back(
			new pass_component_invoker(
				pipeline_window_widget_ptr, 
				pass_ptr));
	
	pipeline_window_widget_ptr->pass_comp_group.back()->show();
}


extern connect_port* select_connect_port[2];

void s_connect_resource_command::execute()
{
	const port_information connect_port1 = select_connect_port[0]->port_inf;
	const port_information connect_port2 = select_connect_port[1]->port_inf;

	//��ʱֻд�����ҵ�
	//��ȡ���ӵ�����port
	bool connect_success = false;
	switch (connect_port1.port_type)
	{
	case port_information::TEXTURE_GROUP_INPUT:
	{
		//��������Pass�������
		switch (connect_port2.port_type)
		{
		case port_information::PASS_OUTPUT:
		{
			texture_component_invoker* t_ptr = reinterpret_cast<texture_component_invoker*>(connect_port1.ptr);
			
			vector<cpu_texture*> pack_buffer;
			for (int i = 0; i < t_ptr->textures_group.size(); i++)
			{
				pack_buffer.push_back(t_ptr->textures_group[i]->texture_instance);
			}

			stre_engine::get_instance()->package_textures(pack_buffer, t_ptr->texture_instance);


			s_pass* p_ptr = reinterpret_cast<s_pass*>(connect_port2.ptr);

			connect_success = stre_engine::get_instance()->pass_add_render_target(p_ptr, t_ptr->texture_instance);
			
		}

		break;
		}
	}
		break;
	case port_information::TEXTURE_OUTPUT:
	{
		//��������Pass��res�����
		switch (connect_port2.port_type)
		{
		case port_information::PASS_RES_PORT_GROUP:
		{

		}
		break;
		}
	}
		break;
	case port_information::MESH_OUTPUT:
	{
		switch (connect_port2.port_type)
		{
		case port_information::PASS_MESH_INPUT:
		{
			cpu_mesh* m_ptr = reinterpret_cast<cpu_mesh*>(connect_port1.ptr);

			s_pass* p_ptr = reinterpret_cast<s_pass*>(connect_port2.ptr);
			
			connect_success = stre_engine::get_instance()->pass_add_mesh(p_ptr, m_ptr);
		}
		break;
		}
	}
		break;
	case port_information::SHADER_OUTPUT:
	{
		switch (connect_port2.port_type)
		{
		case port_information::PASS_SHADER_INPUT:
		{
			shader_layout* s_ptr = reinterpret_cast<shader_layout*>(connect_port1.ptr);

			s_pass* p_ptr = reinterpret_cast<s_pass*>(connect_port2.ptr);

			connect_success = stre_engine::get_instance()->pass_set_shader_layout(p_ptr, *s_ptr);

			stre_engine::get_instance()->allocate_pass(p_ptr);
		}
		break;
		}
	}
		break;
	case port_information::PASS_RES_PORT_GROUP:
	{

	}
		break;
	case port_information::PASS_MESH_INPUT:
	{

	}
		break;
	case port_information::PASS_SHADER_INPUT:
	{

	}
		break;
	case port_information::PASS_OUTPUT:
	{

	}
	break;

	}

	if (connect_success)
	{
		QPoint start = QPoint(0,0);
		
		QPoint end = QPoint(0, 0);

		start = select_connect_port[0]->mapToGlobal(QPoint(0, 0));
		start = pipeline_window_widget_ptr->mapFromGlobal(start);

		end = select_connect_port[1]->mapToGlobal(QPoint(0, 0));
		end = pipeline_window_widget_ptr->mapFromGlobal(end);

		//������������
		auto curve_ptr = new curve_tool(
			start,
			end);

		pipeline_window_widget_ptr
			->connect_curve_group.push_back(
				new connect_data(
					select_connect_port[0],
					select_connect_port[1],
					curve_ptr));
	
		pipeline_window_widget_ptr->update();
	
	}
}

void s_package_texture_command::execute()
{
	
}

void s_draw_command::execute()
{

	//!!��Ӧ������״ָ��

	auto pass_group = pipeline_window_widget_ptr->pass_comp_group;
	for (int i = 0; i < pass_group.size(); i++)
	{
		stre_engine::get_instance()->update_gpu_memory();

		stre_engine::get_instance()->draw_pass(pass_group[i]->pass_instance);
		
	}
	stre_engine::get_instance()->execute_command();

}