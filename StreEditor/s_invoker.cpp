#include "s_invoker.h"
#include "concrete_command.h"
#include <QtWidgets/QApplication>
#include <QMouseEvent>
#include <QPen>

/*********************************************
* pipeline_window_invoker
*
*********************************************/

//这个地方对多线程来说是噩梦
//需要整理一下资源关系
int pipeline_w_mouse_position_x = 0;
int pipeline_w_mouse_position_y = 0;

pipeline_window_invoker* pipeline_window_widget_ptr = nullptr;
debug_text_invoker* debug_text_ptr = nullptr;
component_invoker* current_component_ptr = nullptr;
view_port_invoker* view_port_widget_ptr = nullptr;

pipeline_window_invoker::pipeline_window_invoker(
	QWidget* in_parent): QWidget(in_parent)
{
	create_pass_cmd = new s_create_pass_command();
	create_texture_group_cmd = new s_create_texture_group_command();
	create_mesh_cmd = new s_create_mesh_command();
	create_shader_cmd = new s_create_shader_command();
	create_camera_cmd = new s_create_camera_command();
	create_light_cmd = new s_create_light_command();

	pipeline_window_widget_ptr = this;
	right_click_menu = new QMenu(this);

	QAction* create_pass_act = new QAction("create pass");
	QAction* create_texture_group_act = new QAction("create texture group");
	QAction* create_mesh_act = new QAction("create mesh");
	QAction* create_shader_act = new QAction("create shader");
	QAction* create_camera_act = new QAction("create camera");
	QAction* create_light_act = new QAction("create light");

	right_click_menu->addAction(create_pass_act);
	right_click_menu->addAction(create_texture_group_act);
	right_click_menu->addAction(create_mesh_act);
	right_click_menu->addAction(create_shader_act);
	right_click_menu->addAction(create_camera_act);
	right_click_menu->addAction(create_light_act);

	//链接组件和命令
	connect(create_pass_act, &QAction::triggered, this, [&]() {create_pass_cmd->execute(); });
	connect(create_texture_group_act, &QAction::triggered, this, [&]() {create_texture_group_cmd->execute(); });
	connect(create_mesh_act, &QAction::triggered, this, [&]() {create_mesh_cmd->execute(); });
	connect(create_shader_act, &QAction::triggered, this, [&]() {create_shader_cmd->execute(); });
	connect(create_camera_act, &QAction::triggered, this, [&]() {create_camera_cmd->execute(); });
	connect(create_light_act, &QAction::triggered, this, [&]() {create_light_cmd->execute(); });

}

/// <summary>
/// 左键关闭菜单，右键菜单
/// </summary>
/// <param name="in_event"></param>
void pipeline_window_invoker::mousePressEvent(QMouseEvent* in_event)
{
	if (in_event->button() == Qt::LeftButton)
	{
		right_click_menu->close();
	}
	else if (in_event->button() == Qt::RightButton)
	{
		right_click_menu->close();

		QPoint mouse_pos = in_event->globalPos();
		
		pipeline_w_mouse_position_x = in_event->pos().x();
		pipeline_w_mouse_position_y = in_event->pos().y();
		right_click_menu->exec(mouse_pos);
	}
	return QWidget::mousePressEvent(in_event);
}

