#pragma once
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QRadioButton>
#include <QMenu>
#include <QAction>
#include <QPainterPath>
#include <QPainter>
#include <vector>
#include "s_command.h"

using std::vector;

class connect_port;
class curve_tool;
class component_invoker;
class texture_component_invoker;
class mesh_component_invoker;
class pass_component_invoker;

class pipeline_window_invoker : public QWidget
{
public:
	vector<texture_component_invoker*> textrue_comp_group;
	vector<pass_component_invoker*> pass_comp_group;
public:
	pipeline_window_invoker(QWidget* in_parent,
		s_command* in_create_pass_cmd,
		s_command* in_create_texture_cmd,
		s_command* in_create_mesh_cmd,
		s_command* in_create_shader_cmd);
	

protected:
	pipeline_window_invoker() {};
	QMenu* right_click_menu = nullptr;

	s_command* create_pass_cmd = nullptr;
	s_command* create_texture_cmd = nullptr;
	s_command* create_mesh_cmd = nullptr;
	s_command* create_shader_cmd = nullptr;
private:
	virtual void mousePressEvent(QMouseEvent* in_event);

};

class component_invoker : public QGroupBox
{
public:
	component_invoker(QWidget* in_parent):
		QGroupBox(in_parent){};
protected:
	component_invoker() {  };
};


class pass_component_invoker : public component_invoker
{
public:
	pass_component_invoker(
		QWidget* in_parent,
		s_pass* in_pass_ptr);

protected:

	vector<connect_port*> input_res_port_group;
	connect_port* mesh_port = nullptr;
	connect_port* shader_port = nullptr;
	connect_port* output_port = nullptr;

	s_pass* pass_instance = nullptr;

	//...
};

class mesh_component_invoker : public component_invoker
{
public:

protected:
	cpu_mesh* mesh_instance;

	connect_port* output = nullptr;

};

class texture_component_invoker : public component_invoker
{
public:
	texture_component_invoker(
		QWidget* in_parent,
		cpu_texture* in_texture_instance) :
		component_invoker(in_parent),
		texture_instance(in_texture_instance) {}
protected:

	connect_port* input_port = nullptr;
	connect_port* output_port = nullptr;

	cpu_texture* texture_instance = nullptr;
};


//???Ó²×ª»»£¿
struct port_information
{
	enum
	{	
		TEXTURE_INPUT,
		TEXTURE_OUTPUT,
		MESH_OUTPUT,
		SHADER_OUTPUT,
		PASS_RES_PORT_GROUP,
		PASS_MESH_INPUT,
		PASS_SHADER_INPUT,
		PASS_OUTPUT,
		PORT_TYPE_NONE,
	}port_type = PORT_TYPE_NONE;
	void* ptr = nullptr;

	int port_index = 0;

	connect_port* port_ptr = nullptr;
};

class connect_port : public QRadioButton
{
public:
	connect_port(QWidget* in_parent, 
		port_information in_port_inf);

	curve_tool* curve_ptr;
protected:
	s_command* connect_res_cmd = nullptr;
private:
	
	const port_information port_inf;
	virtual void mousePressEvent(QMouseEvent* in_event);
	virtual void mouseReleaseEvent(QMouseEvent* in_event);

};


class curve_tool
{
public:
	curve_tool(connect_port* in_start_port, connect_port* in_end_port);

	void change_start(connect_port* in_start_port);

	void change_end(connect_port* in_end_port);

private:
	connect_port* start_port = nullptr;
	connect_port* end_port = nullptr;
	void draw();
	curve_tool() {};
	QPainterPath curve;
	QPainter painter;
};
