#pragma once
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QLabel>
#include <QSpacerItem>
#include <QPushButton>
#include <QLayout>
#include <QMenu>
#include <QAction>
#include <QPainterPath>
#include <QPainter>
#include <vector>
#include <set>
#include "s_command.h"

using std::set;
using std::vector;

class connect_port;
class curve_tool;
class component_invoker;
class texture_component_invoker;
class mesh_component_invoker;
class pass_component_invoker;
class shader_component_invoker;

struct connect_data
{
	const connect_port* port1 = nullptr;
	const connect_port* port2 = nullptr;
	curve_tool* curve = nullptr;
	connect_data(
		const connect_port* in_port1,
		const connect_port* in_port2,
		curve_tool* in_curve) :
		port1(in_port1),
		port2(in_port2),
		curve(in_curve) {};
};

class pipeline_window_invoker : public QWidget
{
public:
	vector<texture_component_invoker*> texture_comp_group;
	vector<pass_component_invoker*> pass_comp_group;
	vector<mesh_component_invoker*> mesh_comp_group;
	vector<shader_component_invoker*> shader_comp_group;

	vector<connect_data> connect_curve_group;

public:
	pipeline_window_invoker(QWidget* in_parent,
		s_command* in_create_pass_cmd,
		s_command* in_create_texture_group_cmd,
		s_command* in_create_mesh_cmd,
		s_command* in_create_shader_cmd);
	

protected:
	pipeline_window_invoker() {};
	QMenu* right_click_menu = nullptr;

	s_command* create_pass_cmd = nullptr;
	s_command* create_texture_group_cmd = nullptr;
	s_command* create_mesh_cmd = nullptr;
	s_command* create_shader_cmd = nullptr;
private:

	virtual void mousePressEvent(QMouseEvent* in_event);

	virtual void paintEvent(QPaintEvent*);
};

class component_invoker : public QGroupBox
{
public:
	component_invoker(QWidget* in_parent);
protected:
	component_invoker() {  };
	virtual void mousePressEvent(QMouseEvent* in_event);
	virtual void mouseReleaseEvent(QMouseEvent* in_event);

};


class pass_component_invoker : public component_invoker
{
public:
	pass_component_invoker(
		QWidget* in_parent,
		s_pass* in_pass_ptr);

	s_pass* pass_instance = nullptr;

protected:
	pass_component_invoker() = delete;
	vector<connect_port*> input_res_port_group;
	connect_port* mesh_port = nullptr;
	connect_port* shader_port = nullptr;
	connect_port* output_port = nullptr;



	//...
};

class mesh_component_invoker : public component_invoker
{
public:
	mesh_component_invoker(QWidget* in_parent,
		cpu_mesh* in_mesh_ptr);
protected:
	mesh_component_invoker() = delete;

	cpu_mesh* mesh_instance = nullptr;

	connect_port* output_port = nullptr;

};

class texture_element_invoker : public QWidget
{
public:
	texture_element_invoker(cpu_texture* in_texture_ptr,int in_height,QWidget* in_parent);


	cpu_texture* texture_instance = nullptr;
protected:
	
	QLabel* texture_name = nullptr;
	connect_port* input_port = nullptr;
	connect_port* output_port = nullptr;


};

class texture_component_invoker : public component_invoker
{
public:
	texture_component_invoker(
		QWidget* in_parent,
		cpu_texture* in_texture_ptr);

	void add_texture();

	void add_element(cpu_texture* in_texture_ptr);

	vector<texture_element_invoker*> textures_group;
	cpu_texture* texture_instance = nullptr;
protected:

	int element_stk_height = 41;

	texture_component_invoker() = delete;

	

	connect_port* input_port = nullptr;
	connect_port* output_port = nullptr;


	
	QMenu* right_click_menu = nullptr;
	
	QPushButton* add_texture_button = nullptr;
	
	s_command* create_texture_cmd = nullptr;
};

class shader_component_invoker : public component_invoker
{
public:
	shader_component_invoker(QWidget* in_parent, const shader_layout& in_shader_layout);
protected:
	shader_component_invoker() = delete;
	connect_port* output_port = nullptr;

	shader_layout shader_layout_instance;
};


//???Ó²×ª»»£¿
struct port_information
{
	

	enum PORT_TYPE
	{	
		TEXTURE_INPUT,
		TEXTURE_OUTPUT,
		TEXTURE_GROUP_INPUT,
		TEXTURE_GROUP_OUTPUT,
		MESH_OUTPUT,
		SHADER_OUTPUT,
		PASS_RES_PORT_GROUP,
		PASS_MESH_INPUT,
		PASS_SHADER_INPUT,
		PASS_OUTPUT,

		PORT_TYPE_NONE,
	}port_type = PORT_TYPE_NONE;



	void* ptr =  nullptr;

	int port_index = 0;

	port_information() {};

	port_information(
		PORT_TYPE in_port_type,
		void* in_ptr,
		int in_port_index = 0) :
		port_type(in_port_type),
		ptr(in_ptr),
		port_index(in_port_index) {};
};

class connect_port : public QRadioButton
{
public:
	const port_information port_inf;
	connect_port(QWidget* in_parent, 
		port_information in_port_inf);

	curve_tool* curve_ptr = nullptr;
protected:
	s_command* connect_res_cmd = nullptr;

private:

	virtual void mousePressEvent(QMouseEvent* in_event);
	virtual void mouseReleaseEvent(QMouseEvent* in_event);

};


class curve_tool
{
public:
	curve_tool(QPoint in_start_port, QPoint in_end_port);

	void change_start(QPoint in_start);

	void change_end(QPoint in_end);

	QPainterPath get_curve()
	{
		return curve;
	}

private:
	QPoint start = QPoint(0,0);
	QPoint end = QPoint(0,0);
	void update();
	curve_tool() {};
	QPainterPath curve;
};


/******************************************
* draw_window
* 
*********************************************/

class view_port_invoker : public QWidget
{
public:
	view_port_invoker(QWidget* in_parent,s_command* in_draw_cmd);

private:
	virtual void paintEvent(QPaintEvent*);

	s_command* draw_cmd = nullptr;
};