void pipeline_window_invoker::mouseMoveEvent(QMouseEvent* in_event)
{

		if (current_component_ptr && current_component_ptr->is_mouse_down)
		{
			current_component_ptr->setGeometry(
				QRect(
					current_component_ptr->geometry().x() + (in_event->x() - current_component_ptr->mouse_point.x()),
					current_component_ptr->geometry().y() + (in_event->y() - current_component_ptr->mouse_point.y()),
					current_component_ptr->geometry().width(),
					current_component_ptr->geometry().height()));

			current_component_ptr->mouse_point = in_event->pos();

			for (auto it : connect_curve_group)
			{
				if (it->port1->parent() == current_component_ptr || it->port2->parent() == current_component_ptr)
				{
					QPoint start = it->port1->mapToGlobal(QPoint(0, 0));

					QPoint end = it->port2->mapToGlobal(QPoint(0, 0));

					//默认sart组件在end组件的左边
					int local_start_x = it->port1->width();
					int local_end_x = 0;
					//如果在右边就起始位置修改一下
					if (start.x() > end.x())
					{
						local_start_x = 0;
						local_end_x = it->port2->width();
					}

					start = it->port1->mapToGlobal(QPoint(local_start_x, it->port1->height() / 2));
					start = pipeline_window_widget_ptr->mapFromGlobal(start);

					end = it->port2->mapToGlobal(QPoint(local_end_x, it->port2->height() / 2));
					end = pipeline_window_widget_ptr->mapFromGlobal(end);

					it->curve->change_start(start);
					it->curve->change_end(end);
				}
			}

			update();
		}

}


/// <summary>
/// 绘制曲线
/// </summary>
/// <param name=""></param>
void pipeline_window_invoker::paintEvent(QPaintEvent* in_event)
{
	


	
	{	
		QColor line_color = QColor(0x20, 0x20, 0x20, 0xff);
		QPen pen;
		pen.setColor(line_color);
		pen.setWidth(2);
		
		QPainter p(this);
		//清理区域
		//const QRect& rect = in_event->rect();
		//p.eraseRect(rect);

		//绘制连线
		for (auto it : connect_curve_group)
		{
			p.drawPath(it->curve->get_curve());
		}
		
	}

}

/*********************************************
* component
*
*********************************************/

std::set<component_invoker*> selected_components;

component_invoker::~component_invoker()
{
	if (current_component_ptr == this)
	{
		current_component_ptr = nullptr;
	}
}

component_invoker::component_invoker(
	QWidget* in_parent) :
	QGroupBox(in_parent)
{
	setStyleSheet("border: 1px solid gray; border - radius: 5px; ");
	setFocusPolicy(Qt::ClickFocus);
};

/// <summary>
/// 左键选中显示属性
/// </summary>
/// <param name="in_event"></param>
void component_invoker::mousePressEvent(QMouseEvent* in_event)
{
	if (in_event->button() == Qt::LeftButton)
	{
		if(current_component_ptr)
	current_component_ptr->setStyleSheet("border: 1px solid gray; border - radius: 5px; ");
		current_component_ptr = this;
		current_component_ptr->setStyleSheet("border: 2px solid gray; border - radius: 5px; ");
		s_switch_property_widget_command().execute();

		if (comp_type != COMPONENT_TYPE_TEXTURE)
		{
			mouse_point = mapToParent(in_event->pos());
			is_mouse_down = true;
		}
	}



	// 不许沿父节点往上传递点击事件
	//return QWidget::mousePressEvent(in_event);
}



