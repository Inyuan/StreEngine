#include "s_invoker.h"
#include "concrete_command.h"
#include <QtWidgets/QApplication>
#include <QMouseEvent>
#include <QPen>

/*********************************************
* pipeline_window_invoker
*
*********************************************/

pipeline_window_invoker* pipeline_window_widget_ptr = nullptr;

pipeline_window_invoker::pipeline_window_invoker(
	QWidget* in_parent): QWidget(in_parent)

{
	create_pass_cmd = new s_create_pass_command();
	create_texture_group_cmd = new s_create_texture_group_command();
	create_mesh_cmd = new s_create_mesh_command();
	create_shader_cmd = new s_create_shader_command();
	
	pipeline_window_widget_ptr = this;
	right_click_menu = new QMenu(in_parent);

	QAction* create_pass_act = new QAction("create pass");
	QAction* create_texture_group_act = new QAction("create texture group");
	QAction* create_mesh_act = new QAction("create mesh");
	QAction* create_shader_act = new QAction("create shader");

	right_click_menu->addAction(create_pass_act);
	right_click_menu->addAction(create_texture_group_act);
	right_click_menu->addAction(create_mesh_act);
	right_click_menu->addAction(create_shader_act);

	//链接组件和命令
	connect(create_pass_act, &QAction::triggered, this, [&]() {create_pass_cmd->execute(); });
	connect(create_texture_group_act, &QAction::triggered, this, [&]() {create_texture_group_cmd->execute(); });
	connect(create_mesh_act, &QAction::triggered, this, [&]() {create_mesh_cmd->execute(); });
	connect(create_shader_act, &QAction::triggered, this, [&]() {create_shader_cmd->execute(); });

}

int pipeline_w_mouse_position_x = 0;
int pipeline_w_mouse_position_y = 0;

void pipeline_window_invoker::mousePressEvent(QMouseEvent* in_event)
{
	if (in_event->button() == Qt::LeftButton)
	{
		right_click_menu->close();
	}
	else if (in_event->button() == Qt::RightButton)
	{
		

		QPoint mouse_pos = in_event->globalPos();
		
		pipeline_w_mouse_position_x = in_event->pos().x();
		pipeline_w_mouse_position_y = in_event->pos().y();
		right_click_menu->exec(mouse_pos);
	}
	return QWidget::mousePressEvent(in_event);
}

void pipeline_window_invoker::paintEvent(QPaintEvent*)
{
	//绘制连线
	{	
		QColor line_color = QColor(0x20, 0x20, 0x20, 0xff);
		QPen pen;
		pen.setColor(line_color);
		pen.setWidth(2);

		QPainter p(this);

		for (int i = 0; i < connect_curve_group.size(); i++)
		{
			p.drawPath(connect_curve_group[i]->curve->get_curve());
		}
	}

}

/*********************************************
* component
*
*********************************************/

std::set<component_invoker*> selected_components;

component_invoker::component_invoker(
	QWidget* in_parent) :
	QGroupBox(in_parent)
{
};


void component_invoker::mousePressEvent(QMouseEvent* in_event)
{
	if (in_event->button() == Qt::LeftButton)
	{
		if (selected_components.find(this) != selected_components.end())
		{
			selected_components.erase(this);
		}
		else
		{
			selected_components.insert(this);
		}
	}
	else if (in_event->button() == Qt::RightButton)
	{
		for (auto it : selected_components)
		{

		}
	}
	return QWidget::mousePressEvent(in_event);
}

void component_invoker::mouseReleaseEvent(QMouseEvent* in_event)
{
	if (in_event->button() == Qt::LeftButton)
	{
		//QPoint mouse_pos = in_event->pos();
		//setGeometry(
		//	QRect(
		//		mouse_pos.x(),
		//		mouse_pos.y(),
		//		geometry().width(), 
		//		geometry().height()));
	}
	else if (in_event->button() == Qt::RightButton)
	{

	}
	return QWidget::mousePressEvent(in_event);
}




/*********************************************
* mesh_component
*
*********************************************/

mesh_component_invoker::mesh_component_invoker(
	QWidget* in_parent,
	cpu_mesh* in_mesh_ptr) :
	component_invoker(in_parent),
	mesh_instance(in_mesh_ptr)
{
	//构建组件
	setObjectName("mesh_component");
	setGeometry(QRect(pipeline_w_mouse_position_x, pipeline_w_mouse_position_y, 120, 51));
	setAlignment(Qt::AlignCenter);

	//构建端口
	output_port = new connect_port(
		this,
		port_information(
			port_information::MESH_OUTPUT,
			mesh_instance));
	output_port->setObjectName("mesh_output_port");
	output_port->setGeometry(QRect(54, 30, 61, 20));
	output_port->setLayoutDirection(Qt::RightToLeft);
	output_port->setAutoExclusive(false);
	setTitle(QCoreApplication::translate("stre_editorClass", "mesh", nullptr));
	output_port->setText(QCoreApplication::translate("stre_editorClass", "output", nullptr));

}


