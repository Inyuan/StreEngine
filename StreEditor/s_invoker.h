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
#include "stre_engine.h"
#include "RObject.h"


using std::set;
using std::vector;
using std::list;

//
// ��������̣�
// 1.cpu_����
// 2.invoker��,��Ա�˿ڣ�����
// 3.�������ö�٣��˿�����ö��
// 4.stre_engine�ײ��ṩ�������ܱ�¶
// 5.ɾ�����ܣ������������������,
// 6.���ߴ��幹�������ӣ��Ͽ�����
// 7.���Դ���



class connect_port;
class curve_tool;
class component_invoker;
class texture_component_invoker;
class mesh_component_invoker;
class pass_component_invoker;
class shader_component_invoker;
class camera_component_invoker;
class light_component_invoker;



enum COMPONENT_TYPE
{
	COMPONENT_TYPE_MESH,
	COMPONENT_TYPE_CAMERA,
	COMPONENT_TYPE_LIGHT,
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
//����ʱ���
//���뵽���ı���
//������ʱ�����뵽pass�ڵ��У���¼��Զ��λ��level��ˢ���Լ��������λ��
//����ʱ��ɾ��front�Ķ�Ӧ,ˢ��level ����level��ص�������,
//ɾ��ʱ��ɾ������Ľڵ㣬��ˢ�º�������нڵ�
//ִ��ʱ����levelִ��
// 2.0
// //pass �� texture�л����Ӧ��ǰ��ڵ��
// //�½�passʱ�������� levelΪ0
// //������pass�����texture����ʱ,BFS�������к����pass�ڵ�ˢ��level
// //������texture�����pass����ʱ,BFS�������к����pass�ڵ�ˢ��level
// //ɾ��pass�����texture����ʱ,BFS�������к����pass�ڵ�ˢ��level
// //ɾ��texture�����pass����ʱ,BFS�������к����pass�ڵ�ˢ��level
//3.0
//���� ���� ɾ�� ����input outputʱ �ع�level_map
// level_map����
// ��start pass��ʼ�������
// output pass����ٻ�
// 4.0
// �û��Լ�����level
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
	//ȫ��ӳ���
	map<string, texture_component_invoker*> texture_comp_group;
	map<string, pass_component_invoker*> pass_comp_group;
	map<string, mesh_component_invoker*> mesh_comp_group;
	map<string, shader_component_invoker*> shader_comp_group;
	map<string, camera_component_invoker*> camera_comp_group;
	map<string, light_component_invoker*> light_comp_group;

	//-1Ϊoutput
	//0Ϊstart
	////����
	std::map<int,set<pass_component_invoker*>> pass_comp_level_map;

	list<connect_data*> connect_curve_group;

public:
	pipeline_window_invoker(QWidget* in_parentd);
	
	~pipeline_window_invoker()
	{
		//QT���Լ��ͷ�������� �������������ֶ��ͷ������
		for (auto it : connect_curve_group)
		{
			if (it) delete(it);
			it = nullptr;
		}

		if (create_pass_cmd) delete(create_pass_cmd);
		if (create_texture_group_cmd) delete(create_texture_group_cmd);
		if (create_mesh_cmd) delete(create_mesh_cmd);
		if (create_shader_cmd) delete(create_shader_cmd);
		if (create_camera_cmd) delete(create_camera_cmd);
		if (create_light_cmd) delete(create_light_cmd);

		create_pass_cmd = nullptr;
		create_texture_group_cmd = nullptr;
		create_mesh_cmd = nullptr;
		create_shader_cmd = nullptr;
		create_camera_cmd = nullptr;
		create_light_cmd = nullptr;
	}

	void reconnect();

protected:
	pipeline_window_invoker() {};
	QMenu* right_click_menu = nullptr;

	s_command* create_pass_cmd = nullptr;
	s_command* create_texture_group_cmd = nullptr;
	s_command* create_mesh_cmd = nullptr;
	s_command* create_shader_cmd = nullptr;
	s_command* create_camera_cmd = nullptr;
	s_command* create_light_cmd = nullptr;
private:

	virtual void mousePressEvent(QMouseEvent* in_event);

	virtual void paintEvent(QPaintEvent*);
};