/// <summary>
/// useless
/// </summary>
/// <param name="in_event"></param>
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

		if (comp_type != COMPONENT_TYPE_TEXTURE)
		{
			mouse_point = mapToParent(in_event->pos());
			is_mouse_down = false;
		}
	}
	//else if (in_event->button() == Qt::RightButton)
	//{

	//}
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
	comp_type = COMPONENT_TYPE_MESH;
	//构建组件
	setObjectName("mesh_component");
	setGeometry(QRect(pipeline_w_mouse_position_x, pipeline_w_mouse_position_y, 120, 100));
	setAlignment(Qt::AlignCenter);
	setTitle(QCoreApplication::translate("stre_editorClass", "mesh", nullptr));

	//构建端口
	output_port = new connect_port(
		this,
		connect_port::MESH_OUTPUT,
			this);
	output_port->setObjectName("mesh_output_port");
	output_port->setGeometry(QRect(0, 30, 120, 20));
	output_port->setLayoutDirection(Qt::RightToLeft);
	output_port->setAutoExclusive(false);
	
	output_port->setText(QCoreApplication::translate("stre_editorClass", "output", nullptr));

	constants_output_port = new connect_port(
		this,
		connect_port::MESH_CONTANTS_OUTPUT,
			this);
	constants_output_port->setObjectName("constants_output_port");
	constants_output_port->setGeometry(QRect(0, 50, 120, 20));
	constants_output_port->setLayoutDirection(Qt::RightToLeft);
	constants_output_port->setAutoExclusive(false);
	constants_output_port->setText(QCoreApplication::translate("stre_editorClass", "constants output", nullptr));

	material_output_port = new connect_port(
		this,
		connect_port::MESH_MATERIAL_OUTPUT,
			this);
	material_output_port->setObjectName("material_output_port");
	material_output_port->setGeometry(QRect(0, 70, 120, 20));
	material_output_port->setLayoutDirection(Qt::RightToLeft);
	material_output_port->setAutoExclusive(false);
	material_output_port->setText(QCoreApplication::translate("stre_editorClass", "material output", nullptr));

}

/// <summary>
/// delete删除组件
/// </summary>
/// <param name="in_event"></param>
void mesh_component_invoker::keyPressEvent(QKeyEvent* in_event)
{
	if (in_event->key() == Qt::Key_Delete)
	{
		s_remove_mesh_command remove_mesh_cmd;
		remove_mesh_cmd.mesh_component_delete_ptr = this;
		remove_mesh_cmd.execute();
		remove_mesh_cmd.mesh_component_delete_ptr = nullptr;
	}
	return QWidget::keyPressEvent(in_event);
}

/*********************************************
* pass_component
*
*********************************************/


/// <summary>
/// 删除原有，并断开所有连接，更新 pass组件上的shader资源端口
/// </summary>
/// <param name="in_res_port_group"></param>
void pass_component_invoker::update_res_port(vector<connect_port*>& in_res_port_group)
{
	//删除原有的槽
	for (auto it : input_res_port_group)
	{
		//和资源断开
		s_disconnect_all_resource_command disconnect_resource_cmd;
		disconnect_resource_cmd.disconnect_port = it;
		disconnect_resource_cmd.execute();

		it->deleteLater();
	}
	input_res_port_group.clear();
	int height = 10;
	//加入新的槽
	for (auto it : in_res_port_group)
	{
		it->setText(it->objectName());
		it->setGeometry(QRect(10, height, 150, 20));
		it->show();
		input_res_port_group.push_back(it);
		height += 20;
	}
}

pass_component_invoker::pass_component_invoker(
	QWidget* in_parent,
	s_pass* in_pass_ptr) :
	component_invoker(in_parent),
	pass_instance(in_pass_ptr)
{
	//pass_tree_node = new pass_node(this);
	comp_type = COMPONENT_TYPE_PASS;
	//构建组件
	setObjectName("pass_component");
	setGeometry(QRect(pipeline_w_mouse_position_x, pipeline_w_mouse_position_y, 211, 201));
	setAlignment(Qt::AlignCenter);

	
	//构建端口
	
	//!!!编译了着色器再表现端口
	//pass_resource_port = new QRadioButton(pass_component);
	//pass_resource_port->setObjectName("pass_resource_port");
	//pass_resource_port->setGeometry(QRect(10, 20, 71, 20));

	mesh_port = new connect_port(
		this, 
		connect_port::PASS_MESH_INPUT,
			this);
	mesh_port->setObjectName("pass_mesh_port");
	mesh_port->setGeometry(QRect(10, 150, 95, 20));
	mesh_port->setAutoExclusive(false);

	shader_port = new connect_port(
		this,
		connect_port::PASS_SHADER_INPUT,
			this);
	shader_port->setObjectName("pass_shade_port");
	shader_port->setGeometry(QRect(10, 170, 95, 20));
	shader_port->setAutoExclusive(false);

	output_port = new connect_port(
		this,
		connect_port::PASS_OUTPUT,
			this);
	output_port->setObjectName("pass_output_port");
	output_port->setGeometry(QRect(144, 170, 61, 20));
	output_port->setLayoutDirection(Qt::RightToLeft);
	output_port->setAutoExclusive(false);

	setTitle(QCoreApplication::translate("stre_editorClass", "pass", nullptr));

	//pass_resource_port->setText(QCoreApplication::translate("stre_editorClass", "res_port", nullptr));
	
	mesh_port->setText(QCoreApplication::translate("stre_editorClass", "mesh_port", nullptr));
	shader_port->setText(QCoreApplication::translate("stre_editorClass", "shader_port", nullptr));
	output_port->setText(QCoreApplication::translate("stre_editorClass", "output", nullptr));

}

