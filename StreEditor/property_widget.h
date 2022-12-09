#pragma once
#include <QtWidgets/QWidget>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QLabel>

class property_widget;
class mesh_property_widget;
class shader_property_widget;
class texture_property_widget;
class texture_group_property_widget;
class pass_property_widget;


class property_tab_widget : public QTabWidget
{
public:
	property_tab_widget(QWidget* inparent);
	property_widget* empty_property_window_instance = nullptr;
	mesh_property_widget* mesh_property_window_instance = nullptr;
	shader_property_widget* shader_property_window_instance = nullptr;
	texture_property_widget* texture_property_window_instance = nullptr;
	texture_group_property_widget* texture_group_property_window_instance = nullptr;
	pass_property_widget* pass_property_window_instance = nullptr;
};

class property_widget : public QWidget
{
public:
	property_widget(QTabWidget* in_parent_tab_widget);
};

class mesh_property_widget : public QWidget
{
public:
	mesh_property_widget(QTabWidget* in_parent_tab_widget);

	QComboBox* type_select_comcobox = nullptr;
	QTextEdit* path_text = nullptr;
	QPushButton* path_select_pushbutton = nullptr;

	QLabel* transform_label = nullptr;
	QDoubleSpinBox* transform_x_spinbox = nullptr;
	QDoubleSpinBox* transform_y_spinbox = nullptr;
	QDoubleSpinBox* transform_z_spinbox = nullptr;
	
	QLabel* rotation_label = nullptr;
	QDoubleSpinBox* rotation_x_spinbox = nullptr;
	QDoubleSpinBox* rotation_y_spinbox = nullptr;
	QDoubleSpinBox* rotation_z_spinbox = nullptr;
	
	QLabel* scale_label = nullptr;
	QDoubleSpinBox* scale_x_spinbox = nullptr;
	QDoubleSpinBox* scale_y_spinbox = nullptr;
	QDoubleSpinBox* scale_z_spinbox = nullptr;
	void refresh_spin_box();
	void update_mesh();
};

class camera_property_widget : public QWidget
{
public:

};

class light_property_widget : public QWidget
{
public:

};

class shader_property_widget : public QWidget
{
public:
	shader_property_widget(QTabWidget* in_parent_tab_widget);

	QTextEdit* path_text = nullptr;
	QPushButton* path_select_pushbutton = nullptr;

	QCheckBox* vs_check_box = nullptr;
	QCheckBox* ds_check_box = nullptr;
	QCheckBox* hs_check_box = nullptr;
	QCheckBox* gs_check_box = nullptr;
	QCheckBox* ps_check_box = nullptr;
};



class texture_property_widget : public QWidget
{
public:
	texture_property_widget(QTabWidget* in_parent_tab_widget);
	
	QComboBox* type_select_comcobox = nullptr;
	QTextEdit* path_text = nullptr;
	QPushButton* path_select_pushbutton = nullptr;

	void change_combobox_index(int index);
};

class texture_group_property_widget : public QWidget
{
public:
	texture_group_property_widget(QTabWidget* in_parent_tab_widget);

	QComboBox* type_select_comcobox = nullptr;

	void change_combobox_index(int index);
};

class pass_property_widget : public QWidget
{
public:
	pass_property_widget(QTabWidget* in_parent_tab_widget);

	QCheckBox* is_output_check_box = nullptr;
	QCheckBox * is_depth_check_box = nullptr;
	QCheckBox* is_translate_check_box = nullptr;
	
	QComboBox* pass_level_comcobox = nullptr;

	//QCheckBox* is_start_check_box = nullptr;
};