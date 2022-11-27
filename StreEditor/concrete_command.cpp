#include "concrete_command.h"
#include "s_invoker.h"
#include <QLayout>

extern int pipeline_w_mouse_position_x;
extern int pipeline_w_mouse_position_y;

extern pipeline_window_invoker* pipeline_window_widget_ptr;
extern texture_component_invoker* texture_component_add_texture_ptr;

extern connect_port* select_connect_port[2];
extern connect_port* reconnect_port;
extern connect_port* disconnect_port;
extern bool disconnect_success;

extern texture_element_invoker* texture_element_delete_ptr;
extern texture_component_invoker* texture_component_delete_ptr;
extern mesh_component_invoker* mesh_component_delete_ptr;
extern shader_component_invoker* shader_component_delete_ptr;
extern pass_component_invoker* pass_component_delete_ptr;

/// <summary>
/// 生成唯一标识符
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


/// <summary>
/// 在贴图组件中添加贴图
/// </summary>
void s_create_texture_command::execute()
{
	//构建实例
	//DEBUG
	gpu_shader_resource::SHADER_RESOURCE_TYPE debug;
	debug = gpu_shader_resource::SHADER_RESOURCE_TYPE_RENDER_DEPTH_STENCIL;
	auto texture_ptr = stre_engine::get_instance()->create_texture(debug);

	//DEBUG

	//在贴图组件中添加贴图
	texture_component_add_texture_ptr->add_element(texture_ptr);
}

/// <summary>
/// 构建texture group组件
/// </summary>
void s_create_texture_group_command::execute()
{
	//构建实例
	auto t_group = stre_engine::get_instance()->create_texture(gpu_shader_resource::SHADER_RESOURCE_TYPE_RENDER_DEPTH_STENCIL_GROUP);


	//构造蓝图组件 添加进映射表
	pipeline_window_widget_ptr->texture_comp_group[t_group->uid.name] = new texture_component_invoker(pipeline_window_widget_ptr, t_group);
	pipeline_window_widget_ptr->texture_comp_group[t_group->uid.name]->show();
}

/// <summary>
/// 创建物体 创建CPU资源 + 组件本体
/// </summary>
void s_create_mesh_command::execute()
{

	//构建实例
	auto mesh_ptr = stre_engine::get_instance()->create_viewport_mesh();


	//构造蓝图组件
	pipeline_window_widget_ptr->mesh_comp_group[mesh_ptr->uid.name] = new mesh_component_invoker(pipeline_window_widget_ptr, mesh_ptr);
	pipeline_window_widget_ptr->mesh_comp_group[mesh_ptr->uid.name]->show();

}

/// <summary>
/// 构建shader
/// </summary>
void s_create_shader_command::execute()
{

	//Debug
	shader_layout debug_shader_layout;
	generate_unique_identifier<shader_layout>(debug_shader_layout.uid);
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




	//构造蓝图组件
	pipeline_window_widget_ptr->shader_comp_group[debug_shader_layout.uid.name] = new shader_component_invoker(pipeline_window_widget_ptr, debug_shader_layout);
	pipeline_window_widget_ptr->shader_comp_group[debug_shader_layout.uid.name]->show();
}

/// <summary>
/// 构建pass
/// </summary>
/// <param name=""></param>
void s_create_pass_command::execute()
{
	//构建实例
	
	auto pass_ptr = stre_engine::get_instance()->create_pass();
	pass_ptr->is_output = true;
	pass_ptr->is_depth_check = false;
	pass_ptr->is_translate = false;
	//构造蓝图组件
	//!!！应该是树状指针!!!执行有优先次序
	pipeline_window_widget_ptr->pass_comp_group[pass_ptr->uid.name]  =
			new pass_component_invoker(
				pipeline_window_widget_ptr, 
				pass_ptr);
	
	pipeline_window_widget_ptr->pass_comp_group[pass_ptr->uid.name]->show();
}

