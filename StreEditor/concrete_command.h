#pragma once
#include "s_command.h"

//具体命令

//创建贴图
class s_create_texture_command : public s_command
{
public:
	virtual void execute() override;
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
};

class s_reconnect_resource_command : public s_command
{
public:
	virtual void execute() override;
};

class s_create_shader_command : public s_command
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
};

class s_remove_texture_command : public s_command
{
public:
	virtual void execute() override;
};

class s_remove_texture_group_command : public s_command
{
public:
	virtual void execute() override;
};

class s_remove_mesh_command : public s_command
{
public:
	virtual void execute() override;
};

class s_remove_shader_command : public s_command
{
public:
	virtual void execute() override;
};

class s_remove_pass_command : public s_command
{
public:
	virtual void execute() override;
};

