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

	~connect_data()
	{
		if (curve) delete(curve);
		curve = nullptr;
		port1 = nullptr;
		port2 = nullptr;
	}
};

class pipeline_window_invoker : public QWidget
{
public:
	vector<texture_component_invoker*> texture_comp_group;
	vector<pass_component_invoker*> pass_comp_group;
	vector<mesh_component_invoker*> mesh_comp_group;
	vector<shader_component_invoker*> shader_comp_group;

	vector<connect_data*> connect_curve_group;

public:
	pipeline_window_invoker(QWidget* in_parentd);
	
	~pipeline_window_invoker()
	{
		//QT会自己释放子组件， 无需在析构中手动释放子组件
		for (auto it : connect_curve_group)
		{
			if (it) delete(it);
			it = nullptr;
		}

		if (create_pass_cmd) delete(create_pass_cmd);
		if (create_texture_group_cmd) delete(create_texture_group_cmd);
		if (create_mesh_cmd) delete(create_mesh_cmd);
		if (create_shader_cmd) delete(create_shader_cmd);

		create_pass_cmd = nullptr;
		create_texture_group_cmd = nullptr;
		create_mesh_cmd = nullptr;
		create_shader_cmd = nullptr;
	}

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

	~pass_component_invoker()
	{
		//QT会自己释放子组件， 无需在析构中手动释放子组件
		if (pass_instance) delete(pass_instance);
		pass_instance = nullptr;
	}


	s_pass* pass_instance = nullptr;

	void update_res_port(vector<connect_port*>& in_res_port_group);

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

	~mesh_component_invoker()
	{
		//QT会自己释放子组件， 无需在析构中手动释放子组件
		if (mesh_instance) delete(mesh_instance);
		mesh_instance = nullptr;
	}

protected:
	mesh_component_invoker() = delete;

	cpu_mesh* mesh_instance = nullptr;

	connect_port* output_port = nullptr;

};

class texture_element_invoker : public QWidget
{
public:
	texture_element_invoker(cpu_texture* in_texture_ptr,int in_height,QWidget* in_parent);

	~texture_element_invoker()
	{
		//QT会自己释放子组件， 无需在析构中手动释放子组件
		if (texture_instance) delete(texture_instance);
		texture_instance = nullptr;
	}

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

	~texture_component_invoker()
	{
		//QT会自己释放子组件， 无需在析构中手动释放子组件
		if (texture_instance) delete(texture_instance);
		texture_instance = nullptr;

		if (create_texture_cmd) delete(create_texture_cmd);
		create_texture_cmd = nullptr;
	}

public:

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

	~shader_component_invoker()
	{
		//QT会自己释放子组件，无需在析构中手动释放子组件

	}

protected:
	shader_component_invoker() = delete;
	connect_port* output_port = nullptr;

	shader_layout shader_layout_instance;
};


//???硬转换？
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

	~port_information()
	{

	}

};

class connect_port : public QRadioButton
{
public:
	const port_information port_inf;
	connect_port(QWidget* in_parent, 
		port_information in_port_inf);

	~connect_port()
	{
		if (connect_res_cmd) delete(connect_res_cmd);
		connect_res_cmd = nullptr;
	}

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
	view_port_invoker(QWidget* in_parent);

	~view_port_invoker()
	{
		if(draw_cmd) delete(draw_cmd);
		draw_cmd = nullptr;
	}

private:
	virtual void paintEvent(QPaintEvent*);

	s_command* draw_cmd = nullptr;
};