/// <summary>
/// 反射pass的shader资源接口到组件上
/// </summary>
void reflect_pass_res_input(s_pass* in_pass)
{
	auto current_pass_component_ptr = pipeline_window_widget_ptr->pass_comp_group[in_pass->uid.name];
	if (!current_pass_component_ptr)
	{
		return;
	}
	
	vector<connect_port*> res_port_group;

	auto shader_res = current_pass_component_ptr->pass_instance->gpu_pass_ptr->pass_res_group;

	for (auto it : shader_res)
	{
		auto res_port = new connect_port(
			current_pass_component_ptr,
			port_information(
				port_information::PASS_RES_PORT_GROUP,
				current_pass_component_ptr->pass_instance,
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

	current_pass_component_ptr->update_res_port(res_port_group);

}


/// <summary>
/// 两个端口连接，检查，添加资源
/// </summary>
void s_connect_resource_command::execute()
{
	const port_information connect_port1 = select_connect_port[0]->port_inf;
	const port_information connect_port2 = select_connect_port[1]->port_inf;

	//暂时只写了左到右的
	//获取连接的两个port
	bool connect_success = false;
	switch (connect_port1.port_type)
	{
	case port_information::TEXTURE_GROUP_INPUT:
	{
		//可以连到Pass的输出口
		switch (connect_port2.port_type)
		{
		case port_information::PASS_OUTPUT:
		{
			//贴图的port记录的是贴图控件的指针
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
		//可以连到Pass的res输入口
		switch (connect_port2.port_type)
		{
		case port_information::PASS_RES_PORT_GROUP:
		{
			//贴图的port记录的是贴图控件的指针
			texture_component_invoker* t_ptr = reinterpret_cast<texture_component_invoker*>(connect_port1.ptr);

			vector<cpu_texture*> pack_buffer;
			for (int i = 0; i < t_ptr->textures_group.size(); i++)
			{
				pack_buffer.push_back(t_ptr->textures_group[i]->texture_instance);
			}

			stre_engine::get_instance()->package_textures(pack_buffer, t_ptr->texture_instance);

			//寄存器号
			t_ptr->texture_instance->gpu_sr_ptr->register_index = connect_port1.port_index;

			s_pass* p_ptr = reinterpret_cast<s_pass*>(connect_port2.ptr);

			connect_success = stre_engine::get_instance()->pass_add_shader_resource<cpu_texture>(p_ptr, t_ptr->texture_instance);


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

			//!!!即刻刷新
			stre_engine::get_instance()->allocate_pass(p_ptr);

			reflect_pass_res_input(p_ptr);

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

		//制作连线曲线
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
/// 删除后再连接 重新执行一遍连接操作
/// </summary>
void s_reconnect_resource_command::execute()
{
	if (!reconnect_port)
		return;

	for (auto it = pipeline_window_widget_ptr->connect_curve_group.begin(); it != pipeline_window_widget_ptr->connect_curve_group.end();)
	{
		if ((*it)->port1 == reconnect_port || (*it)->port2 == reconnect_port)
		{
			select_connect_port[0] = (*it)->port1;
			select_connect_port[1] = (*it)->port2;

			pipeline_window_widget_ptr->connect_curve_group.erase(it);

			s_connect_resource_command().execute();

		}
		else
		{
			it++;
		}
	}
}

/// <summary>
/// 遍历pass表执行pass
/// </summary>
void s_draw_command::execute()
{
	//!!！应该是树状指针
	
	auto pass_group = pipeline_window_widget_ptr->pass_comp_group;
	for (auto it : pass_group)
	{
		stre_engine::get_instance()->update_gpu_memory();

		stre_engine::get_instance()->draw_pass(it.second->pass_instance);
		
	}
	stre_engine::get_instance()->execute_command();

}


/***
************************************************************
*
* Remove Function
*
************************************************************
*/

//删除连接资源
bool remove_resource(connect_port* in_port1, connect_port* in_port2)
{
	bool disconnect_success = false;

	const port_information connect_port1 = in_port1->port_inf;
	const port_information connect_port2 = in_port2->port_inf;

	switch (connect_port1.port_type)
	{
	case port_information::TEXTURE_GROUP_INPUT:
	{
		//可以连到Pass的输出口
		switch (connect_port2.port_type)
		{
		case port_information::PASS_OUTPUT:
		{
			//贴图的port记录的是贴图控件的指针
			texture_component_invoker* t_ptr = reinterpret_cast<texture_component_invoker*>(connect_port1.ptr);

			s_pass* p_ptr = reinterpret_cast<s_pass*>(connect_port2.ptr);

			disconnect_success = stre_engine::get_instance()->pass_remove_render_target(p_ptr, t_ptr->texture_instance);
		}
		break;
		}
	}
	break;
	case port_information::TEXTURE_OUTPUT:
	{
		//可以连到Pass的res输入口
		switch (connect_port2.port_type)
		{
		case port_information::PASS_RES_PORT_GROUP:
		{
			//贴图的port记录的是贴图控件的指针
			texture_component_invoker* t_ptr = reinterpret_cast<texture_component_invoker*>(connect_port1.ptr);

			s_pass* p_ptr = reinterpret_cast<s_pass*>(connect_port2.ptr);

			disconnect_success = stre_engine::get_instance()->pass_remove_shader_resource<cpu_texture>(p_ptr, t_ptr->texture_instance);

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

			disconnect_success = stre_engine::get_instance()->pass_remove_mesh(p_ptr, m_ptr);
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
			s_pass* p_ptr = reinterpret_cast<s_pass*>(connect_port2.ptr);

			disconnect_success = stre_engine::get_instance()->pass_remove_shader_layout(p_ptr);

			//!!!即刻刷新， 制作空的pass
			stre_engine::get_instance()->allocate_pass(p_ptr);

			reflect_pass_res_input(p_ptr);

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

	return disconnect_success;
}

/// <summary>
/// 移除所有disconnect_port连接的资源
/// </summary>
void s_disconnect_resource_command::execute()
{
	if (!disconnect_port)
		return;

	disconnect_success = true;

	for (auto it = pipeline_window_widget_ptr->connect_curve_group.begin(); it != pipeline_window_widget_ptr->connect_curve_group.end();)
	{
		if ((*it)->port1 == disconnect_port || (*it)->port2 == disconnect_port)
		{
			disconnect_success = remove_resource((*it)->port1, (*it)->port2);
			if (disconnect_success)
			{
				pipeline_window_widget_ptr->connect_curve_group.erase(it);
			}
			else
			{
				break;
			}
		}
		else
		{
			it++;
		}
	}
}

/// <summary>
/// 在贴图组中移除贴图
/// </summary>
void s_remove_texture_command::execute()
{
	if (!texture_element_delete_ptr)
	{
		return;
	}

	auto parent_ptr = (texture_component_invoker*)texture_element_delete_ptr->parent();
	parent_ptr->remove_element(texture_element_delete_ptr->texture_instance);

}

/// <summary>
/// 删除连接的接口
/// 删除表中指针
/// 删除组件
/// </summary>
void s_remove_texture_group_command::execute()
{
	if(!texture_component_delete_ptr)
	{
		return;
	}

	//删除连接接口
	disconnect_port = texture_component_delete_ptr->input_port;
	s_disconnect_resource_command().execute();
	//断不了就不许删
	if (!disconnect_success)
	{
		return;
	}

	disconnect_port = texture_component_delete_ptr->output_port;
	s_disconnect_resource_command().execute();
	//断不了就不许删
	if (!disconnect_success)
	{
		return;
	}

	disconnect_port = nullptr;

	//删除表中的指针
	pipeline_window_widget_ptr->texture_comp_group.erase(texture_component_delete_ptr->texture_instance->uid.name);


	texture_component_delete_ptr->deleteLater();
}

/// <summary>
/// 删除连接的接口
/// 删除表中指针
/// 删除组件
/// </summary>
void s_remove_mesh_command::execute()
{
	if (!mesh_component_delete_ptr)
	{
		return;
	}

	//删除连接接口
	disconnect_port = mesh_component_delete_ptr->output_port;
	s_disconnect_resource_command().execute();
	//断不了就不许删
	if (!disconnect_success)
	{
		return;
	}

	disconnect_port = nullptr;

	//删除表中的指针
	pipeline_window_widget_ptr->mesh_comp_group.erase(mesh_component_delete_ptr->mesh_instance->uid.name);

	mesh_component_delete_ptr->deleteLater();
}

/// <summary>
/// 删除连接的接口
/// 删除表中指针
/// 删除组件
/// </summary>
void s_remove_shader_command::execute()
{
	if (!shader_component_delete_ptr)
	{
		return;
	}

	//删除连接接口
	disconnect_port = shader_component_delete_ptr->output_port;
	s_disconnect_resource_command().execute();
	//断不了就不许删
	if (!disconnect_success)
	{
		return;
	}

	disconnect_port = nullptr;

	//删除表中的指针
	pipeline_window_widget_ptr->shader_comp_group.erase(shader_component_delete_ptr->shader_layout_instance.uid.name);

	shader_component_delete_ptr->deleteLater();
}

/// <summary>
/// 删除连接的接口
/// 删除表中指针
/// 删除组件
/// </summary>
void s_remove_pass_command::execute()
{
	if (!pass_component_delete_ptr)
	{
		return;
	}

	//删除连接接口
	for (auto it : pass_component_delete_ptr->input_res_port_group)
	{
		disconnect_port = it;
		s_disconnect_resource_command().execute();
		//断不了就不许删
		if (!disconnect_success)
		{
			return;
		}
	}

	//删除连接接口
	disconnect_port = pass_component_delete_ptr->mesh_port;
	s_disconnect_resource_command().execute();
	//断不了就不许删
	if (!disconnect_success)
	{
		return;
	}

	//删除连接接口
	disconnect_port = pass_component_delete_ptr->shader_port;
	s_disconnect_resource_command().execute();
	//断不了就不许删
	if (!disconnect_success)
	{
		return;
	}

	//删除连接接口
	disconnect_port = pass_component_delete_ptr->output_port;
	s_disconnect_resource_command().execute();
	//断不了就不许删
	if (!disconnect_success)
	{
		return;
	}

	disconnect_port = nullptr;

	//删除表中的指针
	pipeline_window_widget_ptr->pass_comp_group.erase(pass_component_delete_ptr->pass_instance->uid.name);

	pass_component_delete_ptr->deleteLater();
}