/*********************************************
* pass_component
*
*********************************************/

//TODO 编译着色器暴露资源接口

pass_component_invoker::pass_component_invoker(
	QWidget* in_parent,
	s_pass* in_pass_ptr) :
	component_invoker(in_parent),
	pass_instance(in_pass_ptr)
{
	//

	//构建组件
	setObjectName("pass_component");
	setGeometry(QRect(pipeline_w_mouse_position_x, pipeline_w_mouse_position_y, 211, 131));
	setAlignment(Qt::AlignCenter);

	
	//构建端口
	
	//!!!编译了着色器再表现端口
	//pass_resource_port = new QRadioButton(pass_component);
	//pass_resource_port->setObjectName("pass_resource_port");
	//pass_resource_port->setGeometry(QRect(10, 20, 71, 20));

	mesh_port = new connect_port(
		this, 
		port_information(
			port_information::PASS_MESH_INPUT, 
			pass_instance));
	mesh_port->setObjectName("pass_mesh_port");
	mesh_port->setGeometry(QRect(10, 80, 95, 20));
	mesh_port->setAutoExclusive(false);

	shader_port = new connect_port(
		this,
		port_information(
			port_information::PASS_SHADER_INPUT,
			pass_instance));
	shader_port->setObjectName("pass_shade_port");
	shader_port->setGeometry(QRect(10, 100, 95, 20));
	shader_port->setAutoExclusive(false);

	output_port = new connect_port(
		this,
		port_information(
			port_information::PASS_OUTPUT,
			pass_instance));
	output_port->setObjectName("pass_output_port");
	output_port->setGeometry(QRect(144, 100, 61, 20));
	output_port->setLayoutDirection(Qt::RightToLeft);
	output_port->setAutoExclusive(false);

	setTitle(QCoreApplication::translate("stre_editorClass", "pass", nullptr));

	//pass_resource_port->setText(QCoreApplication::translate("stre_editorClass", "res_port", nullptr));
	
	mesh_port->setText(QCoreApplication::translate("stre_editorClass", "mesh_port", nullptr));
	shader_port->setText(QCoreApplication::translate("stre_editorClass", "shader_port", nullptr));
	output_port->setText(QCoreApplication::translate("stre_editorClass", "output", nullptr));

}

/*********************************************
* texture_component
*
*********************************************/

texture_component_invoker* current_texture_component_ptr = nullptr;

texture_element_invoker::texture_element_invoker(
	cpu_texture* in_texture_ptr,
	int in_height, 
	QWidget* in_parent) :
	texture_instance(in_texture_ptr),
	QWidget(in_parent)
{
	setObjectName("texture_element_widget");
	setGeometry(QRect(0, in_height, 121, 41));


	//构建名字标签
	texture_name = new QLabel(this);
	texture_name->setObjectName("texture_name");
	texture_name->setGeometry(QRect(0, 0, 121, 16));

	//构建端口
	input_port = new connect_port(this, port_information(port_information::TEXTURE_INPUT, texture_instance));
	input_port->setObjectName("texture_input_port");
	input_port->setGeometry(QRect(0, 20, 51, 20));
	input_port->setLayoutDirection(Qt::LeftToRight);
	input_port->setAutoExclusive(false);
	input_port->setText(QCoreApplication::translate("stre_editorClass", "input", nullptr));

	output_port = new connect_port(this, port_information(port_information::TEXTURE_OUTPUT, texture_instance));
	output_port->setObjectName("texture_output_port");
	output_port->setGeometry(QRect(60, 20, 61, 20));
	output_port->setLayoutDirection(Qt::RightToLeft);
	output_port->setAutoExclusive(false);
	output_port->setText(QCoreApplication::translate("stre_editorClass", "output", nullptr));

}


