#include "concrete_command.h"
#include "s_invoker.h"


extern int mouse_position_x;
extern int mouse_position_y;

extern pipeline_window_invoker* pipeline_window_widget_ptr;

void s_create_texture_command::execute()
{
	gpu_shader_resource::SHADER_RESOURCE_TYPE debug;
	debug = gpu_shader_resource::SHADER_RESOURCE_TYPE_RENDER_TARGET;
	auto texture_ptr = stre_engine::get_instance()->create_texture(debug);



	//构造蓝图组件
	//...

	pipeline_window_widget_ptr->textrue_comp_group.push_back(new texture_component_invoker(pipeline_window_widget_ptr,texture_ptr));

}

void s_create_mesh_command::execute()
{
	auto mesh_ptr = stre_engine::get_instance()->create_viewport_mesh();

	//构造蓝图组件
	//... 

}

void s_create_shader_commd::execute()
{

	//构造蓝图组件
	//...

}

void s_create_pass_command::execute()
{
	auto pass_ptr = stre_engine::get_instance()->create_pass();


	//构造蓝图组件
	//...

}


extern port_information connect_port1;
extern port_information connect_port2;

void s_connect_resource_command::execute()
{
	//暂时只写了左到右的
	//获取连接的两个port
	bool connect_success = false;
	switch (connect_port1.port_type)
	{
	case port_information::TEXTURE_INPUT:
	{
		//可以连到Pass的输出口
		switch (connect_port2.port_type)
		{
		case port_information::PASS_OUTPUT:
		{
			cpu_texture* t_ptr = reinterpret_cast<cpu_texture*>(connect_port1.ptr);
			
			s_pass* p_ptr = reinterpret_cast<s_pass*>(connect_port2.ptr);

			connect_success = stre_engine::get_instance()->pass_add_render_target(p_ptr, t_ptr);
			
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
		case port_information::PASS_OUTPUT:
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
		//制作连线曲线
		curve_tool* curve_ptr = new curve_tool(connect_port1.port_ptr, connect_port2.port_ptr);
		connect_port1.port_ptr->curve_ptr = curve_ptr;
		connect_port2.port_ptr->curve_ptr = curve_ptr;
	}


}