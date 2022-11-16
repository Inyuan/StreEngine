#pragma once
#include "s_command.h"

class s_create_texture_command : public s_command
{
public:
	virtual void execute() override;
};

class s_create_mesh_command : public s_command
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

class s_create_shader_commd : public s_command
{
public:
	virtual void execute() override;
};