texture_component_invoker::texture_component_invoker(
	QWidget* in_parent,
	cpu_texture* in_texture_ptr) :
	texture_instance(in_texture_ptr),
	component_invoker(in_parent)
{

	//构建组件
	setObjectName("texture_group_component");
	setGeometry(QRect(
		pipeline_w_mouse_position_x, 
		pipeline_w_mouse_position_y, 
		121, 211));
	setAlignment(Qt::AlignCenter);
	setTitle(QCoreApplication::translate("stre_editorClass", "texture", nullptr));

	create_texture_cmd = new s_create_texture_command();

	//构建端口
	input_port = new connect_port(this, port_information(port_information::TEXTURE_GROUP_INPUT, this));
	input_port->setObjectName("texture_input_port");
	input_port->setGeometry(QRect(0, 20, 51, 20));
	input_port->setLayoutDirection(Qt::LeftToRight);
	input_port->setAutoExclusive(false);
	input_port->setText(QCoreApplication::translate("stre_editorClass", "input", nullptr));

	output_port = new connect_port(this, port_information(port_information::TEXTURE_GROUP_OUTPUT, this));
	output_port->setObjectName("texture_output_port");
	output_port->setGeometry(QRect(60, 20, 61, 20));
	output_port->setLayoutDirection(Qt::RightToLeft);
	output_port->setAutoExclusive(false);
	output_port->setText(QCoreApplication::translate("stre_editorClass", "output", nullptr));

	//构建按钮
	add_texture_button = new QPushButton(this);
	add_texture_button->setObjectName("add_texture_button");
	add_texture_button->setGeometry(QRect(0, 190, 121, 24));
	add_texture_button->setText(QCoreApplication::translate("stre_editorClass", "add texture", nullptr));

	connect(add_texture_button, &QPushButton::clicked, this, &texture_component_invoker::add_texture);

	//构建右键菜单

	right_click_menu = new QMenu(in_parent);

	QAction* package_texture_act = new QAction("create texture");

	right_click_menu->addAction(package_texture_act);

	connect(package_texture_act, &QAction::triggered, this, &texture_component_invoker::add_texture);

}

void texture_component_invoker::add_element(cpu_texture* in_texture_ptr)
{
	textures_group.push_back(
		new texture_element_invoker(
			in_texture_ptr, 
			element_stk_height,
			this));

	textures_group.back()->show();

	element_stk_height += 41;
}

void texture_component_invoker::add_texture()
{
	current_texture_component_ptr = this;

	create_texture_cmd->execute();

	current_texture_component_ptr = nullptr;
}

/*********************************************
* shader_component
*
*********************************************/

shader_component_invoker::shader_component_invoker(
	QWidget* in_parent, 
	const shader_layout& in_shader_layout) :
	component_invoker(in_parent),
	shader_layout_instance(in_shader_layout)
{
	//构建组件
	setObjectName("shader_component");
	setGeometry(QRect(pipeline_w_mouse_position_x, pipeline_w_mouse_position_y, 120, 51));
	setAlignment(Qt::AlignCenter);

	//构建端口
	output_port = new connect_port(this, port_information(port_information::SHADER_OUTPUT, &shader_layout_instance));
	output_port->setObjectName("shader_output_port");
	output_port->setGeometry(QRect(54, 30, 61, 20));
	output_port->setLayoutDirection(Qt::RightToLeft);
	output_port->setAutoExclusive(false);
	setTitle(QCoreApplication::translate("stre_editorClass", "shader", nullptr));
	output_port->setText(QCoreApplication::translate("stre_editorClass", "output", nullptr));

}




/*********************************************
* connect_port
* 
*********************************************/

connect_port* select_connect_port[2] = { nullptr };

int select_port_index = 0;

connect_port::connect_port(
	QWidget* in_parent,
	port_information in_port_inf):
	QRadioButton(in_parent),
	port_inf(in_port_inf)
{
	connect_res_cmd = new s_connect_resource_command();

}

void connect_port::mousePressEvent(QMouseEvent* in_event)
{

	if (in_event->button() == Qt::LeftButton)
	{
		switch (select_port_index)
		{
		case 0:
			select_connect_port[0] = this;
			setChecked(true);
			select_port_index++;
			break;
		case 1:
			if (select_connect_port[0] == this)
			{
				select_connect_port[0] = nullptr;
				setChecked(false);
				select_port_index = 0;
				return;
			}
			select_connect_port[1] = this;
			setChecked(true);
			select_port_index++;
			connect_res_cmd->execute();
		default:
			select_connect_port[0]->setChecked(false);
			select_connect_port[1]->setChecked(false);
			select_connect_port[0] = nullptr;
			select_connect_port[1] = nullptr;
			select_port_index = 0;
			break;
		}
	}
	


	return;
}

void connect_port::mouseReleaseEvent(QMouseEvent* in_event)
{

	return;//QRadioButton::mouseReleaseEvent(in_event);
}


curve_tool::curve_tool(QPoint in_start, QPoint in_end)
{
	start = in_start;
	
	end = in_end;

	update();
}

void curve_tool::change_start(QPoint in_start)
{
	start = in_start;
	update();
}

void curve_tool::change_end(QPoint in_end)
{
	end = in_end;
	update();
}

void curve_tool::update()
{

	QPoint mid_pos = (start + end) / 2;

	curve.moveTo(start);
	curve.cubicTo(QPointF(mid_pos.x(), start.y()), mid_pos, end);
}

/*********************************************
* view_port_invoker
*
*********************************************/

view_port_invoker::view_port_invoker(QWidget* in_parent):QWidget(in_parent)
{
	draw_cmd = new s_draw_command();

	setObjectName("rendering_view_widget");
	setGeometry(QRect(570, 10, 341, 281));
}

void view_port_invoker::paintEvent(QPaintEvent* in_event)
{
	draw_cmd->execute();
}