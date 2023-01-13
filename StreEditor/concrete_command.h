#pragma once
#include "s_command.h"
#include "s_invoker.h"
//具体命令

/******************************************
* create command
*
*********************************************/

class s_create_texture_command : public s_command
{
public:
	/// <summary>
	/// Add textures to the texture component
	/// 在贴图组件中添加贴图
	/// </summary>
	virtual void execute() override;
	gpu_shader_resource::SHADER_RESOURCE_TYPE init_type = gpu_shader_resource::SHADER_RESOURCE_TYPE_RENDER_DEPTH_STENCIL;
	texture_component_invoker* texture_component_add_texture_ptr = nullptr;
};

class s_create_texture_group_command : public s_command
{
public:
	/// <summary>
	/// Build texture group components
	/// 构建texture group组件
	/// </summary>
	virtual void execute() override;
};

class s_create_mesh_command : public s_command
{
public:
	/// <summary>
	/// Create objects Create CPU resources + component ontology
	/// 创建物体 创建CPU资源 + 组件本体
	/// </summary>
	virtual void execute() override;
};

class s_create_pass_command : public s_command
{
public:
	/// <summary>
	/// build pass
	/// 构建pass
	/// </summary>
	/// <param name=""></param>
	virtual void execute() override;
};

class s_create_shader_command : public s_command
{
public:
	/// <summary>
	/// Create shaders
	/// 构建shader
	/// </summary>
	virtual void execute() override;
};

class s_create_camera_command : public s_command
{
public:
	/// <summary>
	/// Create camera
	/// 构建 camera
	/// </summary>
	virtual void execute() override;
};

class s_create_light_command : public s_command
{
public:
	/// <summary>
	/// Create light
	/// 构建 light
	/// </summary>
	virtual void execute() override;
};

/******************************************
* update / change / connect command
*
*********************************************/

class s_update_camera_command : public s_command
{
public:
	/// <summary>
	/// refresh camera
	/// 刷新 相机
	/// </summary>
	virtual void execute() override;
	RCamera* camera_helper_ptr = nullptr;
	cpu_camera* camera_ptr = nullptr;
};

class s_update_light_command : public s_command
{
public:
	/// <summary>
	/// update mesh data
	/// 刷新GPU灯光数据 
	/// </summary>
	virtual void execute() override;
	light_component_invoker* light_ptr = nullptr;
};

class s_change_mesh_type_command : public s_command
{
public:
	/// <summary>
	/// Modify mesh type
	/// 修改模型类型
	/// </summary>
	virtual void execute() override;
};

class s_update_mesh_data_command : public s_command
{
public:
	/// <summary>
	/// update mesh data
	/// 刷新GPU模型数据 
	/// </summary>
	virtual void execute() override;
	mesh_component_invoker* mesh_ptr = nullptr;
};


class s_switch_property_widget_command : public s_command
{
public:
	/// <summary>
	/// Which component is selected
	/// Traversing properties and reflecting values to property window
	/// 选中了哪个组件
	/// 遍历属性 反射值到属性窗口
	/// </summary>
	virtual void execute() override;
};

class s_connect_resource_command : public s_command
{
public:
	/// <summary>
	/// Two port connection, check, add resources
	/// 两个端口连接，检查，添加资源
	/// </summary>
	virtual void execute() override;

	connect_port* select_connect_port[2] = { nullptr };
private:
	
	int try_time = 0; //递归次数
};


class s_reconnect_resource_command : public s_command
{
public:
	/// <summary>
	/// Delete and connect again Perform the connection operation again
	/// 删除后再连接 重新执行一遍连接操作
	/// </summary>
	virtual void execute() override;

	connect_port* reconnect_port = nullptr;

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
	/// <summary>
	/// package texture as table
	/// 合成贴图为表
	/// </summary>
	virtual void execute() override;