/// <summary>
/// delete 删除键
/// </summary>
/// <param name="in_event"></param>
void pass_component_invoker::keyPressEvent(QKeyEvent* in_event)
{
	if (in_event->key() == Qt::Key_Delete)
	{
		s_remove_pass_command remove_pass_cmd;
		remove_pass_cmd.pass_component_delete_ptr = this;
		remove_pass_cmd.execute();
		remove_pass_cmd.pass_component_delete_ptr = nullptr;
	}
	return QWidget::keyPressEvent(in_event);
}

/*********************************************
* texture_component
*
*********************************************/

texture_element_invoker::texture_element_invoker(
	cpu_texture* in_texture_ptr,
	int in_height, 
	QWidget* in_parent) :
	texture_instance(in_texture_ptr),
	component_invoker(in_parent)
{
	comp_type = COMPONENT_TYPE_TEXTURE;

	setObjectName("texture_element_widget");
	setGeometry(QRect(0, in_height, 121, 41));
	setTitle(QCoreApplication::translate("stre_editorClass", "element", nullptr));

	//构建名字标签
	texture_name = new QLabel(this);
	texture_name->setObjectName("texture_name");
	texture_name->setGeometry(QRect(0, 0, 121, 16));

	//构建端口
	// //单张贴图的输入依靠贴图组
	//input_port = new connect_port(this, port_information(port_information::TEXTURE_INPUT, texture_instance));
	//input_port->setObjectName("texture_input_port");
	//input_port->setGeometry(QRect(0, 20, 51, 20));
	//input_port->setLayoutDirection(Qt::LeftToRight);
	//input_port->setAutoExclusive(false);
	//input_port->setText(QCoreApplication::translate("stre_editorClass", "input", nullptr));

	//并不能作为pass的输入
	//output_port = new connect_port(this, port_information(port_information::TEXTURE_OUTPUT, this));
	//output_port->setObjectName("texture_output_port");
	//output_port->setGeometry(QRect(60, 20, 61, 20));
	//output_port->setLayoutDirection(Qt::RightToLeft);
	//output_port->setAutoExclusive(false);
	//output_port->setText(QCoreApplication::translate("stre_editorClass", "output", nullptr));

}

/// <summary>
/// 设置高度
/// </summary>
/// <param name="in_height"></param>
void texture_element_invoker::set_height(int in_height)
{
	setGeometry(QRect(0, in_height, 121, 41));
}

/// <summary>
/// delete删除组件
/// </summary>
/// <param name="in_event"></param>
void texture_element_invoker::keyPressEvent(QKeyEvent* in_event)
{
	if (in_event->key() == Qt::Key_Delete)
	{
		s_remove_texture_command remove_texture_cmd;
		remove_texture_cmd.texture_element_delete_ptr = this;
		remove_texture_cmd.execute();
		remove_texture_cmd.texture_element_delete_ptr = nullptr;
	}
	return QWidget::keyPressEvent(in_event);
}

