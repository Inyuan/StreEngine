#pragma once
#include "s_command.h"
#include "s_invoker.h"
//具体命令

//创建贴图
class s_create_texture_command : public s_command
{
public:
	virtual void execute() override;
	gpu_shader_resource::SHADER_RESOURCE_TYPE init_type = gpu_shader_resource::SHADER_RESOURCE_TYPE_RENDER_DEPTH_STENCIL;
	texture_component_invoker* texture_component_add_texture_ptr = nullptr;
};



class s_create_texture_group_command : public s_command
{
public:
	virtual void execute() override;
};

//创建物体
class s_create_mesh_command : public s_command
{
public:
	virtual void execute() override;
};

class s_change_mesh_data_command : public s_command
{
public:
	virtual void execute() override;
};

class s_switch_property_widget_command : public s_command 
{
public:
	virtual void execute() override;
};


class s_create_pass_command : public s_command
{
public:
	virtual void execute() override;
};

class s_connect_resource_command : public s_command
{
public:
	virtual void execute() override;

	connect_port* select_connect_port[2] = { nullptr };
private:
	
	int try_time = 0; //递归次数
};


class s_reconnect_resource_command : public s_command
{
public:
	virtual void execute() override;

	connect_port* reconnect_port = nullptr;

};

class s_create_shader_command : public s_command
{
public:
	virtual void execute() override;
};

/***
************************************************************
*
* Draw Function
*
************************************************************
*/
class s_package_texture_command : public s_command
{
public:
	virtual void execute() override;

	texture_component_invoker* texture_component_package_texture_ptr = nullptr;
};
class s_update_texture_gpu_command : public s_command
{
public:
	virtual void execute() override;

	cpu_texture* texture_update_ptr = nullptr;
};


class s_update_gpu_command : public s_command
{
public:
	virtual void execute() override;
	//自己局部定义该命令，并使用execute_success获得刷新结果。
	//刷新错误是否影响命令 要求执行命令者自行判断
	//一般来说，执行刷新命令的位置总是对资源有GPU实时要求
	bool execute_success = true;
};

class s_debug_output_refresh_command : public s_command
{
public:
	virtual void execute() override;
};

class s_draw_command : public s_command
{
public:
	virtual void execute() override;
};

/***
************************************************************
*
* Remove Function
*
************************************************************
*/

class s_disconnect_resource_command : public s_command
{
public:
	virtual void execute() override;

	connect_port* in_port1;
	connect_port* in_port2;
	bool disconnect_success = false;

private:
	int in_try_times = 0;
};

class s_disconnect_all_resource_command : public s_command
{
public:
	virtual void execute() override;

	bool disconnect_success = false;
	connect_port* disconnect_port = nullptr;
};

class s_remove_texture_command : public s_command
{
public:
	virtual void execute() override;

	texture_element_invoker* texture_element_delete_ptr = nullptr;

};

class s_remove_texture_group_command : public s_command
{
public:
	virtual void execute() override;

	texture_component_invoker* texture_component_delete_ptr = nullptr;

};

class s_remove_mesh_command : public s_command
{
public:
	virtual void execute() override;

	mesh_component_invoker* mesh_component_delete_ptr = nullptr;

};

class s_remove_shader_command : public s_command
{
public:
	virtual void execute() override;

	shader_component_invoker* shader_component_delete_ptr = nullptr;

};

class s_remove_pass_command : public s_command
{
public:
	virtual void execute() override;

	pass_component_invoker* pass_component_delete_ptr = nullptr;

};

