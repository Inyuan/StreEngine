#include "s_invoker.h"
#include "concrete_command.h"
#include <QMouseEvent>
#include <QPen>
/*********************************************
* pipeline_window_invoker
*
*********************************************/

pipeline_window_invoker* pipeline_window_widget_ptr = nullptr;

pipeline_window_invoker::pipeline_window_invoker(
	QWidget* in_parent,
	s_command* in_create_pass_cmd,
	s_command* in_create_texture_cmd,
	s_command* in_create_mesh_cmd,
	s_command* in_create_shader_cmd): QWidget(in_parent),
	create_pass_cmd(in_create_pass_cmd),
	create_texture_cmd(in_create_texture_cmd),
	create_mesh_cmd(in_create_mesh_cmd),
	create_shader_cmd(in_create_shader_cmd)
{
	pipeline_window_widget_ptr = this;
	right_click_menu = new QMenu(in_parent);

	QAction* create_pass_act = new QAction("create pass");
	QAction* create_texture_act = new QAction("create texture");
	QAction* create_mesh_act = new QAction("create mesh");
	QAction* create_shader_act = new QAction("create shader");

	right_click_menu->addAction(create_pass_act);
	right_click_menu->addAction(create_texture_act);
	right_click_menu->addAction(create_mesh_act);
	right_click_menu->addAction(create_shader_act);

	//链接组件和命令
	connect(create_pass_act, &QAction::triggered, this, [&]() {create_pass_cmd->execute(); });
	connect(create_texture_act, &QAction::triggered, this, [&]() {create_texture_cmd->execute(); });
	connect(create_mesh_act, &QAction::triggered, this, [&]() {create_mesh_cmd->execute(); });
	connect(create_shader_act, &QAction::triggered, this, [&]() {create_shader_cmd->execute(); });

}

int mouse_position_x = 0;
int mouse_position_y = 0;

void pipeline_window_invoker::mousePressEvent(QMouseEvent* in_event)
{
	if (in_event->button() == Qt::LeftButton)
	{
		right_click_menu->close();
	}
	else if (in_event->button() == Qt::RightButton)
	{
		
		QPoint mouse_pos = in_event->globalPos();
		
		right_click_menu->exec(mouse_pos);
	}
	return QWidget::mousePressEvent(in_event);
}


pass_component_invoker::pass_component_invoker(
	QWidget* in_parent,
	s_pass* in_pass_ptr) :
	component_invoker(in_parent),
	pass_instance(in_pass_ptr)
{
	//构建端口


	//
}


/*********************************************
* connect_port
* 
*********************************************/

port_information connect_port1;
port_information connect_port2;

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
	connect_port1 = port_inf;

	return QRadioButton::mousePressEvent(in_event);
}
void connect_port::mouseReleaseEvent(QMouseEvent* in_event)
{
	connect_port2 = port_inf;
	connect_res_cmd->execute();
	return QRadioButton::mouseReleaseEvent(in_event);
}


curve_tool::curve_tool(connect_port* in_start_port, connect_port* in_end_port)
{
	start_port = in_start_port;
	start_port->curve_ptr = this;
	
	end_port = in_end_port;
	end_port->curve_ptr = this;

	QColor line_color = QColor(0x20, 0x20, 0x20, 0xff);
	QPen pen;
	pen.setColor(line_color);
	pen.setWidth(2);
	painter.setPen(pen);

	draw();
}

void curve_tool::change_start(connect_port* in_start_port)
{
	start_port = in_start_port;
	draw();
}

void curve_tool::change_end(connect_port* in_end_port)
{
	end_port = in_end_port;
	draw();
}

void curve_tool::draw()
{
	QPoint start = start_port->pos();
	QPoint end = end_port->pos();
	QPoint mid_pos = (start + end) / 2;

	curve.moveTo(start);
	curve.cubicTo(QPointF(mid_pos.x(), start.y()), mid_pos, end);
	painter.drawPath(curve);
}