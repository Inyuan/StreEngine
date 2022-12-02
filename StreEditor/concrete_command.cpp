#include "concrete_command.h"
#include "s_invoker.h"
#include "property_widget.h"
#include <QLayout>
#include <cmath>

extern const int pass_max_level = 100;

extern int pipeline_w_mouse_position_x;
extern int pipeline_w_mouse_position_y;

extern pipeline_window_invoker* pipeline_window_widget_ptr;
extern debug_text_invoker* debug_text_ptr;
extern component_invoker* current_component_ptr;

extern property_tab_widget* current_property_tab_widget;
extern property_widget* current_empty_property_widget;
extern mesh_property_widget* current_mesh_property_widget;
extern shader_property_widget* current_shader_property_widget;
extern texture_property_widget* current_texture_property_widget;
extern texture_group_property_widget* current_texture_group_property_widget;
extern pass_property_widget* current_pass_property_widget;

extern bool had_output_pass;

/// <summary>
/// ����Ψһ��ʶ��
/// </summary>
/// <typeparam name="t_type"></typeparam>
/// <param name="in_out_uid"></param>
template<typename t_type>
void generate_unique_identifier(s_uid& in_out_uid)
{
	std::string name_str(typeid(t_type).name());

	name_str += std::to_string(time(NULL));
	name_str += std::to_string((unsigned int)(&in_out_uid));

	memcpy(in_out_uid.name, name_str.c_str(), 256 * sizeof(char));
}

/***
************************************************************
*
* Create Function
*
************************************************************
*/

/// <summary>
/// ����ͼ����������ͼ
/// </summary>
void s_create_texture_command::execute()
{
	//����ʵ��
	//DEBUG
	auto texture_ptr = stre_engine::get_instance()->create_texture(init_type);

	//DEBUG

	//����ͼ����������ͼ
	texture_component_add_texture_ptr->add_element(texture_ptr);
}

/// <summary>
/// ����texture group���
/// </summary>
void s_create_texture_group_command::execute()
{
	//����ʵ��
	auto t_group = stre_engine::get_instance()->create_texture(gpu_shader_resource::SHADER_RESOURCE_TYPE_RENDER_DEPTH_STENCIL_GROUP);


	//������ͼ��� ��ӽ�ӳ���
	pipeline_window_widget_ptr->texture_comp_group[t_group->uid.name] = new texture_component_invoker(pipeline_window_widget_ptr, t_group);
	pipeline_window_widget_ptr->texture_comp_group[t_group->uid.name]->show();
}

/// <summary>
/// �������� ����CPU��Դ + �������
/// </summary>
void s_create_mesh_command::execute()
{

	//����ʵ��
	auto mesh_ptr = stre_engine::get_instance()->create_viewport_mesh();

	//������ͼ���
	pipeline_window_widget_ptr->mesh_comp_group[mesh_ptr->uid.name] = new mesh_component_invoker(pipeline_window_widget_ptr, mesh_ptr);
	pipeline_window_widget_ptr->mesh_comp_group[mesh_ptr->uid.name]->show();

}


/// <summary>
/// ����shader
/// </summary>
void s_create_shader_command::execute()
{

	//Debug
	shader_layout debug_shader_layout;
	generate_unique_identifier<shader_layout>(debug_shader_layout.uid);
	debug_shader_layout.shader_vaild[shader_layout::VS] = true;
	debug_shader_layout.shader_vaild[shader_layout::PS] = true;
	debug_shader_layout.shader_path[shader_layout::VS] = L"Shaders\\m_pass_debug_shader.hlsl";
	debug_shader_layout.shader_path[shader_layout::PS] = L"Shaders\\m_pass_debug_shader.hlsl";
	debug_shader_layout.shader_input_group.push_back(
		{ "POSITION",shader_layout::shader_input::INPUT_ELEMENT_SIZE_R32G32B32 });
	debug_shader_layout.shader_input_group.push_back(
		{ "NORMAL",shader_layout::shader_input::INPUT_ELEMENT_SIZE_R32G32B32 });
	debug_shader_layout.shader_input_group.push_back(
		{ "TEXCOORD",shader_layout::shader_input::INPUT_ELEMENT_SIZE_R32G32 });
	debug_shader_layout.shader_input_group.push_back(
		{ "TANGENT",shader_layout::shader_input::INPUT_ELEMENT_SIZE_R32G32B32 });




	//������ͼ���
	pipeline_window_widget_ptr->shader_comp_group[debug_shader_layout.uid.name] = new shader_component_invoker(pipeline_window_widget_ptr, debug_shader_layout);
	pipeline_window_widget_ptr->shader_comp_group[debug_shader_layout.uid.name]->show();
}

/// <summary>
/// ����pass
/// </summary>
/// <param name=""></param>
void s_create_pass_command::execute()
{
	//����ʵ��
	auto pass_ptr = stre_engine::get_instance()->create_pass();
	pass_ptr->is_output = false;
	pass_ptr->is_depth_check = false;
	pass_ptr->is_translate = false;
	//������ͼ���
	
	pass_component_invoker* new_pass_comp = new pass_component_invoker(pipeline_window_widget_ptr,pass_ptr);

	pipeline_window_widget_ptr->pass_comp_group[pass_ptr->uid.name]= new_pass_comp;
	
	//��������
	pipeline_window_widget_ptr->pass_comp_level_map[new_pass_comp->level].insert(new_pass_comp);

	new_pass_comp->show();
	
}