texture_component_invoker::texture_component_invoker(
	QWidget* in_parent,
	cpu_texture* in_texture_ptr) :
	texture_instance(in_texture_ptr),
	component_invoker(in_parent)
{
	comp_type = COMPONENT_TYPE_TEXTURE_GROUP;
	//构建组件
	setObjectName("texture_group_component");
	setGeometry(QRect(
		pipeline_w_mouse_position_x, 
		pipeline_w_mouse_position_y, 
		121, 211));
	setAlignment(Qt::AlignCenter);
	setTitle(QCoreApplication::translate("stre_editorClass", "ds texture", nullptr));

	//构建端口
	input_port = new connect_port(this, connect_port::TEXTURE_GROUP_INPUT, this);
	input_port->setObjectName("texture_input_port");
	input_port->setGeometry(QRect(0, 20, 51, 20));
	input_port->setLayoutDirection(Qt::LeftToRight);
	input_port->setAutoExclusive(false);
	input_port->setText(QCoreApplication::translate("stre_editorClass", "input", nullptr));

	output_port = new connect_port(this, connect_port::TEXTURE_GROUP_OUTPUT, this);
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

/// <summary>
/// 添加贴图子组件
/// </summary>
/// <param name="in_texture_ptr"></param>
void texture_component_invoker::add_element(cpu_texture* in_texture_ptr)
{
	textures_group.push_back(
		new texture_element_invoker(
			in_texture_ptr, 
			element_stk_height,
			this));

	textures_group.back()->show();

	update_package();

	element_stk_height += 41;
}

/// <summary>
/// 删除元素，并重新排序，重新连接接口
/// </summary>
/// <param name="in_texture_ptr"></param>
void texture_component_invoker::remove_element(cpu_texture* in_texture_ptr)
{
	//删除目标组件
	for (int i =0 ; i < textures_group.size();i++)
	{
		if (textures_group[i]->texture_instance->uid.name == in_texture_ptr->uid.name)
		{
			//没有端口
			//断开端口
			//s_disconnect_all_resource_command disconnect_resource_cmd;
			//disconnect_resource_cmd.disconnect_port = textures_group[i]->output_port;
			//disconnect_resource_cmd.execute();
			////断不了就不许删
			//if (!disconnect_resource_cmd.disconnect_success)
			//{
			//	//!!!出问题了
			//	return;
			//}
			//disconnect_resource_cmd.disconnect_port = nullptr;

			textures_group[i]->deleteLater();
			textures_group.erase(textures_group.begin()+i);
			break;
		}
	}

	//重新排序
	element_stk_height = 41;

	for (int i = 0; i < textures_group.size(); i++)
	{
		textures_group[i]->set_height(element_stk_height);
		textures_group[i]->show();
		element_stk_height += 41;
	}

	update_package();
}

/// <summary>
/// 更新package
/// </summary>
void texture_component_invoker::update_package()
{

	//刷新自己
	if (textures_group.empty())
	{
		s_update_texture_gpu_command update_texture_gpu_cmd;
		update_texture_gpu_cmd.texture_update_ptr = texture_instance;
		update_texture_gpu_cmd.execute();
		update_texture_gpu_cmd.texture_update_ptr = nullptr;
	}
	else
	{//重新打包
		s_package_texture_command package_texture_cmd;
		package_texture_cmd.texture_component_package_texture_ptr = this;
		package_texture_cmd.execute();
	}

	//刷新自己的名字
	switch (texture_instance->gpu_sr_ptr->shader_resource_type)
	{
	case gpu_shader_resource::SHADER_RESOURCE_TYPE_RENDER_DEPTH_STENCIL_GROUP:
		setTitle(QCoreApplication::translate("stre_editorClass", "ds texture", nullptr));
		break;
	case gpu_shader_resource::SHADER_RESOURCE_TYPE_RENDER_TARGET_GROUP:
		setTitle(QCoreApplication::translate("stre_editorClass", "rt texture", nullptr));
		break;
	case gpu_shader_resource::SHADER_RESOURCE_TYPE_TEXTURE_GROUP:
		setTitle(QCoreApplication::translate("stre_editorClass", "tx texture", nullptr));
		break;
	}

	//重新连接接口
	s_reconnect_resource_command reconnect_cmd;
	reconnect_cmd.reconnect_port = input_port;
	reconnect_cmd.execute();

	reconnect_cmd.reconnect_port = output_port;
	reconnect_cmd.execute();
	reconnect_cmd.reconnect_port = nullptr;
}

/// <summary>
/// 添加贴图按键
/// </summary>
void texture_component_invoker::add_texture()
{
	s_create_texture_command create_texture_cmd;
	create_texture_cmd.texture_component_add_texture_ptr = this;
	create_texture_cmd.execute();
	create_texture_cmd.texture_component_add_texture_ptr = nullptr;
}

/// <summary>
/// delete删除控件
/// </summary>
/// <param name="in_event"></param>
void texture_component_invoker::keyPressEvent(QKeyEvent* in_event)
{
	if (in_event->key() == Qt::Key_Delete)
	{
		s_remove_texture_group_command remove_texture_group_cmd;
		remove_texture_group_cmd.texture_component_delete_ptr = this;
		remove_texture_group_cmd.execute();
		remove_texture_group_cmd.texture_component_delete_ptr = nullptr;
	
	}
	return QWidget::keyPressEvent(in_event);
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
	comp_type = COMPONENT_TYPE_SHADER;
	//构建组件
	setObjectName("shader_component");
	setGeometry(QRect(pipeline_w_mouse_position_x, pipeline_w_mouse_position_y, 120, 51));
	setAlignment(Qt::AlignCenter);

	//构建端口
	output_port = new connect_port(this, connect_port::SHADER_OUTPUT, this);
	output_port->setObjectName("shader_output_port");
	output_port->setGeometry(QRect(54, 30, 61, 20));
	output_port->setLayoutDirection(Qt::RightToLeft);
	output_port->setAutoExclusive(false);
	setTitle(QCoreApplication::translate("stre_editorClass", "shader", nullptr));
	output_port->setText(QCoreApplication::translate("stre_editorClass", "output", nullptr));

}

/// <summary>
/// delete删除控件
/// </summary>
/// <param name="in_event"></param>
void shader_component_invoker::keyPressEvent(QKeyEvent* in_event)
{
	if (in_event->key() == Qt::Key_Delete)
	{
		s_remove_shader_command remove_shader_cmd;
		remove_shader_cmd.shader_component_delete_ptr = this;
		remove_shader_cmd.execute();
		remove_shader_cmd.shader_component_delete_ptr = nullptr;
	}
	return QWidget::keyPressEvent(in_event);
}
/*********************************************
* camera_component
*
*********************************************/

camera_component_invoker::camera_component_invoker(
	QWidget* in_parent,
	cpu_camera* in_camera_ptr) :
	component_invoker(in_parent),
	camera_instance(in_camera_ptr)
{
	comp_type = COMPONENT_TYPE_CAMERA;
	//构建组件
	setObjectName("camera_component");
	setGeometry(QRect(pipeline_w_mouse_position_x, pipeline_w_mouse_position_y, 120, 51));
	setAlignment(Qt::AlignCenter);

	//构建端口
	output_port = new connect_port(this, connect_port::CAMERA_OUTPUT, this);
	output_port->setObjectName("camera_output_port");
	output_port->setGeometry(QRect(54, 30, 61, 20));
	output_port->setLayoutDirection(Qt::RightToLeft);
	output_port->setAutoExclusive(false);
	setTitle(QCoreApplication::translate("stre_editorClass", "camera", nullptr));
	output_port->setText(QCoreApplication::translate("stre_editorClass", "output", nullptr));

}

/// <summary>
/// delete删除控件
/// </summary>
/// <param name="in_event"></param>
void camera_component_invoker::keyPressEvent(QKeyEvent* in_event)
{
	if (current_component_ptr->comp_type == COMPONENT_TYPE_CAMERA)
	{
		auto camera_comp_ptr = static_cast<camera_component_invoker*>(current_component_ptr);
		if (in_event->key() == Qt::Key_W)
		{
			camera_comp_ptr->camera_cal_helper.Walk(10.0f * 0.1);
		}
		if (in_event->key() == Qt::Key_S)
		{
			camera_comp_ptr->camera_cal_helper.Walk(-10.0f * 0.1);
		}
		if (in_event->key() == Qt::Key_A)
		{
			camera_comp_ptr->camera_cal_helper.Strafe(-10.0f * 0.1);
		}
		if (in_event->key() == Qt::Key_D)
		{
			camera_comp_ptr->camera_cal_helper.Strafe(10.0f * 0.1);
		}

		s_update_camera_command update_camera_cmd;
		update_camera_cmd.camera_helper_ptr = &camera_comp_ptr->camera_cal_helper;
		update_camera_cmd.camera_ptr = camera_comp_ptr->camera_instance;
		update_camera_cmd.execute();
		update_camera_cmd.camera_helper_ptr = nullptr;
		update_camera_cmd.camera_ptr = nullptr;

	}

	if (in_event->key() == Qt::Key_Delete)
	{
		s_remove_camera_command remove_camera_cmd;
		remove_camera_cmd.camera_component_delete_ptr = this;
		remove_camera_cmd.execute();
		remove_camera_cmd.camera_component_delete_ptr = nullptr;

	}

	return QWidget::keyPressEvent(in_event);
}





/*********************************************
* light_component
*
*********************************************/

light_component_invoker::light_component_invoker(
	QWidget* in_parent,
	cpu_light* in_light_ptr) :
	component_invoker(in_parent),
	light_instance(in_light_ptr)
{
	comp_type = COMPONENT_TYPE_LIGHT;
	//构建组件
	setObjectName("light_component");
	setGeometry(QRect(pipeline_w_mouse_position_x, pipeline_w_mouse_position_y, 120, 51));
	setAlignment(Qt::AlignCenter);

	//构建端口
	output_port = new connect_port(this, connect_port::LIGHT_OUTPUT, this);
	output_port->setObjectName("light_output_port");
	output_port->setGeometry(QRect(54, 30, 61, 20));
	output_port->setLayoutDirection(Qt::RightToLeft);
	output_port->setAutoExclusive(false);
	setTitle(QCoreApplication::translate("stre_editorClass", "light", nullptr));
	output_port->setText(QCoreApplication::translate("stre_editorClass", "output", nullptr));

}

/// <summary>
/// delete删除控件
/// </summary>
/// <param name="in_event"></param>
void light_component_invoker::keyPressEvent(QKeyEvent* in_event)
{
	if (in_event->key() == Qt::Key_Delete)
	{
		s_remove_light_command remove_light_cmd;
		remove_light_cmd.light_component_delete_ptr = this;
		remove_light_cmd.execute();
		remove_light_cmd.light_component_delete_ptr = nullptr;

	}
	return QWidget::keyPressEvent(in_event);
}



/*********************************************
* connect_port
* 
*********************************************/
//这个好像只能全局  //执行命令时装入命令的局部执行
connect_port* connect_port::select_connect_port[2] = { nullptr };
int connect_port::select_port_index = 0;

connect_port::connect_port(
	QWidget* in_parent,
	PORT_TYPE in_port_type,
	void* in_ptr,
	int in_port_index) :
	QRadioButton(in_parent),
	port_type(in_port_type),
	ptr(in_ptr),
	port_index(in_port_index)
{
}

/// <summary>
/// 选中一个，选中两个，判断能否连接，执行连接命令
/// </summary>
/// <param name="in_event"></param>
void connect_port::mousePressEvent(QMouseEvent* in_event)
{

	if (in_event->button() == Qt::LeftButton)
	{
		//!!!不太对
		switch (select_port_index)
		{
		case 0:
		{
			select_connect_port[0] = this;
			setChecked(true);
			select_port_index++;
		}
			break;
		case 1:
		{
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
			s_connect_resource_command connect_resource_cmd;
			connect_resource_cmd.select_connect_port[0] = select_connect_port[0];
			connect_resource_cmd.select_connect_port[1] = select_connect_port[1];
			connect_resource_cmd.execute();//装入局部执行
		}
		default:
		{
			if(select_connect_port[0])
			select_connect_port[0]->setChecked(false);
			if (select_connect_port[1])
			select_connect_port[1]->setChecked(false);
			select_connect_port[0] = nullptr;
			select_connect_port[1] = nullptr;
			select_port_index = 0;
		}
			break;
		}
	}
	


	return;
}

/// <summary>
/// useless
/// </summary>
/// <param name="in_event"></param>
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
/// <summary>
/// 改变曲线起始点
/// </summary>
/// <param name="in_start"></param>
void curve_tool::change_start(QPoint in_start)
{
	start = in_start;
	update();
}

/// <summary>
/// 改变曲线终点
/// </summary>
/// <param name="in_end"></param>
void curve_tool::change_end(QPoint in_end)
{
	end = in_end;
	update();
}

/// <summary>
/// 刷新曲线
/// </summary>
void curve_tool::update()
{

	QPoint mid_pos = (start + end) / 2;
	curve.clear();//会自己留下痕迹，所以得先清理
	curve.moveTo(start);
	curve.cubicTo(QPointF(mid_pos.x(), start.y()), mid_pos, end);
}

/*********************************************
* view_port_invoker
*
*********************************************/

view_port_invoker::view_port_invoker(QWidget* in_parent):QWidget(in_parent)
{
	view_port_widget_ptr = this;
	draw_cmd = new s_draw_command();

	setObjectName("rendering_view_widget");
	setGeometry(QRect(0,0, 1920, 1080));
	//setGeometry(QRect(1100, 10, 311, 241));
}


void view_port_invoker::mousePressEvent(QMouseEvent* in_event)
{
	mouse_point = in_event->pos();
	is_mouse_down = true;
}

void view_port_invoker::mouseMoveEvent(QMouseEvent* in_event)
{
	if (current_component_ptr && current_component_ptr->comp_type == COMPONENT_TYPE_CAMERA)
	{
		auto camera_comp_ptr = static_cast<camera_component_invoker*>(current_component_ptr);
		camera_comp_ptr->camera_cal_helper.Pitch((in_event->y() - mouse_point.y())*0.1);
		camera_comp_ptr->camera_cal_helper.RotateY((in_event->x() - mouse_point.x()) * 0.1);
		mouse_point = in_event->pos();

		s_update_camera_command update_camera_cmd;
		update_camera_cmd.camera_helper_ptr = &camera_comp_ptr->camera_cal_helper;
		update_camera_cmd.camera_ptr = camera_comp_ptr->camera_instance;
		update_camera_cmd.execute();
		update_camera_cmd.camera_helper_ptr = nullptr;
		update_camera_cmd.camera_ptr = nullptr;

	}
}

void view_port_invoker::mouseReleaseEvent(QMouseEvent* in_event)
{
	mouse_point = in_event->pos();
	is_mouse_down = false;
}

/// <summary>
/// 输出端口刷新
/// </summary>
/// <param name="in_event"></param>
void view_port_invoker::paintEvent(QPaintEvent* in_event)
{
	draw_cmd->execute();
}


debug_text_invoker::debug_text_invoker(QWidget* in_parent) : QTextEdit(in_parent)
{
	debug_text_ptr = this;
	setObjectName("debug_text_edit");
	setGeometry(QRect(1,849, 1279,50));
}

