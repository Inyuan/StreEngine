#pragma once
#include "s_command.h"
#include "s_invoker.h"
//��������

/******************************************
* create command
*
*********************************************/

class s_create_texture_command : public s_command
{
public:
	/// <summary>
	/// Add textures to the texture component
	/// ����ͼ����������ͼ
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
	/// ����texture group���
	/// </summary>
	virtual void execute() override;
};

class s_create_mesh_command : public s_command
{
public:
	/// <summary>
	/// Create objects Create CPU resources + component ontology
	/// �������� ����CPU��Դ + �������
	/// </summary>
	virtual void execute() override;
};

class s_create_pass_command : public s_command
{
public:
	/// <summary>
	/// build pass
	/// ����pass
	/// </summary>
	/// <param name=""></param>
	virtual void execute() override;
};

class s_create_shader_command : public s_command
{
public:
	/// <summary>
	/// Create shaders
	/// ����shader
	/// </summary>
	virtual void execute() override;
};

class s_create_camera_command : public s_command
{
public:
	/// <summary>
	/// Create camera
	/// ���� camera
	/// </summary>
	virtual void execute() override;
};

class s_create_light_command : public s_command
{
public:
	/// <summary>
	/// Create light
	/// ���� light
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
	/// ˢ�� ���
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
	/// ˢ��GPU�ƹ����� 
	/// </summary>
	virtual void execute() override;
	light_component_invoker* light_ptr = nullptr;
};

class s_change_mesh_type_command : public s_command
{
public:
	/// <summary>
	/// Modify mesh type
	/// �޸�ģ������
	/// </summary>
	virtual void execute() override;
};

class s_update_mesh_data_command : public s_command
{
public:
	/// <summary>
	/// update mesh data
	/// ˢ��GPUģ������ 
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
	/// ѡ�����ĸ����
	/// �������� ����ֵ�����Դ���
	/// </summary>
	virtual void execute() override;
};

class s_connect_resource_command : public s_command
{
public:
	/// <summary>
	/// Two port connection, check, add resources
	/// �����˿����ӣ���飬�����Դ
	/// </summary>
	virtual void execute() override;

	connect_port* select_connect_port[2] = { nullptr };
private:
	
	int try_time = 0; //�ݹ����
};


class s_reconnect_resource_command : public s_command
{
public:
	/// <summary>
	/// Delete and connect again Perform the connection operation again
	/// ɾ���������� ����ִ��һ�����Ӳ���
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
	/// �ϳ���ͼΪ��
	/// </summary>
	virtual void execute() override;

	texture_component_invoker* texture_component_package_texture_ptr = nullptr;
};
class s_update_texture_gpu_command : public s_command
{
public:
	/// <summary>
	/// Refresh gpu textures
	/// ˢ�� gpu ��ͼ 
	/// </summary>
	virtual void execute() override;

	cpu_texture* texture_update_ptr = nullptr;
};


class s_update_gpu_command : public s_command
{
public:
	/// <summary>
	/// Refresh gpu textures
	/// ˢ�� gpu ��ͼ 
	/// </summary>
	virtual void execute() override;
	//�Լ��ֲ�����������ʹ��execute_success���ˢ�½����
	//ˢ�´����Ƿ�Ӱ������ Ҫ��ִ�������������ж�
	//һ����˵��ִ��ˢ�������λ�����Ƕ���Դ��GPUʵʱҪ��
	bool execute_success = true;
};

class s_debug_output_refresh_command : public s_command
{
public:
	/// <summary>
	/// Refresh the debug_output window
	/// ˢ��debug_output ����
	/// </summary>
	virtual void execute() override;
};

class s_draw_command : public s_command
{
public:
	/// <summary>
	/// Traverse pass table to execute pass
	/// ����pass��ִ��pass
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
	/// ɾ��������Դ
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
	/// �Ƴ�����disconnect_port���ӵ���Դ
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
	/// ����ͼ�����Ƴ���ͼ
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
	/// ɾ�����ӵĽӿ�
	/// ɾ������ָ��
	/// ɾ�����
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
	/// ɾ�����ӵĽӿ�
	/// ɾ������ָ��
	/// ɾ�����
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
	/// ɾ�����ӵĽӿ�
	/// ɾ������ָ��
	/// ɾ�����
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
	/// ɾ�����ӵĽӿ�
	/// ɾ������ָ��
	/// ɾ�����
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
	/// ɾ�����ӵĽӿ�
	/// ɾ������ָ��
	/// ɾ�����
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
	/// ɾ�����ӵĽӿ�
	/// ɾ������ָ��
	/// ɾ�����
	/// </summary>
	virtual void execute() override;

	light_component_invoker* light_component_delete_ptr = nullptr;

};