/// <summary>
/// ����pass��shader��Դ�ӿڵ������
/// </summary>
void reflect_pass_res_input(pass_component_invoker* in_pass_cmp)
{
	auto current_pass_component_ptr = in_pass_cmp;
	if (!current_pass_component_ptr)
	{
		return;
	}
	
	vector<connect_port*> res_port_group;

	if (current_pass_component_ptr->pass_instance->gpu_pass_ptr)
	{
		auto shader_res = current_pass_component_ptr->pass_instance->gpu_pass_ptr->pass_res_group;

		for (auto it : shader_res)
		{
			auto res_port = new connect_port(
				current_pass_component_ptr,
				port_information(
					port_information::PASS_RES_PORT_GROUP,
					current_pass_component_ptr,
					it.bind_point));

			string res_t;
			switch (it.type)
			{
			case gpu_shader_resource::SHADER_RESOURCE_TYPE_CUSTOM_BUFFER:
				res_t = "b";
				break;
			case gpu_shader_resource::SHADER_RESOURCE_TYPE_CUSTOM_BUFFER_GROUP:
			case gpu_shader_resource::SHADER_RESOURCE_TYPE_TEXTURE:
			case gpu_shader_resource::SHADER_RESOURCE_TYPE_TEXTURE_GROUP:
				res_t = "t";
				break;
			}

			string res_port_name = it.name + " " + res_t + " " + std::to_string(it.bind_point); //+ " " + std::to_string(it.register_space)

			res_port->setObjectName(res_port_name);
			res_port->setAutoExclusive(false);

			res_port_group.push_back(res_port);

		}
	}

	current_pass_component_ptr->update_res_port(res_port_group);

}