class component_invoker : public QGroupBox
{
public:
	COMPONENT_TYPE comp_type = COMPONENT_TYPE_NONE;
	component_invoker(QWidget* in_parent);
	virtual ~component_invoker();
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
		//QT���Լ��ͷ�������� �������������ֶ��ͷ������
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
		//QT���Լ��ͷ�������� �������������ֶ��ͷ������
		if (mesh_instance) delete(mesh_instance);
		mesh_instance = nullptr;
	}

	connect_port* output_port = nullptr;
	connect_port* constants_output_port = nullptr;
	connect_port* material_output_port = nullptr;
	cpu_mesh* mesh_instance = nullptr;
	RObject object_helper;
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
		//QT���Լ��ͷ�������� �������������ֶ��ͷ������
		if (texture_instance) delete(texture_instance);
		texture_instance = nullptr;
	}

	cpu_texture* texture_instance = nullptr;
	//connect_port* output_port = nullptr;
protected:
	
	QLabel* texture_name = nullptr;
	//������ͼ������������ͼ��
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
		//QT���Լ��ͷ�������� �������������ֶ��ͷ������
		if (texture_instance) delete(texture_instance);
		texture_instance = nullptr;

	}
private:
	virtual void keyPressEvent(QKeyEvent* in_event);

public:

	vector<texture_element_invoker*> textures_group;
	
	//����ʱ���
	//�Ͽ�ʱɾ��
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
		//QT���Լ��ͷ���������������������ֶ��ͷ������

	}

	connect_port* output_port = nullptr;

	shader_layout shader_layout_instance;

protected:
	shader_component_invoker() = delete;
	

	

private:
	virtual void keyPressEvent(QKeyEvent* in_event);
};

class camera_component_invoker : public component_invoker
{
public:
	camera_component_invoker(QWidget* in_parent, cpu_camera* in_camera_ptr);

	~camera_component_invoker()
	{
		//QT���Լ��ͷ���������������������ֶ��ͷ������
		if (camera_instance)
		{
			delete(camera_instance);
		}

		camera_instance = nullptr;
	}

	connect_port* output_port = nullptr;

	RCamera camera_cal_helper;

	cpu_camera* camera_instance = nullptr;

protected:
	camera_component_invoker() = delete;

private:
	virtual void keyPressEvent(QKeyEvent* in_event);
};

class light_component_invoker : public component_invoker
{
public:
	light_component_invoker(QWidget* in_parent, cpu_light* in_light_ptr);

	~light_component_invoker()
	{
		//QT���Լ��ͷ���������������������ֶ��ͷ������
		if (light_instance)
		{
			delete(light_instance);
		}

		light_instance = nullptr;
	}

	connect_port* output_port = nullptr;

	cpu_light* light_instance = nullptr;

protected:
	light_component_invoker() = delete;

private:
	virtual void keyPressEvent(QKeyEvent* in_event);
};

/******************************************
* connect port
*
*********************************************/

class connect_port : public QRadioButton
{
public:

	//�������ֻ��ȫ��  //ִ������ʱװ������ľֲ�ִ��
	//����������ͬ���͵Ľӿ��������Բ�����ģ��
	static connect_port* select_connect_port[2]; 
	static int select_port_index;

	enum PORT_TYPE
	{
		TEXTURE_INPUT,
		TEXTURE_OUTPUT,
		TEXTURE_GROUP_INPUT,
		TEXTURE_GROUP_OUTPUT,
		MESH_OUTPUT,
		MESH_CONTANTS_OUTPUT,
		MESH_MATERIAL_OUTPUT,
		SHADER_OUTPUT,
		CAMERA_OUTPUT,
		LIGHT_OUTPUT,
		PASS_RES_PORT_GROUP,
		PASS_MESH_INPUT,
		PASS_SHADER_INPUT,
		PASS_OUTPUT,

		PORT_TYPE_NONE,
	}port_type = PORT_TYPE_NONE;



	void* ptr = nullptr;

	const int port_index = 0;

	connect_port(QWidget* in_parent, 
		PORT_TYPE in_port_type,
		void* in_ptr,
		int in_port_index = 0);

	~connect_port()
	{
		//��ֹ��ɾ�󻹱�ʹ��
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
	virtual void mousePressEvent(QMouseEvent* in_event);
	virtual void mouseReleaseEvent(QMouseEvent* in_event);
	virtual void mouseMoveEvent(QMouseEvent* in_event);
	QPoint mouse_point = QPoint(0,0);
	bool is_mouse_down = false;
	s_command* draw_cmd = nullptr;
};



class debug_text_invoker : public QTextEdit
{
public:
	debug_text_invoker(QWidget * in_parent);

};