	texture_component_invoker* texture_component_package_texture_ptr = nullptr;
};
class s_update_texture_gpu_command : public s_command
{
public:
	/// <summary>
	/// Refresh gpu textures
	/// 刷新 gpu 贴图 
	/// </summary>
	virtual void execute() override;

	cpu_texture* texture_update_ptr = nullptr;
};


class s_update_gpu_command : public s_command
{
public:
	/// <summary>
	/// Refresh gpu textures
	/// 刷新 gpu 贴图 
	/// </summary>
	virtual void execute() override;
	//自己局部定义该命令，并使用execute_success获得刷新结果。
	//刷新错误是否影响命令 要求执行命令者自行判断
	//一般来说，执行刷新命令的位置总是对资源有GPU实时要求
	bool execute_success = true;
};

class s_debug_output_refresh_command : public s_command
{
public:
	/// <summary>
	/// Refresh the debug_output window
	/// 刷新debug_output 窗口
	/// </summary>
	virtual void execute() override;
};

class s_draw_command : public s_command
{
public:
	/// <summary>
	/// Traverse pass table to execute pass
	/// 遍历pass表执行pass
	/// </summary>
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
	/// <summary>
	/// delete connected resource
	/// 删除连接资源
	/// </summary>
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
	/// <summary>
	/// Remove all resources connected by disconnect_port
	/// 移除所有disconnect_port连接的资源
	/// </summary>
	virtual void execute() override;

	bool disconnect_success = false;
	connect_port* disconnect_port = nullptr;
};

class s_remove_texture_command : public s_command
{
public:
	/// <summary>
	/// Remove a texture from a texture group
	/// 在贴图组中移除贴图
	/// </summary>
	virtual void execute() override;

	texture_element_invoker* texture_element_delete_ptr = nullptr;

};

class s_remove_texture_group_command : public s_command
{
public:
	/// <summary>
	/// Delete the connected interface
	/// Delete the pointer in the table
	/// Delete component
	/// 删除连接的接口
	/// 删除表中指针
	/// 删除组件
	/// </summary>
	virtual void execute() override;

	texture_component_invoker* texture_component_delete_ptr = nullptr;

};

class s_remove_mesh_command : public s_command
{
public:
	/// <summary>
	/// Delete the connected interface
	/// Delete the pointer in the table
	/// Delete component
	/// 删除连接的接口
	/// 删除表中指针
	/// 删除组件
	/// </summary>
	virtual void execute() override;

	mesh_component_invoker* mesh_component_delete_ptr = nullptr;

};

class s_remove_shader_command : public s_command
{
public:
	/// <summary>
	/// Delete the connected interface
	/// Delete the pointer in the table
	/// Delete component
	/// 删除连接的接口
	/// 删除表中指针
	/// 删除组件
	/// </summary>
	virtual void execute() override;

	shader_component_invoker* shader_component_delete_ptr = nullptr;

};

class s_remove_pass_command : public s_command
{
public:
	/// <summary>
	/// Delete the connected interface
	/// Delete the pointer in the table
	/// Delete component
	/// 删除连接的接口
	/// 删除表中指针
	/// 删除组件
	/// </summary>
	virtual void execute() override;

	pass_component_invoker* pass_component_delete_ptr = nullptr;

};

class s_remove_camera_command : public s_command
{
public:
	/// <summary>
	/// Delete the connected interface
	/// Delete the pointer in the table
	/// Delete component
	/// 删除连接的接口
	/// 删除表中指针
	/// 删除组件
	/// </summary>
	virtual void execute() override;

	camera_component_invoker* camera_component_delete_ptr = nullptr;

};

class s_remove_light_command : public s_command
{
public:
	/// <summary>
	/// Delete the connected interface
	/// Delete the pointer in the table
	/// Delete component
	/// 删除连接的接口
	/// 删除表中指针
	/// 删除组件
	/// </summary>
	virtual void execute() override;

	light_component_invoker* light_component_delete_ptr = nullptr;

};