/// <summary>
/// �����˿����ӣ���飬�����Դ
/// </summary>
void s_connect_resource_command::execute()
{
	if (!select_connect_port[0] || !select_connect_port[1])
	{
		//!!!��������
		//! ָ�� �п����������ɾ��
		return;
	}
	const port_information connect_port1 = select_connect_port[0]->port_inf;
	const port_information connect_port2 = select_connect_port[1]->port_inf;


	//��ȡ���ӵ�����port
	bool connect_success = false;
	switch (connect_port1.port_type)
	{
	//texture���ܵ�����ΪSRV���뵽pass��
	//case port_information::TEXTURE_OUTPUT:
	//{
	//	//��������Pass��res�����
	//	switch (connect_port2.port_type)
	//	{
	//		case port_information::PASS_RES_PORT_GROUP:
	//		{
	//			texture_element_invoker* t_ptr = reinterpret_cast<texture_element_invoker*>(connect_port1.ptr);
	//			pass_component_invoker* p_ptr = reinterpret_cast<pass_component_invoker*>(connect_port2.ptr);
	//			t_ptr->texture_instance->gpu_sr_ptr->register_index = connect_port1.port_index;
	//			connect_success = stre_engine::get_instance()->pass_add_shader_resource<cpu_texture>(p_ptr->pass_instance, t_ptr->texture_instance);
	//		}
	//		break;
	//	}
	//}
	//	break;
	case port_information::TEXTURE_GROUP_INPUT:
	{
		//��������Pass�������
		switch (connect_port2.port_type)
		{
		case port_information::PASS_OUTPUT:
		{
			//��ͼ��port��¼������ͼ�ؼ���ָ��
			texture_component_invoker* t_ptr = reinterpret_cast<texture_component_invoker*>(connect_port1.ptr);
			pass_component_invoker* p_ptr = reinterpret_cast<pass_component_invoker*>(connect_port2.ptr);
			//����
			{
				//�����texture_comp�������
				
				//��ˢ�£���textureû����gpu
				s_update_gpu_command local_update_request;
				local_update_request.execute();
				if (!local_update_request.execute_success)
				{
					//!!!�������ˣ���ֹ����
					connect_success = false;
				}
				else
				{
					connect_success = stre_engine::get_instance()->pass_add_render_target(p_ptr->pass_instance, t_ptr->texture_instance);
				}
			}
			//ˢ��texture��pass�ı�
			{
				p_ptr->output_texture_comp_group.insert(t_ptr);
				t_ptr->input_pass_comp_group.insert(p_ptr);
			}
			//ˢ��texture�����pass
			{
				for (auto it : t_ptr->output_pass_comp_group)
				{
					if (it->level < p_ptr->level + 1)
					{
						pipeline_window_widget_ptr->pass_comp_level_map[it->level].erase(it);
						//ˢ��level
						it->level = p_ptr->level + 1;
						//ˢ�� ����λ��
						pipeline_window_widget_ptr->pass_comp_level_map[it->level].insert(it);
					}
				}
			}
		}

		break;
		}
	}
		break;
	case port_information::TEXTURE_GROUP_OUTPUT:
	{
		//��������Pass��res�����
		switch (connect_port2.port_type)
		{
		case port_information::PASS_RES_PORT_GROUP:
		{
			//��ͼ��port��¼������ͼ�ؼ���ָ��
			texture_component_invoker* t_ptr = reinterpret_cast<texture_component_invoker*>(connect_port1.ptr);
			pass_component_invoker* p_ptr = reinterpret_cast<pass_component_invoker*>(connect_port2.ptr);
			//����
			{
				//�����texture_comp�������
				// 
				//�Ĵ�����
				t_ptr->texture_instance->gpu_sr_ptr->register_index = connect_port1.port_index;

				connect_success = stre_engine::get_instance()->pass_add_shader_resource<cpu_texture>(p_ptr->pass_instance, t_ptr->texture_instance);
			}
			//ˢ��texture��pass�ı�
			{
				p_ptr->input_texture_comp_group.insert(t_ptr);
				t_ptr->output_pass_comp_group.insert(p_ptr);
			}
			//ˢ��pass��level������
			{
				//�򵥵��Ը�texture������pass ���� passlevel
				int max_level = p_ptr->level;
				for (auto it : t_ptr->input_pass_comp_group)
				{
					max_level = std::max(it->level + 1, max_level);
				}
				if (max_level != p_ptr->level)
				{
					pipeline_window_widget_ptr->pass_comp_level_map[p_ptr->level].erase(p_ptr);
					//ˢ��level
					p_ptr->level = max_level;
					//ˢ�� ����λ��
					pipeline_window_widget_ptr->pass_comp_level_map[p_ptr->level].insert(p_ptr);
				}
			}

		}
		break;
		}
	}
		break;
	case port_information::MESH_OUTPUT:
	{
		//MESH����
		switch (connect_port2.port_type)
		{
		case port_information::PASS_MESH_INPUT:
		{
			cpu_mesh* m_ptr = reinterpret_cast<cpu_mesh*>(connect_port1.ptr);

			pass_component_invoker* p_ptr = reinterpret_cast<pass_component_invoker*>(connect_port2.ptr);


			//��ˢ����meshûgpu_resource
			s_update_gpu_command local_update_request;
			local_update_request.execute();
			if (!local_update_request.execute_success)
			{
				//!!!�������ˣ���ֹ����
				connect_success = false;
			}
			else
			{
				connect_success = stre_engine::get_instance()->pass_add_mesh(p_ptr->pass_instance, m_ptr);
			}
		}
		break;
		}
	}
		break;
	case port_information::SHADER_OUTPUT:
	{
		//Shader����
		switch (connect_port2.port_type)
		{
		case port_information::PASS_SHADER_INPUT:
		{
			shader_layout* s_ptr = reinterpret_cast<shader_layout*>(connect_port1.ptr);

			pass_component_invoker* p_ptr = reinterpret_cast<pass_component_invoker*>(connect_port2.ptr);

			connect_success = stre_engine::get_instance()->pass_set_shader_layout(p_ptr->pass_instance, *s_ptr);

			//!!!����ˢ��
			s_update_gpu_command local_update_request;
			local_update_request.execute();
			if (!local_update_request.execute_success)
			{
				//!!!�������ˣ���ֹ����
				connect_success = false;
			}
			connect_success &= stre_engine::get_instance()->allocate_pass(p_ptr->pass_instance);
			
			local_update_request.execute();
			if (!local_update_request.execute_success)
			{
				//��ɫ�����벻ͨ��
				//!!!�������ˣ���ֹ����
				connect_success = false;
			}

			reflect_pass_res_input(p_ptr);

		}
		break;
		}
	}
		break;
	//case port_information::PASS_RES_PORT_GROUP:
	//{
	//}
	//	break;
	//case port_information::PASS_MESH_INPUT:
	//{
	//}
	//	break;
	//case port_information::PASS_SHADER_INPUT:
	//{
	//}
	//	break;
	//case port_information::PASS_OUTPUT:
	//{
	//}
	//break;
	default:
	{
		//��������һ��
		if (try_time < 1)
		{
			auto tmp_ptr = select_connect_port[0];
			select_connect_port[0] = select_connect_port[1];
			select_connect_port[1] = tmp_ptr;
			execute();
			try_time++;
		}
		//����͹�0
		try_time = 0;

	}
	break;
	}

	if (connect_success)
	{
		QPoint start = select_connect_port[0]->mapToGlobal(QPoint(0,0));
		
		QPoint end = select_connect_port[1]->mapToGlobal(QPoint(0, 0));

		//Ĭ��sart�����end��������
		int local_start_x = select_connect_port[0]->width();
		int local_end_x = 0;
		//������ұ߾���ʼλ���޸�һ��
		if (start.x() > end.x())
		{
			local_start_x = 0;
			local_end_x = select_connect_port[1]->width();
		}



		start = select_connect_port[0]->mapToGlobal(QPoint(local_start_x, select_connect_port[0]->height() / 2));
		start = pipeline_window_widget_ptr->mapFromGlobal(start);

		end = select_connect_port[1]->mapToGlobal(QPoint(local_end_x, select_connect_port[1]->height() / 2));
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


/// <summary>
/// ɾ���������� ����ִ��һ�����Ӳ���
/// </summary>
void s_reconnect_resource_command::execute()
{
	if (!reconnect_port)
		return;

	for (auto it = pipeline_window_widget_ptr->connect_curve_group.begin(); it != pipeline_window_widget_ptr->connect_curve_group.end();)
	{
		if ((*it)->port1 == reconnect_port || (*it)->port2 == reconnect_port)
		{
			s_disconnect_resource_command disconnect_resource_cmd;
			disconnect_resource_cmd.in_port1 = (*it)->port1;
			disconnect_resource_cmd.in_port2 = (*it)->port2;
			disconnect_resource_cmd.execute();


			if (!disconnect_resource_cmd.disconnect_success)
			{
				//!!!��������
				return;
			}
			s_connect_resource_command connect_resource_cmd;

			connect_resource_cmd.select_connect_port[0] = (*it)->port1;
			connect_resource_cmd.select_connect_port[1] = (*it)->port2;

			it = pipeline_window_widget_ptr->connect_curve_group.erase(it);

			connect_resource_cmd.execute();

			connect_resource_cmd.select_connect_port[0] = nullptr;
			connect_resource_cmd.select_connect_port[1] = nullptr;

		}
		else
		{
			it++;
		}
	}
}

/***
************************************************************
*
* Draw Function
*
************************************************************
*/

/// <summary>
/// useless
/// </summary>
//void s_update_pass_tree_command::execute()
//{
//	//auto pass_group = pipeline_window_widget_ptr->pass_comp_group;
//
//	//pass_component_invoker* output_pass_comp;
//
//	////���ҵ������pass
//	//for (auto it : pass_group)
//	//{
//	//	if (it.second->pass_instance->is_output)
//	//	{
//	//		output_pass_comp = it.second;
//	//		break;
//	//	}
//	//}
//
//	//auto connect_group = pipeline_window_widget_ptr->connect_curve_group;
//
//	//for (auto it : connect_group)
//	//{
//
//	//}
//
//
//}

/// <summary>
/// ����pass��ִ��pass
/// </summary>
void s_draw_command::execute()
{

	auto pass_group = pipeline_window_widget_ptr->pass_comp_level_map;
	//����0->n�����ȼ�˳�� ����ִ��ÿ��pass
	for (auto it : pass_group)
	{
		for (auto itt : it.second)
		{
			if (!stre_engine::get_instance()->check_pass(itt->pass_instance))
			{
				continue;
			}

			stre_engine::get_instance()->update_gpu_memory();

			stre_engine::get_instance()->draw_pass(itt->pass_instance);
		}
	}
	stre_engine::get_instance()->execute_command();

}

/// <summary>
/// ˢ��debug_output ����
/// </summary>
void s_debug_output_refresh_command::execute()
{
	string current_output;
	for (auto it : stre_exception::exception_output_str_group)
	{
		current_output.append(it + "\n");
	}
	stre_exception::exception_output_str_group.clear();

	debug_text_ptr->setText(QString::fromStdString(current_output));
}

void s_update_gpu_command::execute()
{
	
	if (!stre_engine::get_instance()->update_gpu_memory())
	{
		execute_success = false;
		s_debug_output_refresh_command().execute();
	}
}


/***
************************************************************
*
* Remove Function
*
************************************************************
*/

//ɾ��������Դ
void s_disconnect_resource_command::execute()
{
	disconnect_success = false;

	const port_information connect_port1 = in_port1->port_inf;
	const port_information connect_port2 = in_port2->port_inf;

	switch (connect_port1.port_type)
	{
	//texture���ܵ�����ΪSRV���뵽pass��
	//case port_information::TEXTURE_OUTPUT:
	//{
	//	//��������Pass��res�����
	//	switch (connect_port2.port_type)
	//	{
	//	case port_information::PASS_RES_PORT_GROUP:
	//	{
	//		//��ͼ��port��¼������ͼ�ؼ���ָ��
	//		texture_element_invoker* t_ptr = reinterpret_cast<texture_element_invoker*>(connect_port1.ptr);
	//		pass_component_invoker* p_ptr = reinterpret_cast<pass_component_invoker*>(connect_port2.ptr);
	//		disconnect_success = stre_engine::get_instance()->pass_remove_shader_resource<cpu_texture>(p_ptr->pass_instance, t_ptr->texture_instance);
	//	}
	//	break;
	//	}
	//}
	//break;

	case port_information::TEXTURE_GROUP_INPUT:
	{
		//��������Pass�������
		switch (connect_port2.port_type)
		{
		case port_information::PASS_OUTPUT:
		{
			//��ͼ��port��¼������ͼ�ؼ���ָ��
			texture_component_invoker* t_ptr = reinterpret_cast<texture_component_invoker*>(connect_port1.ptr);

			pass_component_invoker* p_ptr = reinterpret_cast<pass_component_invoker*>(connect_port2.ptr);

			//����
			{
				disconnect_success = stre_engine::get_instance()->pass_remove_render_target(p_ptr->pass_instance, t_ptr->texture_instance);
			}
			//ɾ����ͼ��pass���еļ�¼
			{
				p_ptr->output_texture_comp_group.erase(t_ptr);
				t_ptr->input_pass_comp_group.erase(p_ptr);
			}
			//ˢ�����pass��level
			//{
			//	//����ͼ��������pass
			//	for (auto it : t_ptr->output_pass_comp_group)
			//	{
			//		int max_level = 0;
			//		//�ڸ�pass��������б�����ͼ
			//		for (auto itt : it->input_texture_comp_group)
			//		{
			//			//�ڸ���ͼ��������б���pass �����level
			//			for (auto ittt : itt->input_pass_comp_group)
			//			{
			//				max_level = std::max(max_level, ittt->level + 1);
			//			}
			//		}
			//		if (max_level != it->level)
			//		{
			//			//ˢ�� ����
			//			pipeline_window_widget_ptr->pass_comp_level_map[it->level].erase(it);
			//			it->level = max_level;
			//			pipeline_window_widget_ptr->pass_comp_level_map[it->level].insert(it);
			//		}
			//	}
			//}
		}
		break;
		}
	}
	break;
	case port_information::TEXTURE_GROUP_OUTPUT:
	{
		//��������Pass��res�����
		switch (connect_port2.port_type)
		{
		case port_information::PASS_RES_PORT_GROUP:
		{
			//��ͼ��port��¼������ͼ�ؼ���ָ��
			texture_component_invoker* t_ptr = reinterpret_cast<texture_component_invoker*>(connect_port1.ptr);

			pass_component_invoker* p_ptr = reinterpret_cast<pass_component_invoker*>(connect_port2.ptr);

			//����
			{
				disconnect_success = stre_engine::get_instance()->pass_remove_shader_resource<cpu_texture>(p_ptr->pass_instance, t_ptr->texture_instance);
			}
			//ɾ����ͼ��pass���еļ�¼
			{
				p_ptr->input_texture_comp_group.erase(t_ptr);
				t_ptr->output_pass_comp_group.erase(p_ptr);
			}
			//ˢ�¸�pass��level
			//{
			//	int max_level = 0;
			//	//�ڸ�pass��������б�����ͼ
			//	for (auto it : p_ptr->input_texture_comp_group)
			//	{
			//		//�ڸ���ͼ��������б���pass �����level
			//		for (auto itt : it->input_pass_comp_group)
			//		{
			//			max_level = std::max(max_level, itt->level + 1);
			//		}
			//	}
			//	if (max_level != p_ptr->level)
			//	{
			//		//ˢ�� ����
			//		pipeline_window_widget_ptr->pass_comp_level_map[p_ptr->level].erase(p_ptr);
			//		p_ptr->level = max_level;
			//		pipeline_window_widget_ptr->pass_comp_level_map[p_ptr->level].insert(p_ptr);
			//	}
			//}
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

			pass_component_invoker* p_ptr = reinterpret_cast<pass_component_invoker*>(connect_port2.ptr);

			disconnect_success = stre_engine::get_instance()->pass_remove_mesh(p_ptr->pass_instance, m_ptr);
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
			pass_component_invoker* p_ptr = reinterpret_cast<pass_component_invoker*>(connect_port2.ptr);

			disconnect_success = stre_engine::get_instance()->pass_remove_shader_layout(p_ptr->pass_instance);

			//�������� �����յ�pass
			stre_engine::get_instance()->release_pass(p_ptr->pass_instance);
			
			reflect_pass_res_input(p_ptr);
		}
		break;
		}
	}
	break;
	//case port_information::PASS_RES_PORT_GROUP:
	//{
	//}
	//break;
	//case port_information::PASS_MESH_INPUT:
	//{
	//}
	//break;
	//case port_information::PASS_SHADER_INPUT:
	//{
	//}
	//break;
	//case port_information::PASS_OUTPUT:
	//{
	//}
	//break;
	default:
	{
		//����������
		if (in_try_times < 1)
		{
			in_try_times++;
			auto tmp = in_port1;
			in_port1 = in_port2;
			in_port2 = tmp;
			execute();
		}
	}
		break;
	}

}

/// <summary>
/// �Ƴ�����disconnect_port���ӵ���Դ
/// </summary>
void s_disconnect_all_resource_command::execute()
{
	if (!disconnect_port)
		return;

	disconnect_success = true;

	for (auto it = pipeline_window_widget_ptr->connect_curve_group.begin(); it != pipeline_window_widget_ptr->connect_curve_group.end();)
	{
		//������ߣ��о�ɾ
		if ((*it)->port1 == disconnect_port || (*it)->port2 == disconnect_port)
		{
			s_disconnect_resource_command disconnect_resource_cmd;
			disconnect_resource_cmd.in_port1 = (*it)->port1;
			disconnect_resource_cmd.in_port2 = (*it)->port2;
			disconnect_resource_cmd.execute();
			disconnect_success = disconnect_resource_cmd.disconnect_success;
			if (disconnect_success)
			{
				it = pipeline_window_widget_ptr->connect_curve_group.erase(it);
			}
			else
			{
				//!!!��������
				break;
			}
		}
		else
		{
			it++;
		}
	}
	pipeline_window_widget_ptr->update();
}

/// <summary>
/// ����ͼ�����Ƴ���ͼ
/// </summary>
void s_remove_texture_command::execute()
{
	if (!texture_element_delete_ptr)
	{
		return;
	}

	auto parent_ptr = (texture_component_invoker*)texture_element_delete_ptr->parent();
	parent_ptr->remove_element(texture_element_delete_ptr->texture_instance);
	texture_element_delete_ptr = nullptr;
}

/// <summary>
/// ɾ�����ӵĽӿ�
/// ɾ������ָ��
/// ɾ�����
/// </summary>
void s_remove_texture_group_command::execute()
{
	if(!texture_component_delete_ptr)
	{
		//!!!��������
		return;
	}

	//ɾ�����ӽӿ�
	s_disconnect_all_resource_command disconnect_resource_cmd;
	disconnect_resource_cmd.disconnect_port = texture_component_delete_ptr->input_port;
	disconnect_resource_cmd.execute();
	//�ϲ��˾Ͳ���ɾ
	if (!disconnect_resource_cmd.disconnect_success)
	{
		//!!!��������
		return;
	}

	disconnect_resource_cmd.disconnect_port = texture_component_delete_ptr->output_port;
	disconnect_resource_cmd.execute();
	//�ϲ��˾Ͳ���ɾ
	if (!disconnect_resource_cmd.disconnect_success)
	{
		//!!!��������
		return;
	}

	disconnect_resource_cmd.disconnect_port = nullptr;

	//ɾ����ͼ��
	for (auto it : texture_component_delete_ptr->textures_group)
	{
		//û�ж˿�
		//ֻ��Ͽ��˿�
		//disconnect_resource_cmd.disconnect_port = it->output_port;
		//disconnect_resource_cmd.execute();
		////�ϲ��˾Ͳ���ɾ
		//if (!disconnect_resource_cmd.disconnect_success)
		//{
		//	//!!!��������
		//	return;
		//}
		//disconnect_resource_cmd.disconnect_port = nullptr;

		//�������ɾ����,�Զ�ɾ�����������������������ɾ��
		//it->deleteLater();
	}

	//ɾ�����е�ָ��
	pipeline_window_widget_ptr->texture_comp_group.erase(texture_component_delete_ptr->texture_instance->uid.name);


	texture_component_delete_ptr->deleteLater();
	texture_component_delete_ptr = nullptr;
}

/// <summary>
/// ɾ�����ӵĽӿ�
/// ɾ������ָ��
/// ɾ�����
/// </summary>
void s_remove_mesh_command::execute()
{
	if (!mesh_component_delete_ptr)
	{
		return;
	}

	//ɾ�����ӽӿ�
	s_disconnect_all_resource_command disconnect_resource_cmd;
	disconnect_resource_cmd.disconnect_port = mesh_component_delete_ptr->output_port;
	disconnect_resource_cmd.execute();
	//�ϲ��˾Ͳ���ɾ
	if (!disconnect_resource_cmd.disconnect_success)
	{
		return;
	}

	disconnect_resource_cmd.disconnect_port = nullptr;

	//ɾ�����е�ָ��
	pipeline_window_widget_ptr->mesh_comp_group.erase(mesh_component_delete_ptr->mesh_instance->uid.name);

	mesh_component_delete_ptr->deleteLater();
}

/// <summary>
/// ɾ�����ӵĽӿ�
/// ɾ������ָ��
/// ɾ�����
/// </summary>
void s_remove_shader_command::execute()
{
	if (!shader_component_delete_ptr)
	{
		//!!!��������
		return;
	}

	//ɾ�����ӽӿ�
	s_disconnect_all_resource_command disconnect_resource_cmd;
	disconnect_resource_cmd.disconnect_port = shader_component_delete_ptr->output_port;
	disconnect_resource_cmd.execute();
	//�ϲ��˾Ͳ���ɾ
	if (!disconnect_resource_cmd.disconnect_success)
	{
		//!!!��������
		return;
	}

	disconnect_resource_cmd.disconnect_port = nullptr;

	//ɾ�����е�ָ��
	pipeline_window_widget_ptr->shader_comp_group.erase(shader_component_delete_ptr->shader_layout_instance.uid.name);

	shader_component_delete_ptr->deleteLater();
	shader_component_delete_ptr = nullptr;
}



/// <summary>
/// ɾ�����ӵĽӿ�
/// ɾ������ָ��
/// ɾ�����
/// </summary>
void s_remove_pass_command::execute()
{
	if (!pass_component_delete_ptr)
	{
		//!!!��������
		return;
	}
	if (pass_component_delete_ptr->pass_instance->is_output)
	{
		had_output_pass = false;
	}

	s_disconnect_all_resource_command disconnect_resource_cmd;
	

	//ɾ�����ӽӿ�
	for (auto it : pass_component_delete_ptr->input_res_port_group)
	{
		disconnect_resource_cmd.disconnect_port = it;
		disconnect_resource_cmd.execute();
		//�ϲ��˾Ͳ���ɾ
		if (!disconnect_resource_cmd.disconnect_success)
		{
			//!!!��������
			return;
		}
		disconnect_resource_cmd.disconnect_port = nullptr;
	}

	//ɾ�����ӽӿ�
	disconnect_resource_cmd.disconnect_port = pass_component_delete_ptr->mesh_port;
	disconnect_resource_cmd.execute();
	//�ϲ��˾Ͳ���ɾ
	if (!disconnect_resource_cmd.disconnect_success)
	{
		//!!!��������
		return;
	}
	disconnect_resource_cmd.disconnect_port = nullptr;

	//ɾ�����ӽӿ�
	disconnect_resource_cmd.disconnect_port = pass_component_delete_ptr->shader_port;
	disconnect_resource_cmd.execute();
	//�ϲ��˾Ͳ���ɾ
	if (!disconnect_resource_cmd.disconnect_success)
	{
		//!!!��������
		return;
	}
	disconnect_resource_cmd.disconnect_port = nullptr;

	//ɾ�����ӽӿ�
	disconnect_resource_cmd.disconnect_port = pass_component_delete_ptr->output_port;
	disconnect_resource_cmd.execute();
	//�ϲ��˾Ͳ���ɾ
	if (!disconnect_resource_cmd.disconnect_success)
	{
		//!!!��������
		return;
	}

	disconnect_resource_cmd.disconnect_port = nullptr;

	//ɾ�����е�ָ��
	pipeline_window_widget_ptr->pass_comp_group.erase(pass_component_delete_ptr->pass_instance->uid.name);
	pipeline_window_widget_ptr->pass_comp_level_map[pass_component_delete_ptr->level].erase(pass_component_delete_ptr);

	pass_component_delete_ptr->deleteLater();
	pass_component_delete_ptr = nullptr;
}



/***
************************************************************
*
* Update Function
*
************************************************************
*/


void s_change_mesh_data_command::execute()
{
	if (current_component_ptr->comp_type != COMPONENT_TYPE_MESH)
	{
		return;
	}

	auto mesh_comp_ptr = static_cast<mesh_component_invoker*>(current_component_ptr);
	if (mesh_comp_ptr->mesh_instance->is_view_mesh || mesh_comp_ptr->mesh_instance->path.empty())
	{
		if (mesh_comp_ptr->mesh_instance)
		{
			delete(mesh_comp_ptr->mesh_instance);
		}
		mesh_comp_ptr->mesh_instance = nullptr;
		mesh_comp_ptr->mesh_instance = stre_engine::get_instance()->create_viewport_mesh();
	}
	else
	{

		auto path_str = mesh_comp_ptr->mesh_instance->path;
		if (mesh_comp_ptr->mesh_instance)
		{
			delete(mesh_comp_ptr->mesh_instance);
		}
		mesh_comp_ptr->mesh_instance = nullptr;
		//��Ҫ���·��ɶ��
		mesh_comp_ptr->mesh_instance = stre_engine::get_instance()->create_mesh_from_fbx(path_str);
	}

	s_switch_property_widget_command().execute();
}


/// <summary>
/// ѡ�����ĸ����
/// �������� ����ֵ�����Դ���
/// /// </summary>
void s_switch_property_widget_command::execute()
{
	s_update_gpu_command local_update_request;
	local_update_request.execute();
	//0:Empty
	//1:mesh
	//2:shader
	//3:texture
	//4:texture group
	//4:pass
	current_property_tab_widget->setTabEnabled(0, false);
	current_property_tab_widget->setTabEnabled(1, false);
	current_property_tab_widget->setTabEnabled(2, false);
	current_property_tab_widget->setTabEnabled(3, false);
	current_property_tab_widget->setTabEnabled(4, false);
	current_property_tab_widget->setTabEnabled(5, false);
	current_property_tab_widget->setTabVisible(0, false);
	current_property_tab_widget->setTabVisible(1, false);
	current_property_tab_widget->setTabVisible(2, false);
	current_property_tab_widget->setTabVisible(3, false);
	current_property_tab_widget->setTabVisible(4, false);
	current_property_tab_widget->setTabVisible(5, false);
	if (!current_component_ptr)
	{
		//��ʾ�մ���
		current_property_tab_widget->setTabEnabled(0,true);
		current_property_tab_widget->setTabVisible(0, true);
		return;
	}
	
	switch (current_component_ptr->comp_type)
	{
	case COMPONENT_TYPE_MESH:
	{
		current_property_tab_widget->setTabEnabled(1, true);
		current_property_tab_widget->setTabVisible(1, true);
		//�������� ����ֵ������
		auto mesh_comp_ptr = static_cast<mesh_component_invoker*>(current_component_ptr);

		if (mesh_comp_ptr->mesh_instance->is_view_mesh)
		{
			current_mesh_property_widget->type_select_comcobox->setCurrentIndex(0);
			current_mesh_property_widget->path_select_pushbutton->setEnabled(false);
		}
		else
		{
			current_mesh_property_widget->type_select_comcobox->setCurrentIndex(1);
			current_mesh_property_widget->path_select_pushbutton->setEnabled(true);
			QString path_str;
			path_str = path_str.fromStdWString(mesh_comp_ptr->mesh_instance->path);
			current_mesh_property_widget->path_text->setText(path_str);
		}
	}
	break;
	case COMPONENT_TYPE_SHADER:
	{
		current_property_tab_widget->setTabEnabled(2, true);
		current_property_tab_widget->setTabVisible(2, true);
		
		auto shader_comp_ptr = static_cast<shader_component_invoker*>(current_component_ptr);

		QString path_str;
		path_str = path_str.fromStdWString(shader_comp_ptr->shader_layout_instance.shader_path[0]);
		current_shader_property_widget->path_text->setText(path_str);
		
		current_shader_property_widget->vs_check_box->setChecked(shader_comp_ptr->shader_layout_instance.shader_vaild[0]);
		current_shader_property_widget->ds_check_box->setChecked(shader_comp_ptr->shader_layout_instance.shader_vaild[1]);
		current_shader_property_widget->hs_check_box->setChecked(shader_comp_ptr->shader_layout_instance.shader_vaild[2]);
		current_shader_property_widget->gs_check_box->setChecked(shader_comp_ptr->shader_layout_instance.shader_vaild[3]);
		current_shader_property_widget->ps_check_box->setChecked(shader_comp_ptr->shader_layout_instance.shader_vaild[4]);
	
	}
	break;
	case COMPONENT_TYPE_TEXTURE:
	{
		current_property_tab_widget->setTabEnabled(3, true);
		current_property_tab_widget->setTabVisible(3, true);

		auto texture_comp_ptr = static_cast<texture_element_invoker*>(current_component_ptr);

		//����·��
		QString path_str;
		path_str = path_str.fromStdWString(texture_comp_ptr->texture_instance->path);
		current_texture_property_widget->path_text->setText(path_str);

		//����ѡ���
		int index = -1;
		switch (texture_comp_ptr->texture_instance->gpu_sr_ptr->shader_resource_type)
		{
		case gpu_shader_resource::SHADER_RESOURCE_TYPE_TEXTURE:
			index = 0;
			break;
		case gpu_shader_resource::SHADER_RESOURCE_TYPE_RENDER_TARGET:
			index = 1;
			break;
		case gpu_shader_resource::SHADER_RESOURCE_TYPE_RENDER_DEPTH_STENCIL:
			index = 2;
			break;
		}
		current_texture_property_widget->type_select_comcobox->setCurrentIndex(index);
	
	}
	break;
	case COMPONENT_TYPE_TEXTURE_GROUP:
	{
		current_property_tab_widget->setTabEnabled(4, true);
		current_property_tab_widget->setTabVisible(4, true);
	
		auto texture_comp_ptr = static_cast<texture_component_invoker*>(current_component_ptr);

		//����ѡ���
		int index = -1;
		switch (texture_comp_ptr->texture_instance->gpu_sr_ptr->shader_resource_type)
		{
		case gpu_shader_resource::SHADER_RESOURCE_TYPE_TEXTURE_GROUP:
			index = 0;
			break;
		case gpu_shader_resource::SHADER_RESOURCE_TYPE_RENDER_TARGET_GROUP:
			index = 1;
			break;
		case gpu_shader_resource::SHADER_RESOURCE_TYPE_RENDER_DEPTH_STENCIL_GROUP:
			index = 2;
			break;
		}

		current_texture_group_property_widget->type_select_comcobox->setCurrentIndex(index);

	}
	break;

	case COMPONENT_TYPE_PASS:
	{
		current_property_tab_widget->setTabEnabled(5, true);
		current_property_tab_widget->setTabVisible(5, true);
		
		auto pass_comp_ptr = static_cast<pass_component_invoker*>(current_component_ptr);
		
		//level comcobox
		current_pass_property_widget->pass_level_comcobox->setCurrentIndex(pass_comp_ptr->level);

		//is_output check
		//ֻ����һ��
		if (had_output_pass)
		{
			current_pass_property_widget->is_output_check_box->setEnabled(false);
		}
		else
		{
			current_pass_property_widget->is_output_check_box->setEnabled(true);
		}

		if (pass_comp_ptr->pass_instance->is_output)
		{
			current_pass_property_widget->is_output_check_box->setEnabled(true);
		}

		current_pass_property_widget->is_output_check_box->setChecked(pass_comp_ptr->pass_instance->is_output);
	}
	break;
	}
}

void s_update_texture_gpu_command::execute()
{
	if(!texture_update_ptr)
	{
		//!!!��������
		return;
	}
	//��ˢ�� ��ԭ����Դ�Ѿ������������
	s_update_gpu_command().execute();

	stre_engine::get_instance()->update_texture_gpu(texture_update_ptr);


}

void s_package_texture_command::execute()
{
	if (!texture_component_package_texture_ptr)
	{
		//!!!��������
		return;
	}
	vector<cpu_texture*> pack_buffer;
	for (int i = 0; i < texture_component_package_texture_ptr->textures_group.size(); i++)
	{
		pack_buffer.push_back(texture_component_package_texture_ptr->textures_group[i]->texture_instance);
	}
	//��ˢ�£��´������Դ��ûgpu��Դ
	s_update_gpu_command().execute();

	stre_engine::get_instance()->package_textures(pack_buffer, texture_component_package_texture_ptr->texture_instance);

}
