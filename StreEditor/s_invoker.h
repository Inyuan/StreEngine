#pragma once
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QLabel>
#include <QtWidgets/QTextEdit>
#include <QSpacerItem>
#include <QPushButton>
#include <QLayout>
#include <QMenu>
#include <QAction>
#include <QPainterPath>
#include <QPainter>
#include <vector>
#include <set>
#include <list>
#include "s_command.h"

using std::set;
using std::vector;
using std::list;

class connect_port;
class curve_tool;
class component_invoker;
class texture_component_invoker;
class mesh_component_invoker;
class pass_component_invoker;
class shader_component_invoker;

enum COMPONENT_TYPE
{
	COMPONENT_TYPE_MESH,
	COMPONENT_TYPE_SHADER,
	COMPONENT_TYPE_TEXTURE_GROUP,
	COMPONENT_TYPE_TEXTURE,
	COMPONENT_TYPE_PASS,
	COMPONENT_TYPE_NONE
};

struct connect_data
{
	connect_port* port1 = nullptr;
	connect_port* port2 = nullptr;
	curve_tool* curve = nullptr;
	connect_data(
		connect_port* in_port1,
		connect_port* in_port2,
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

//1.0
//连接时检查
//插入到树的表中
//有相连时，接入到pass节点中，记录最远的位置level，刷新自己在树表的位置
//断连时，删除front的对应,刷新level 按照level插回到树表中,
//删除时，删除树表的节点，并刷新后面的所有节点
//执行时按照level执行
// 2.0
// //pass 和 texture有互相对应的前后节点表
// //新建pass时加入树表 level为0
// //新连接pass输出到texture输入时,BFS遍历所有后面的pass节点刷新level
// //新连接texture输出到pass输入时,BFS遍历所有后面的pass节点刷新level
// //删除pass输出到texture输入时,BFS遍历所有后面的pass节点刷新level
// //删除texture输出到pass输入时,BFS遍历所有后面的pass节点刷新level
//3.0
//遇到 连接 删除 设置input output时 重构level_map
// level_map归零
// 从start pass开始往后遍历
// output pass最后再画
// 4.0
// 用户自己控制level
//struct pass_node
//{
//public:
//	pass_node(pass_component_invoker* in_pass_comp_ptr) :pass_comp_ptr(in_pass_comp_ptr) {};
//	pass_node() = delete;
//	pass_node(const pass_node&) = delete;
//
//	
//	pass_component_invoker* pass_comp_ptr = nullptr;
//	list<pass_node*> front_passes;
//	list<pass_node*> next_passes;
//};

class pipeline_window_invoker : public QWidget
{
public:
	//全局映射表
	map<string, texture_component_invoker*> texture_comp_group;
	map<string, pass_component_invoker*> pass_comp_group;
	map<string, mesh_component_invoker*> mesh_comp_group;
	map<string,shader_component_invoker*> shader_comp_group;
	
	//树表
	std::set<pass_component_invoker*> running_pass_comp;
	
	//-1为output
	//0为start
	std::map<int,set<pass_component_invoker*>> pass_comp_level_map;

	list<connect_data*> connect_curve_group;

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

	void reconnect();

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
	COMPONENT_TYPE comp_type = COMPONENT_TYPE_NONE;
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
		//if (pass_tree_node) delete(pass_tree_node);
		//pass_tree_node = nullptr;
	}


	s_pass* pass_instance = nullptr;

	void update_res_port(vector<connect_port*>& in_res_port_group);

	int level = 0;
	set<texture_component_invoker*> input_texture_comp_group;
	set<texture_component_invoker*> output_texture_comp_group;
	//pass_node* pass_tree_node = nullptr;

	vector<connect_port*> input_res_port_group;
	connect_port* mesh_port = nullptr;
	connect_port* shader_port = nullptr;
	connect_port* output_port = nullptr;

protected:
	pass_component_invoker() = delete;
	

	//...

private:
	virtual void keyPressEvent(QKeyEvent* in_event);
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

	connect_port* output_port = nullptr;
	cpu_mesh* mesh_instance = nullptr;
protected:
	mesh_component_invoker() = delete;

private:
	virtual void keyPressEvent(QKeyEvent* in_event);
};

class texture_element_invoker : public component_invoker
{
public:

	void set_height(int in_height);

	texture_element_invoker(cpu_texture* in_texture_ptr,int in_height,QWidget* in_parent);

	~texture_element_invoker()
	{
		//QT会自己释放子组件， 无需在析构中手动释放子组件
		if (texture_instance) delete(texture_instance);
		texture_instance = nullptr;
	}

	cpu_texture* texture_instance = nullptr;
	//connect_port* output_port = nullptr;
protected:
	
	QLabel* texture_name = nullptr;
	//单张贴图的输入依靠贴图组
	//connect_port* input_port = nullptr;
	

private:
	virtual void keyPressEvent(QKeyEvent* in_event);
};

class texture_component_invoker : public component_invoker
{
public:
	texture_component_invoker(
		QWidget* in_parent,
		cpu_texture* in_texture_ptr);

	void add_texture();

	void add_element(cpu_texture* in_texture_ptr);

	void remove_element(cpu_texture* in_texture_ptr);

	void update_package();

	~texture_component_invoker()
	{
		//QT会自己释放子组件， 无需在析构中手动释放子组件
		if (texture_instance) delete(texture_instance);
		texture_instance = nullptr;

	}
private:
	virtual void keyPressEvent(QKeyEvent* in_event);

public:

	vector<texture_element_invoker*> textures_group;
	
	//连接时添加
	//断开时删除
	set<pass_component_invoker*> input_pass_comp_group;
	set<pass_component_invoker*> output_pass_comp_group;

	cpu_texture* texture_instance = nullptr;


	connect_port* input_port = nullptr;
	connect_port* output_port = nullptr;

protected:

	int element_stk_height = 41;

	texture_component_invoker() = delete;


	QMenu* right_click_menu = nullptr;
	QPushButton* add_texture_button = nullptr;
};

class shader_component_invoker : public component_invoker
{
public:
	shader_component_invoker(QWidget* in_parent, const shader_layout& in_shader_layout);

	~shader_component_invoker()
	{
		//QT会自己释放子组件，无需在析构中手动释放子组件

	}

	connect_port* output_port = nullptr;

	shader_layout shader_layout_instance;

protected:
	shader_component_invoker() = delete;
	

	

private:
	virtual void keyPressEvent(QKeyEvent* in_event);
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

	const int port_index = 0;

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
	//这个好像只能全局  //执行命令时装入命令的局部执行
	static connect_port* select_connect_port[2];
	static int select_port_index;

	const port_information port_inf;
	connect_port(QWidget* in_parent, 
		port_information in_port_inf);

	~connect_port()
	{
		//防止被删后还被使用
		if (select_connect_port[0] == this)
		{
			select_connect_port[0] = nullptr;
		}
		if (select_connect_port[1] == this)
		{
			select_connect_port[1] = nullptr;
		}
	}

protected:

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



class debug_text_invoker : public QTextEdit
{
public:
	debug_text_invoker(QWidget * in_parent);

};