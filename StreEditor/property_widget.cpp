#include "property_widget.h"
#include <QFileDialog>
#include "s_invoker.h"
#include "concrete_command.h"

QString open_file_path(QWidget* in_widget, const QString file_param);

//Property panel code design Property panel code design
//Property panel class Add yourself in property_tab_widget
//Write the control in the constructor
//add extern global variable
//Add your own reflection information in s_switch_property_widget_command
//属性面板代码设计 Property panel code design
//属性面板类 在property_tab_widget 添加自己
//在构造函数 写控制控件
//加extern全局变量
//在s_switch_property_widget_command 添加自己的反射信息

//没写那么好，只能赋值一次
property_tab_widget* current_property_tab_widget = nullptr;
property_widget* current_empty_property_widget = nullptr;
mesh_property_widget* current_mesh_property_widget = nullptr;
shader_property_widget* current_shader_property_widget = nullptr;
texture_property_widget* current_texture_property_widget = nullptr;
texture_group_property_widget* current_texture_group_property_widget = nullptr;
pass_property_widget* current_pass_property_widget = nullptr;
camera_property_widget* current_camera_property_widget = nullptr;
light_property_widget* current_light_property_widget = nullptr;

bool had_output_pass = false;

extern component_invoker* current_component_ptr;
extern pipeline_window_invoker* pipeline_window_widget_ptr;
extern const int pass_max_level;

property_tab_widget::property_tab_widget(QWidget* in_parent) :QTabWidget(in_parent)
{
    current_property_tab_widget = this;
    //没写那么好，只能构建一次
    empty_property_window_instance = new property_widget(this);
    mesh_property_window_instance = new mesh_property_widget(this);
    shader_property_window_instance = new shader_property_widget(this);
    texture_property_window_instance = new texture_property_widget(this);
    texture_group_property_window_instance = new texture_group_property_widget(this);
    pass_property_window_instance = new pass_property_widget(this);
    camera_property_window_instance = new camera_property_widget(this);
    light_property_window_instance = new light_property_widget(this);

    addTab(empty_property_window_instance,"");
    addTab(mesh_property_window_instance, "mesh property");
    addTab(shader_property_window_instance, "shader property");
    addTab(texture_property_window_instance, "texture property");
    addTab(texture_group_property_window_instance, "texture group property");
    addTab(pass_property_window_instance, "pass property");
    addTab(camera_property_window_instance, "camera property");
    addTab(light_property_window_instance, "light property");
}

property_widget::property_widget(QTabWidget* in_parent_tab_widget) : QWidget(in_parent_tab_widget)
{
    current_empty_property_widget = this;
}

camera_property_widget::camera_property_widget(QTabWidget* in_parent_tab_widget)
{

    current_camera_property_widget = this;

    position_label = new QLabel(this);
    position_label->setObjectName("position_label");
    position_label->setGeometry(QRect(30, 10, 53, 16));
    position_label->setText("position");
    position_x_spinbox = new QDoubleSpinBox(this);
    position_x_spinbox->setSingleStep(0.1);
    position_x_spinbox->setRange(-9999, 9999);
    position_x_spinbox->setObjectName("position_x_spinbox");
    position_x_spinbox->setGeometry(QRect(30, 30, 62, 22));
    position_y_spinbox = new QDoubleSpinBox(this);
    position_y_spinbox->setSingleStep(0.1);
    position_y_spinbox->setRange(-9999, 9999);
    position_y_spinbox->setObjectName("position_y_spinbox");
    position_y_spinbox->setGeometry(QRect(100, 30, 62, 22));
    position_z_spinbox = new QDoubleSpinBox(this);
    position_z_spinbox->setSingleStep(0.1);
    position_z_spinbox->setRange(-9999, 9999);
    position_z_spinbox->setObjectName("position_z_spinbox");
    position_z_spinbox->setGeometry(QRect(170, 30, 62, 22));

    look_at_label = new QLabel(this);
    look_at_label->setObjectName("look_at_label");
    look_at_label->setGeometry(QRect(30, 50, 53, 16));
    look_at_label->setText("look at");
    look_at_x_spinbox = new QDoubleSpinBox(this);
    look_at_x_spinbox->setSingleStep(0.1);
    look_at_x_spinbox->setRange(-9999, 9999);
    look_at_x_spinbox->setObjectName("look_at_x_spinbox");
    look_at_x_spinbox->setGeometry(QRect(30, 70, 62, 22));
    look_at_y_spinbox = new QDoubleSpinBox(this);
    look_at_y_spinbox->setSingleStep(0.1);
    look_at_y_spinbox->setRange(-9999, 9999);
    look_at_y_spinbox->setObjectName("look_at_y_spinbox");
    look_at_y_spinbox->setGeometry(QRect(100, 70, 62, 22));
    look_at_z_spinbox = new QDoubleSpinBox(this);
    look_at_z_spinbox->setSingleStep(0.1);
    look_at_z_spinbox->setRange(-9999, 9999);
    look_at_z_spinbox->setObjectName("look_at_z_spinbox");
    look_at_z_spinbox->setGeometry(QRect(170, 70, 62, 22));

    connect(position_x_spinbox, &QDoubleSpinBox::valueChanged, this,
        [&]()
        {
            if (current_component_ptr && current_component_ptr->comp_type == COMPONENT_TYPE_CAMERA)
            {
                auto camera_comp_ptr = static_cast<camera_component_invoker*>(current_component_ptr);
                auto position = camera_comp_ptr->camera_cal_helper.GetPosition3f();
                position.x = position_x_spinbox->value();
                camera_comp_ptr->camera_cal_helper.SetPosition(position);
                update_camera();
            }
            
        });
    connect(position_y_spinbox, &QDoubleSpinBox::valueChanged, this,
        [&]()
        {
            if (current_component_ptr && current_component_ptr->comp_type == COMPONENT_TYPE_CAMERA)
            {
                auto camera_comp_ptr = static_cast<camera_component_invoker*>(current_component_ptr);
                auto position = camera_comp_ptr->camera_cal_helper.GetPosition3f();
                position.y = position_y_spinbox->value();
                camera_comp_ptr->camera_cal_helper.SetPosition(position);

                update_camera();
            }
            
        });
    connect(position_z_spinbox, &QDoubleSpinBox::valueChanged, this,
        [&]()
        {
            if (current_component_ptr && current_component_ptr->comp_type == COMPONENT_TYPE_CAMERA)
            {
                auto camera_comp_ptr = static_cast<camera_component_invoker*>(current_component_ptr);
                auto position = camera_comp_ptr->camera_cal_helper.GetPosition3f();
                position.z = position_z_spinbox->value();
                camera_comp_ptr->camera_cal_helper.SetPosition(position);

                update_camera();
            }
            
        });

    connect(look_at_x_spinbox, &QDoubleSpinBox::valueChanged, this,
        [&]()
        {
            if (current_component_ptr && current_component_ptr->comp_type == COMPONENT_TYPE_CAMERA)
            {
                auto camera_comp_ptr = static_cast<camera_component_invoker*>(current_component_ptr);
                auto look_at = camera_comp_ptr->camera_cal_helper.GetLook3f();
                auto position = camera_comp_ptr->camera_cal_helper.GetPosition3f();
                look_at.x = look_at_x_spinbox->value();
                camera_comp_ptr->camera_cal_helper.LookAt(position, look_at, {0,1,0});
                update_camera();
            }

        });
    connect(look_at_y_spinbox, &QDoubleSpinBox::valueChanged, this,
        [&]()
        {
            if (current_component_ptr && current_component_ptr->comp_type == COMPONENT_TYPE_CAMERA)
            {
                auto camera_comp_ptr = static_cast<camera_component_invoker*>(current_component_ptr);
                auto look_at = camera_comp_ptr->camera_cal_helper.GetLook3f();
                auto position = camera_comp_ptr->camera_cal_helper.GetPosition3f();
                look_at.y = look_at_y_spinbox->value();
                camera_comp_ptr->camera_cal_helper.LookAt(position, look_at, { 0,1,0 });
                update_camera();
            }

        });
    connect(look_at_z_spinbox, &QDoubleSpinBox::valueChanged, this,
        [&]()
        {
            if (current_component_ptr && current_component_ptr->comp_type == COMPONENT_TYPE_CAMERA)
            {
                auto camera_comp_ptr = static_cast<camera_component_invoker*>(current_component_ptr);
                auto look_at = camera_comp_ptr->camera_cal_helper.GetLook3f();
                auto position = camera_comp_ptr->camera_cal_helper.GetPosition3f();
                look_at.z = look_at_z_spinbox->value();
                camera_comp_ptr->camera_cal_helper.LookAt(position, look_at, { 0,1,0 });
                update_camera();
            }

        });

}

void camera_property_widget::update_camera()
{
    if (current_component_ptr && current_component_ptr->comp_type == COMPONENT_TYPE_CAMERA)
    {
        auto camera_comp_ptr = static_cast<camera_component_invoker*>(current_component_ptr);
        s_update_camera_command update_camera_data_cmd;
        update_camera_data_cmd.camera_ptr = camera_comp_ptr->camera_instance;
        update_camera_data_cmd.camera_helper_ptr = &camera_comp_ptr->camera_cal_helper;
        update_camera_data_cmd.execute();
        update_camera_data_cmd.camera_ptr = nullptr;
        update_camera_data_cmd.camera_helper_ptr = nullptr;
    }
}

void camera_property_widget::refresh_spin_box()
{
    if (current_component_ptr && current_component_ptr->comp_type == COMPONENT_TYPE_CAMERA)
    {
        auto camera_comp_ptr = static_cast<camera_component_invoker*>(current_component_ptr);
        auto position = camera_comp_ptr->camera_cal_helper.GetPosition3f();
        auto look_at = camera_comp_ptr->camera_cal_helper.GetLook3f();
        position_x_spinbox->setValue(position.x);
        position_y_spinbox->setValue(position.y);
        position_z_spinbox->setValue(position.z);

        look_at_x_spinbox->setValue(look_at.x);
        look_at_y_spinbox->setValue(look_at.y);
        look_at_z_spinbox->setValue(look_at.z);
    }
}

light_property_widget::light_property_widget(QTabWidget* in_parent_tab_widget)
{

    current_light_property_widget = this;

    strength_label = new QLabel(this);
    strength_label->setObjectName("strength_label");
    strength_label->setGeometry(QRect(30, 10, 53, 16));
    strength_label->setText("strength");
    strength_r_spinbox = new QDoubleSpinBox(this);
    strength_r_spinbox->setSingleStep(0.1);
    strength_r_spinbox->setRange(-9999, 9999);
    strength_r_spinbox->setObjectName("strength_r_spinbox");
    strength_r_spinbox->setGeometry(QRect(30, 30, 62, 22));
    strength_g_spinbox = new QDoubleSpinBox(this);
    strength_g_spinbox->setSingleStep(0.1);
    strength_g_spinbox->setRange(-9999, 9999);
    strength_g_spinbox->setObjectName("strength_g_spinbox");
    strength_g_spinbox->setGeometry(QRect(100, 30, 62, 22));
    strength_b_spinbox = new QDoubleSpinBox(this);
    strength_b_spinbox->setSingleStep(0.1);
    strength_b_spinbox->setRange(-9999, 9999);
    strength_b_spinbox->setObjectName("strength_b_spinbox");
    strength_b_spinbox->setGeometry(QRect(170, 30, 62, 22));

    direction_label = new QLabel(this);
    direction_label->setObjectName("direction_label");
    direction_label->setGeometry(QRect(30, 50, 53, 16));
    direction_label->setText("direction");
    direction_x_spinbox = new QDoubleSpinBox(this);
    direction_x_spinbox->setSingleStep(0.1);
    direction_x_spinbox->setRange(-9999, 9999);
    direction_x_spinbox->setObjectName("direction_x_spinbox");
    direction_x_spinbox->setGeometry(QRect(30, 70, 62, 22));
    direction_y_spinbox = new QDoubleSpinBox(this);
    direction_y_spinbox->setSingleStep(0.1);
    direction_y_spinbox->setRange(-9999, 9999);
    direction_y_spinbox->setObjectName("direction_y_spinbox");
    direction_y_spinbox->setGeometry(QRect(100, 70, 62, 22));
    direction_z_spinbox = new QDoubleSpinBox(this);
    direction_z_spinbox->setSingleStep(0.1);
    direction_z_spinbox->setRange(-9999, 9999);
    direction_z_spinbox->setObjectName("direction_z_spinbox");
    direction_z_spinbox->setGeometry(QRect(170, 70, 62, 22));

    position_label = new QLabel(this);
    position_label->setObjectName("position_label");
    position_label->setGeometry(QRect(30, 90, 53, 16));
    position_label->setText("position");
    position_x_spinbox = new QDoubleSpinBox(this);
    position_x_spinbox->setSingleStep(0.1);
    position_x_spinbox->setRange(-9999, 9999);
    position_x_spinbox->setObjectName("position_x_spinbox");
    position_x_spinbox->setGeometry(QRect(30, 110, 62, 22));
    position_y_spinbox = new QDoubleSpinBox(this);
    position_y_spinbox->setSingleStep(0.1);
    position_y_spinbox->setRange(-9999, 9999);
    position_y_spinbox->setObjectName("position_y_spinbox");
    position_y_spinbox->setGeometry(QRect(100, 110, 62, 22));
    position_z_spinbox = new QDoubleSpinBox(this);
    position_z_spinbox->setSingleStep(0.1);
    position_z_spinbox->setRange(-9999, 9999);
    position_z_spinbox->setObjectName("position_z_spinbox");
    position_z_spinbox->setGeometry(QRect(170, 110, 62, 22));

    fall_off_start_label = new QLabel(this);
    fall_off_start_label->setObjectName("fall_off_start_label");
    fall_off_start_label->setGeometry(QRect(30, 140, 100, 16));
    fall_off_start_label->setText("fall off start");
    fall_off_start_spinbox = new QDoubleSpinBox(this);
    fall_off_start_spinbox->setSingleStep(0.1);
    fall_off_start_spinbox->setRange(-9999, 9999);
    fall_off_start_spinbox->setObjectName("fall_off_start_spinbox");
    fall_off_start_spinbox->setGeometry(QRect(170, 140, 62, 22));

    fall_off_end_label = new QLabel(this);
    fall_off_end_label->setObjectName("fall_off_end_label");
    fall_off_end_label->setGeometry(QRect(30, 170, 100, 16));
    fall_off_end_label->setText("fall off end");
    fall_off_end_spinbox = new QDoubleSpinBox(this);
    fall_off_end_spinbox->setSingleStep(0.1);
    fall_off_end_spinbox->setRange(-9999, 9999);
    fall_off_end_spinbox->setObjectName("fall_off_end_spinbox");
    fall_off_end_spinbox->setGeometry(QRect(170, 170, 62, 22));

    spot_power_label = new QLabel(this);
    spot_power_label->setObjectName("spot_power_label");
    spot_power_label->setGeometry(QRect(30, 200, 100, 16));
    spot_power_label->setText("spot power");
    spot_power_spinbox = new QDoubleSpinBox(this);
    spot_power_spinbox->setSingleStep(0.1);
    spot_power_spinbox->setRange(-9999, 9999);
    spot_power_spinbox->setObjectName("spot_power_spinbox");
    spot_power_spinbox->setGeometry(QRect(170, 200, 62, 22));


    connect(strength_r_spinbox, &QDoubleSpinBox::valueChanged, this,
        [&]()
        {
            if (current_component_ptr && current_component_ptr->comp_type == COMPONENT_TYPE_LIGHT)
            {
                auto light_comp_ptr = static_cast<light_component_invoker*>(current_component_ptr);
                light_comp_ptr->light_instance->get_data()->strength.x = strength_r_spinbox->value();
            }
            update_light();
        });
    connect(strength_g_spinbox, &QDoubleSpinBox::valueChanged, this,
        [&]()
        {
            if (current_component_ptr && current_component_ptr->comp_type == COMPONENT_TYPE_LIGHT)
            {
                auto light_comp_ptr = static_cast<light_component_invoker*>(current_component_ptr);
                light_comp_ptr->light_instance->get_data()->strength.y = strength_g_spinbox->value();
            }
            update_light();
        });
    connect(strength_b_spinbox, &QDoubleSpinBox::valueChanged, this,
        [&]()
        {
            if (current_component_ptr && current_component_ptr->comp_type == COMPONENT_TYPE_LIGHT)
            {
                auto light_comp_ptr = static_cast<light_component_invoker*>(current_component_ptr);
                light_comp_ptr->light_instance->get_data()->strength.z = strength_b_spinbox->value();
            }
            update_light();
        });
    connect(direction_x_spinbox, &QDoubleSpinBox::valueChanged, this,
        [&]()
        {
            if (current_component_ptr && current_component_ptr->comp_type == COMPONENT_TYPE_LIGHT)
            {
                auto light_comp_ptr = static_cast<light_component_invoker*>(current_component_ptr);
                light_comp_ptr->light_instance->get_data()->Direction.x = direction_x_spinbox->value();
            }
            update_light();
        });
    connect(direction_y_spinbox, &QDoubleSpinBox::valueChanged, this,
        [&]()
        {
            if (current_component_ptr && current_component_ptr->comp_type == COMPONENT_TYPE_LIGHT)
            {
                auto light_comp_ptr = static_cast<light_component_invoker*>(current_component_ptr);
                light_comp_ptr->light_instance->get_data()->Direction.y = direction_y_spinbox->value();
            }
            update_light();
        });
    connect(direction_z_spinbox, &QDoubleSpinBox::valueChanged, this,
        [&]()
        {
            if (current_component_ptr && current_component_ptr->comp_type == COMPONENT_TYPE_LIGHT)
            {
                auto light_comp_ptr = static_cast<light_component_invoker*>(current_component_ptr);
                light_comp_ptr->light_instance->get_data()->Direction.z = direction_z_spinbox->value();
            }
            update_light();
        });
    connect(position_x_spinbox, &QDoubleSpinBox::valueChanged, this,
        [&]()
        {
            if (current_component_ptr && current_component_ptr->comp_type == COMPONENT_TYPE_LIGHT)
            {
                auto light_comp_ptr = static_cast<light_component_invoker*>(current_component_ptr);
                light_comp_ptr->light_instance->get_data()->position.x = position_x_spinbox->value();
            }
            update_light();
        });
    connect(position_y_spinbox, &QDoubleSpinBox::valueChanged, this,
        [&]()
        {
            if (current_component_ptr && current_component_ptr->comp_type == COMPONENT_TYPE_LIGHT)
            {
                auto light_comp_ptr = static_cast<light_component_invoker*>(current_component_ptr);
                light_comp_ptr->light_instance->get_data()->position.y = position_y_spinbox->value();
            }
            update_light();
        });
    connect(position_z_spinbox, &QDoubleSpinBox::valueChanged, this,
        [&]()
        {
            if (current_component_ptr && current_component_ptr->comp_type == COMPONENT_TYPE_LIGHT)
            {
                auto light_comp_ptr = static_cast<light_component_invoker*>(current_component_ptr);
                light_comp_ptr->light_instance->get_data()->position.z = position_z_spinbox->value();
            }
            update_light();
        });
    connect(fall_off_start_spinbox, &QDoubleSpinBox::valueChanged, this,
        [&]()
        {
            if (current_component_ptr && current_component_ptr->comp_type == COMPONENT_TYPE_LIGHT)
            {
                auto light_comp_ptr = static_cast<light_component_invoker*>(current_component_ptr);
                light_comp_ptr->light_instance->get_data()->fall_off_start = fall_off_start_spinbox->value();
            }
            update_light();
        });
    connect(fall_off_end_spinbox, &QDoubleSpinBox::valueChanged, this,
        [&]()
        {
            if (current_component_ptr && current_component_ptr->comp_type == COMPONENT_TYPE_LIGHT)
            {
                auto light_comp_ptr = static_cast<light_component_invoker*>(current_component_ptr);
                light_comp_ptr->light_instance->get_data()->fall_off_end = fall_off_end_spinbox->value();
            }
            update_light();
        });
    connect(spot_power_spinbox, &QDoubleSpinBox::valueChanged, this,
        [&]()
        {
            if (current_component_ptr && current_component_ptr->comp_type == COMPONENT_TYPE_LIGHT)
            {
                auto light_comp_ptr = static_cast<light_component_invoker*>(current_component_ptr);
                light_comp_ptr->light_instance->get_data()->spot_power = spot_power_spinbox->value();
            }
            update_light();
        });
}

void light_property_widget::update_light()
{
    if (current_component_ptr && current_component_ptr->comp_type == COMPONENT_TYPE_LIGHT)
    {
        auto light_comp_ptr = static_cast<light_component_invoker*>(current_component_ptr);
        s_update_light_command update_light_data_cmd;
        update_light_data_cmd.light_ptr = light_comp_ptr;
        update_light_data_cmd.execute();
        update_light_data_cmd.light_ptr = nullptr;
    }
}

void light_property_widget::refresh_spin_box()
{
    if (current_component_ptr && current_component_ptr->comp_type == COMPONENT_TYPE_LIGHT)
    {
        auto light_comp_ptr = static_cast<light_component_invoker*>(current_component_ptr);
        auto light_data = light_comp_ptr->light_instance->get_data();

        strength_r_spinbox->setValue(light_data->strength.x);
        strength_g_spinbox->setValue(light_data->strength.y);
        strength_b_spinbox->setValue(light_data->strength.z);

        direction_x_spinbox->setValue(light_data->Direction.x);
        direction_y_spinbox->setValue(light_data->Direction.y);
        direction_z_spinbox->setValue(light_data->Direction.z);

        position_x_spinbox->setValue(light_data->position.x);
        position_y_spinbox->setValue(light_data->position.y);
        position_z_spinbox->setValue(light_data->position.z);

        fall_off_start_spinbox->setValue(light_data->fall_off_start.x);
        fall_off_end_spinbox->setValue(light_data->fall_off_end.x);
        spot_power_spinbox->setValue(light_data->spot_power.x);
    }
}

mesh_property_widget::mesh_property_widget(QTabWidget* in_parent_tab_widget) : QWidget(in_parent_tab_widget)
{
    current_mesh_property_widget = this;
	setObjectName("mesh_property_tab");

    //类型选择
	type_select_comcobox = new QComboBox(this);
	type_select_comcobox->setObjectName("type_select_comcobox");
	type_select_comcobox->setGeometry(QRect(20, 30, 271, 22));
    
    //文件路径选择
    path_text = new QTextEdit(this);
    path_text->setObjectName("path_text");
    path_text->setEnabled(false);
    path_text->setGeometry(QRect(20, 60, 221, 31));
    path_select_pushbutton = new QPushButton(this);
    path_select_pushbutton->setObjectName("path_select_pushbutton");
    path_select_pushbutton->setGeometry(QRect(250, 60, 41, 31));
    path_select_pushbutton->setText("/...");

    QAction* select_view_mesh_act = new QAction("view mesh");
    QAction* select_fbx_mesh_act = new QAction("fbx mesh");

    type_select_comcobox->addItem(select_view_mesh_act->text(), QVariant::fromValue(select_view_mesh_act)); //add actions
    type_select_comcobox->addItem(select_fbx_mesh_act->text(), QVariant::fromValue(select_fbx_mesh_act));

    //object_constants
    transform_label = new QLabel(this);
    transform_label->setObjectName("transform_label");
    transform_label->setGeometry(QRect(30, 90, 53, 16));
    transform_x_spinbox = new QDoubleSpinBox(this);
    transform_x_spinbox->setSingleStep(0.1);
    transform_x_spinbox->setRange(-9999, 9999);
    transform_x_spinbox->setObjectName("transform_x_spinbox");
    transform_x_spinbox->setGeometry(QRect(30, 110, 62, 22));
    transform_y_spinbox = new QDoubleSpinBox(this);
    transform_y_spinbox->setSingleStep(0.1);
    transform_y_spinbox->setRange(-9999, 9999);
    transform_y_spinbox->setObjectName("transform_y_spinbox");
    transform_y_spinbox->setGeometry(QRect(100, 110, 62, 22));
    transform_z_spinbox = new QDoubleSpinBox(this);
    transform_z_spinbox->setSingleStep(0.1);
    transform_z_spinbox->setRange(-9999, 9999);
    transform_z_spinbox->setObjectName("transform_z_spinbox");
    transform_z_spinbox->setGeometry(QRect(170, 110, 62, 22));
    transform_label->setText("transform");

    rotation_label = new QLabel(this);
    rotation_label->setObjectName("rotation_label");
    rotation_label->setGeometry(QRect(30, 130, 53, 16));
    rotation_x_spinbox = new QDoubleSpinBox(this);
    rotation_x_spinbox->setSingleStep(0.1);
    rotation_x_spinbox->setRange(-9999, 9999);
    rotation_x_spinbox->setObjectName("rotation_x_spinbox");
    rotation_x_spinbox->setGeometry(QRect(30, 150, 62, 22));
    rotation_y_spinbox = new QDoubleSpinBox(this);
    rotation_y_spinbox->setSingleStep(0.1);
    rotation_y_spinbox->setRange(-9999, 9999);
    rotation_y_spinbox->setObjectName("rotation_y_spinbox");
    rotation_y_spinbox->setGeometry(QRect(100, 150, 62, 22));
    rotation_z_spinbox = new QDoubleSpinBox(this);
    rotation_z_spinbox->setSingleStep(0.1);
    rotation_z_spinbox->setRange(-9999, 9999);
    rotation_z_spinbox->setObjectName("rotation_z_spinbox");
    rotation_z_spinbox->setGeometry(QRect(170, 150, 62, 22));
    rotation_label->setText("rotation");

    scale_label = new QLabel(this);
    scale_label->setObjectName("scale_label");
    scale_label->setGeometry(QRect(30, 170, 53, 16));
    scale_x_spinbox = new QDoubleSpinBox(this);
    scale_x_spinbox->setSingleStep(0.1);
    scale_x_spinbox->setRange(-9999, 9999);
    scale_x_spinbox->setObjectName("scale_x_spinbox");
    scale_x_spinbox->setGeometry(QRect(30, 190, 62, 22));
    scale_y_spinbox = new QDoubleSpinBox(this);
    scale_y_spinbox->setSingleStep(0.1);
    scale_y_spinbox->setRange(-9999, 9999);
    scale_y_spinbox->setObjectName("scale_y_spinbox");
    scale_y_spinbox->setGeometry(QRect(100, 190, 62, 22));
    scale_z_spinbox = new QDoubleSpinBox(this);
    scale_z_spinbox->setSingleStep(0.1);
    scale_z_spinbox->setRange(-9999, 9999);
    scale_z_spinbox->setObjectName("scale_z_spinbox");
    scale_z_spinbox->setGeometry(QRect(170, 190, 62, 22));
    scale_label->setText("scale");

    connect(type_select_comcobox, &QComboBox::currentIndexChanged, this, [&](int index)
        {
            QAction* selectedAction = type_select_comcobox->itemData(index, Qt::UserRole).value<QAction*>();
            if (selectedAction)
            {
                selectedAction->trigger(); //do stuff with your action
            }
        }
    );
    connect(select_view_mesh_act, &QAction::triggered, this, 
        [&]() 
        {
            path_select_pushbutton->setEnabled(false);
            
            if (current_component_ptr->comp_type == COMPONENT_TYPE_MESH)
            {
               auto mesh_comp_ptr =  static_cast<mesh_component_invoker*>(current_component_ptr);
               mesh_comp_ptr->mesh_instance->is_view_mesh = true;
               s_change_mesh_type_command().execute();
            }

        }
    );
    connect(select_fbx_mesh_act, &QAction::triggered, this, 
        [&]() 
        {
            path_select_pushbutton->setEnabled(true);
        }
    );
    connect(path_select_pushbutton, &QPushButton::clicked,this,
        [&]() 
        {
            auto path_str = QFileDialog::getExistingDirectory() + "/";
            path_text->setText(path_str);
            if (current_component_ptr && current_component_ptr->comp_type == COMPONENT_TYPE_MESH)
            {
                auto mesh_comp_ptr = static_cast<mesh_component_invoker*>(current_component_ptr);
                mesh_comp_ptr->mesh_instance->path = path_str.toStdWString();
                mesh_comp_ptr->mesh_instance->is_view_mesh = false;
                s_change_mesh_type_command().execute();
            }
        }
    );

    connect(transform_x_spinbox, &QDoubleSpinBox::valueChanged, this,
        [&]()
        {
            if (current_component_ptr && current_component_ptr->comp_type == COMPONENT_TYPE_MESH)
            {
                auto mesh_comp_ptr = static_cast<mesh_component_invoker*>(current_component_ptr);
                mesh_comp_ptr->object_helper.Transform[0] = transform_x_spinbox->value();
            }
            update_mesh();
        });
    connect(transform_y_spinbox, &QDoubleSpinBox::valueChanged, this,
        [&]()
        {
            if (current_component_ptr && current_component_ptr->comp_type == COMPONENT_TYPE_MESH)
            {
                auto mesh_comp_ptr = static_cast<mesh_component_invoker*>(current_component_ptr);
                mesh_comp_ptr->object_helper.Transform[1] = transform_y_spinbox->value();
            }
            update_mesh();
        });
    connect(transform_z_spinbox, &QDoubleSpinBox::valueChanged, this,
        [&]()
        {
            if (current_component_ptr && current_component_ptr->comp_type == COMPONENT_TYPE_MESH)
            {
                auto mesh_comp_ptr = static_cast<mesh_component_invoker*>(current_component_ptr);
                mesh_comp_ptr->object_helper.Transform[2] = transform_z_spinbox->value();
            }
            update_mesh();
        });
    connect(rotation_x_spinbox, &QDoubleSpinBox::valueChanged, this,
        [&]()
        {
            if (current_component_ptr && current_component_ptr->comp_type == COMPONENT_TYPE_MESH)
            {
                auto mesh_comp_ptr = static_cast<mesh_component_invoker*>(current_component_ptr);
                mesh_comp_ptr->object_helper.Rotation[0] = rotation_x_spinbox->value();
            }
            update_mesh();
        });
    connect(rotation_y_spinbox, &QDoubleSpinBox::valueChanged, this,
        [&]()
        {
            if (current_component_ptr && current_component_ptr->comp_type == COMPONENT_TYPE_MESH)
            {
                auto mesh_comp_ptr = static_cast<mesh_component_invoker*>(current_component_ptr);
                mesh_comp_ptr->object_helper.Rotation[1] = rotation_y_spinbox->value();
            }
            update_mesh();
        });
    connect(rotation_z_spinbox, &QDoubleSpinBox::valueChanged, this,
        [&]()
        {
            if (current_component_ptr && current_component_ptr->comp_type == COMPONENT_TYPE_MESH)
            {
                auto mesh_comp_ptr = static_cast<mesh_component_invoker*>(current_component_ptr);
                mesh_comp_ptr->object_helper.Rotation[2] = rotation_z_spinbox->value();
            }
            update_mesh();
        });
    connect(scale_x_spinbox, &QDoubleSpinBox::valueChanged, this,
        [&]()
        {
            if (current_component_ptr && current_component_ptr->comp_type == COMPONENT_TYPE_MESH)
            {
                auto mesh_comp_ptr = static_cast<mesh_component_invoker*>(current_component_ptr);
                mesh_comp_ptr->object_helper.Scale[0] = scale_x_spinbox->value();
            }
            update_mesh();
        });
    connect(scale_y_spinbox, &QDoubleSpinBox::valueChanged, this,
        [&]()
        {
            if (current_component_ptr && current_component_ptr->comp_type == COMPONENT_TYPE_MESH)
            {
                auto mesh_comp_ptr = static_cast<mesh_component_invoker*>(current_component_ptr);
                mesh_comp_ptr->object_helper.Scale[1] = scale_y_spinbox->value();
            }
            update_mesh();
        });
    connect(scale_z_spinbox, &QDoubleSpinBox::valueChanged, this,
        [&]()
        {
            if (current_component_ptr && current_component_ptr->comp_type == COMPONENT_TYPE_MESH)
            {
                auto mesh_comp_ptr = static_cast<mesh_component_invoker*>(current_component_ptr);
                mesh_comp_ptr->object_helper.Scale[2] = scale_z_spinbox->value();
            }
            update_mesh();
        });
}

void mesh_property_widget::refresh_spin_box()
{
    if (current_component_ptr && current_component_ptr->comp_type == COMPONENT_TYPE_MESH)
    {
        auto mesh_comp_ptr = static_cast<mesh_component_invoker*>(current_component_ptr);

        transform_x_spinbox->setValue(mesh_comp_ptr->object_helper.Transform[0]);
        transform_y_spinbox->setValue(mesh_comp_ptr->object_helper.Transform[1]);
        transform_z_spinbox->setValue(mesh_comp_ptr->object_helper.Transform[2]);
        
        rotation_x_spinbox->setValue(mesh_comp_ptr->object_helper.Rotation[0]);
        rotation_y_spinbox->setValue(mesh_comp_ptr->object_helper.Rotation[1]);
        rotation_z_spinbox->setValue(mesh_comp_ptr->object_helper.Rotation[2]);
        
        scale_x_spinbox->setValue(mesh_comp_ptr->object_helper.Scale[0]);
        scale_y_spinbox->setValue(mesh_comp_ptr->object_helper.Scale[1]);
        scale_z_spinbox->setValue(mesh_comp_ptr->object_helper.Scale[2]);
    }
}

void mesh_property_widget::update_mesh()
{
    if (current_component_ptr && current_component_ptr->comp_type == COMPONENT_TYPE_MESH)
    {
        auto mesh_comp_ptr = static_cast<mesh_component_invoker*>(current_component_ptr);
        s_update_mesh_data_command update_mesh_data_cmd;
        update_mesh_data_cmd.mesh_ptr = mesh_comp_ptr;
        update_mesh_data_cmd.execute();
        update_mesh_data_cmd.mesh_ptr = nullptr;
    }
}

shader_property_widget::shader_property_widget(QTabWidget* in_parent_tab_widget) : QWidget(in_parent_tab_widget)
{
    current_shader_property_widget = this;
    setObjectName("shader_property_tab");


    //文件路径选择
    path_text = new QTextEdit(this);
    path_text->setObjectName("path_text");
    path_text->setEnabled(false);
    path_text->setGeometry(QRect(20, 60, 221, 31));
    path_select_pushbutton = new QPushButton(this);
    path_select_pushbutton->setObjectName("path_select_pushbutton");
    path_select_pushbutton->setGeometry(QRect(250, 60, 41, 31));
    path_select_pushbutton->setText("/...");

    connect(path_select_pushbutton, &QPushButton::clicked, this,
        [&]()
        {
            auto path_str = open_file_path(this, "Shader Files(*.hlsl);;All(*.*)");
            path_text->setText(path_str);
            if (current_component_ptr && current_component_ptr->comp_type == COMPONENT_TYPE_SHADER)
            {
                auto shader_comp_ptr = static_cast<shader_component_invoker*>(current_component_ptr);
                shader_comp_ptr->shader_layout_instance.shader_path[0] = path_str.toStdWString();
                shader_comp_ptr->shader_layout_instance.shader_path[1] = path_str.toStdWString();
                shader_comp_ptr->shader_layout_instance.shader_path[2] = path_str.toStdWString();
                shader_comp_ptr->shader_layout_instance.shader_path[3] = path_str.toStdWString();
                shader_comp_ptr->shader_layout_instance.shader_path[4] = path_str.toStdWString();

                //重连 重连保证所连的pass全部重新编译
                s_reconnect_resource_command reconnect_cmd;
                reconnect_cmd.reconnect_port = shader_comp_ptr->output_port;
                reconnect_cmd.execute();
                reconnect_cmd.reconnect_port = nullptr;
            }


        }
    );

    vs_check_box = new QCheckBox(this);
    vs_check_box->setObjectName("vs_check_box");
    vs_check_box->setGeometry(QRect(20, 90, 100, 31));
    vs_check_box->setText("VS");

    ds_check_box = new QCheckBox(this);
    ds_check_box->setObjectName("ds_check_box");
    ds_check_box->setGeometry(QRect(20, 120, 100, 31));
    ds_check_box->setText("DS");

    hs_check_box = new QCheckBox(this);
    hs_check_box->setObjectName("hs_check_box");
    hs_check_box->setGeometry(QRect(20, 150, 100, 31));
    hs_check_box->setText("HS");

    gs_check_box = new QCheckBox(this);
    gs_check_box->setObjectName("gs_check_box");
    gs_check_box->setGeometry(QRect(20, 180, 100, 31));
    gs_check_box->setText("GS");

    ps_check_box = new QCheckBox(this);
    ps_check_box->setObjectName("ps_check_box");
    ps_check_box->setGeometry(QRect(20, 210, 100, 31));
    ps_check_box->setText("PS");

    connect(vs_check_box, &QCheckBox::clicked, this,
        [&]() 
        {
            if (current_component_ptr && current_component_ptr->comp_type == COMPONENT_TYPE_SHADER)
            {
                auto shader_comp_ptr = static_cast<shader_component_invoker*>(current_component_ptr);
                shader_comp_ptr->shader_layout_instance.shader_vaild[0] = vs_check_box->isChecked();
                
                //重连 重连保证所连的pass全部重新编译
                s_reconnect_resource_command reconnect_cmd;
                reconnect_cmd.reconnect_port = shader_comp_ptr->output_port;
                reconnect_cmd.execute();
                reconnect_cmd.reconnect_port = nullptr;
            }
        }
    );
    connect(ds_check_box, &QCheckBox::clicked, this,
        [&]()
        {
            if (current_component_ptr && current_component_ptr->comp_type == COMPONENT_TYPE_SHADER)
            {
                auto shader_comp_ptr = static_cast<shader_component_invoker*>(current_component_ptr);
                shader_comp_ptr->shader_layout_instance.shader_vaild[1] = ds_check_box->isChecked();

                //重连 重连保证所连的pass全部重新编译
                s_reconnect_resource_command reconnect_cmd;
                reconnect_cmd.reconnect_port = shader_comp_ptr->output_port;
                reconnect_cmd.execute();
                reconnect_cmd.reconnect_port = nullptr;
            }
        }
    );
    connect(hs_check_box, &QCheckBox::clicked, this,
        [&]()
        {
            if (current_component_ptr && current_component_ptr->comp_type == COMPONENT_TYPE_SHADER)
            {
                auto shader_comp_ptr = static_cast<shader_component_invoker*>(current_component_ptr);
                shader_comp_ptr->shader_layout_instance.shader_vaild[2] = hs_check_box->isChecked();

                //重连 重连保证所连的pass全部重新编译
                s_reconnect_resource_command reconnect_cmd;
                reconnect_cmd.reconnect_port = shader_comp_ptr->output_port;
                reconnect_cmd.execute();
                reconnect_cmd.reconnect_port = nullptr;
            }
        }
    );
    connect(gs_check_box, &QCheckBox::clicked, this,
        [&]()
        {
            if (current_component_ptr && current_component_ptr->comp_type == COMPONENT_TYPE_SHADER)
            {
                auto shader_comp_ptr = static_cast<shader_component_invoker*>(current_component_ptr);
                shader_comp_ptr->shader_layout_instance.shader_vaild[3] = gs_check_box->isChecked();

                //重连 重连保证所连的pass全部重新编译
                s_reconnect_resource_command reconnect_cmd;
                reconnect_cmd.reconnect_port = shader_comp_ptr->output_port;
                reconnect_cmd.execute();
                reconnect_cmd.reconnect_port = nullptr;
            }
        }
    );
    connect(ps_check_box, &QCheckBox::clicked, this,
        [&]()
        {
            if (current_component_ptr && current_component_ptr->comp_type == COMPONENT_TYPE_SHADER)
            {
                auto shader_comp_ptr = static_cast<shader_component_invoker*>(current_component_ptr);
                shader_comp_ptr->shader_layout_instance.shader_vaild[4] = ps_check_box->isChecked();

                //重连 重连保证所连的pass全部重新编译
                s_reconnect_resource_command reconnect_cmd;
                reconnect_cmd.reconnect_port = shader_comp_ptr->output_port;
                reconnect_cmd.execute();
                reconnect_cmd.reconnect_port = nullptr;
            }
        }
    );


}

texture_property_widget::texture_property_widget(QTabWidget * in_parent_tab_widget) : QWidget(in_parent_tab_widget)
{
    current_texture_property_widget = this;
    setObjectName("texture_property_tab");

    //文件路径选择
    path_text = new QTextEdit(this);
    path_text->setObjectName("path_text");
    path_text->setEnabled(false);
    path_text->setGeometry(QRect(20, 60, 221, 31));
    path_select_pushbutton = new QPushButton(this);
    path_select_pushbutton->setObjectName("path_select_pushbutton");
    path_select_pushbutton->setGeometry(QRect(250, 60, 41, 31));
    path_select_pushbutton->setText("/...");

    //类型选择
    type_select_comcobox = new QComboBox(this);
    type_select_comcobox->setObjectName("type_select_comcobox");
    type_select_comcobox->setGeometry(QRect(20, 30, 271, 22));

    type_select_comcobox->addItem("texture");
    type_select_comcobox->addItem("render targer");
    type_select_comcobox->addItem("depth stencil");

    connect(type_select_comcobox, &QComboBox::currentIndexChanged, this, [&](int index)
        {
            change_combobox_index(index);
        }
    );

    connect(path_select_pushbutton, &QPushButton::clicked, this,
        [&]()
        {
            auto path_str = open_file_path(this, "Shader Files(*.hlsl);;All(*.*)");
            path_text->setText(path_str);
            if (current_component_ptr && current_component_ptr->comp_type == COMPONENT_TYPE_TEXTURE)
            {
                auto texture_comp_ptr = static_cast<texture_element_invoker*>(current_component_ptr);
                texture_comp_ptr->texture_instance->path = path_str.toStdWString();
            }
        }
    );

}

void texture_property_widget::change_combobox_index(int index)
{
    if (!current_component_ptr && current_component_ptr->comp_type != COMPONENT_TYPE_TEXTURE)
    {
        return;
    }
    auto texture_comp_ptr = static_cast<texture_element_invoker*>(current_component_ptr);

    switch (index)
    {
    case 0:// texture
    {
        texture_comp_ptr->texture_instance->gpu_sr_ptr->shader_resource_type = gpu_shader_resource::SHADER_RESOURCE_TYPE_TEXTURE;
    }
    break;
    case 1:// render targer
    {
        texture_comp_ptr->texture_instance->gpu_sr_ptr->shader_resource_type = gpu_shader_resource::SHADER_RESOURCE_TYPE_RENDER_TARGET;
    }
    break;
    case 2:// depth stencil
    {
        texture_comp_ptr->texture_instance->gpu_sr_ptr->shader_resource_type = gpu_shader_resource::SHADER_RESOURCE_TYPE_RENDER_DEPTH_STENCIL;
    }
    break;
    }
    //重新分配贴图
    s_update_texture_gpu_command update_texture_gpu_cmd;
    update_texture_gpu_cmd.texture_update_ptr = texture_comp_ptr->texture_instance;
    update_texture_gpu_cmd.execute();
    update_texture_gpu_cmd.texture_update_ptr = nullptr;
    
    //先刷新
    s_update_gpu_command().execute();
    
    //重新打包贴图组
    auto texture_component_package_texture_ptr = static_cast<texture_component_invoker*>(current_component_ptr->parent());
    texture_component_package_texture_ptr->update_package();
    
    //先刷新
    s_update_gpu_command().execute();
}

texture_group_property_widget::texture_group_property_widget(QTabWidget* in_parent_tab_widget) : QWidget(in_parent_tab_widget)
{
    current_texture_group_property_widget = this;
    setObjectName("texture_group_property_tab");

    //类型选择
    type_select_comcobox = new QComboBox(this);
    type_select_comcobox->setObjectName("type_select_comcobox");
    type_select_comcobox->setGeometry(QRect(20, 30, 271, 22));

    type_select_comcobox->addItem("texture group");
    type_select_comcobox->addItem("render targer group");
    type_select_comcobox->addItem("depth stencil group");

    connect(type_select_comcobox, &QComboBox::currentIndexChanged, this, [&](int index)
        {
            change_combobox_index(index);
        }
    );

}

void texture_group_property_widget::change_combobox_index(int index)
{
    if (!current_component_ptr && current_component_ptr->comp_type != COMPONENT_TYPE_TEXTURE_GROUP)
    {
        return;
    }

    auto texture_comp_ptr = static_cast<texture_component_invoker*>(current_component_ptr);

    switch (index)
    {
    case 0:// texture group
    {
        texture_comp_ptr->texture_instance->gpu_sr_ptr->shader_resource_type = gpu_shader_resource::SHADER_RESOURCE_TYPE_TEXTURE_GROUP;
    }
    break;
    case 1:// render targer group
    {
        texture_comp_ptr->texture_instance->gpu_sr_ptr->shader_resource_type = gpu_shader_resource::SHADER_RESOURCE_TYPE_RENDER_TARGET_GROUP;
    }
    break;
    case 2:// depth stencil group
    {
        texture_comp_ptr->texture_instance->gpu_sr_ptr->shader_resource_type = gpu_shader_resource::SHADER_RESOURCE_TYPE_RENDER_DEPTH_STENCIL_GROUP;
    }
    break;
    }

    //先刷新
    s_update_gpu_command().execute();

    //重新打包贴图组
    texture_comp_ptr->update_package();

    //先刷新
    s_update_gpu_command().execute();
}

pass_property_widget::pass_property_widget(QTabWidget* in_parent_tab_widget)
{
    current_pass_property_widget = this;
    setObjectName("pass_property_tab");

    //类型选择
    pass_level_label = new QLabel(this);
    pass_level_label->setObjectName("pass_level_label");
    pass_level_label->setGeometry(QRect(20, 10, 271, 22));
    pass_level_label->setText("level");
    pass_level_comcobox = new QComboBox(this);
    pass_level_comcobox->setObjectName("pass_level_comcobox");
    pass_level_comcobox->setGeometry(QRect(20, 30, 271, 22));

    for (int i = 0; i < pass_max_level; i++)
    {
        pass_level_comcobox->addItem(QString::fromStdString(std::to_string(i)));
    }
    

    connect(pass_level_comcobox, &QComboBox::currentIndexChanged, this, [&](int index)
        {
            if (current_component_ptr && current_component_ptr->comp_type == COMPONENT_TYPE_PASS)
            {
                auto pass_comp_ptr = static_cast<pass_component_invoker*>(current_component_ptr);
                
                pipeline_window_widget_ptr->pass_comp_level_map[pass_comp_ptr->level].erase(pass_comp_ptr);
                pass_comp_ptr->level = index;
                pipeline_window_widget_ptr->pass_comp_level_map[pass_comp_ptr->level].insert(pass_comp_ptr);
            
            }
        }
    );


    is_output_check_box = new QCheckBox(this);
    is_output_check_box->setObjectName("is_output_check_box");
    is_output_check_box->setGeometry(QRect(20, 90, 100, 31));
    is_output_check_box->setText("is output");

    is_depth_check_box = new QCheckBox(this);
    is_depth_check_box->setObjectName("is_depth_check_box");
    is_depth_check_box->setGeometry(QRect(20, 120, 100, 31));
    is_depth_check_box->setText("check depth");

    is_translate_check_box = new QCheckBox(this);
    is_translate_check_box->setObjectName("is_translate_check_box");
    is_translate_check_box->setGeometry(QRect(20, 150, 100, 31));
    is_translate_check_box->setText("is translate");

    connect(is_output_check_box, &QCheckBox::clicked, this,
        [&]()
        {
            if (current_component_ptr && current_component_ptr->comp_type == COMPONENT_TYPE_PASS)
            {
                auto pass_comp_ptr = static_cast<pass_component_invoker*>(current_component_ptr);

                pass_comp_ptr->pass_instance->is_output = is_output_check_box->isChecked();
                had_output_pass = is_output_check_box->isChecked();
                
                //重连 保证pass重新编译
                s_reconnect_resource_command reconnect_cmd;
                reconnect_cmd.reconnect_port = pass_comp_ptr->shader_port;
                reconnect_cmd.execute();
                reconnect_cmd.reconnect_port = nullptr;

            }

        }
    );

    connect(is_depth_check_box, &QCheckBox::clicked, this,
        [&]()
        {
            if (current_component_ptr && current_component_ptr->comp_type == COMPONENT_TYPE_PASS)
            {
                auto pass_comp_ptr = static_cast<pass_component_invoker*>(current_component_ptr);

                pass_comp_ptr->pass_instance->is_depth_check = is_depth_check_box->isChecked();

                //重连 保证pass重新编译
                s_reconnect_resource_command reconnect_cmd;
                reconnect_cmd.reconnect_port = pass_comp_ptr->shader_port;
                reconnect_cmd.execute();
                reconnect_cmd.reconnect_port = nullptr;
            }

        }
    );

    connect(is_translate_check_box, &QCheckBox::clicked, this,
        [&]()
        {
            if (current_component_ptr && current_component_ptr->comp_type == COMPONENT_TYPE_PASS)
            {
                auto pass_comp_ptr = static_cast<pass_component_invoker*>(current_component_ptr);

                pass_comp_ptr->pass_instance->is_translate = is_translate_check_box->isChecked();

                //重连 保证pass重新编译
                s_reconnect_resource_command reconnect_cmd;
                reconnect_cmd.reconnect_port = pass_comp_ptr->shader_port;
                reconnect_cmd.execute();
                reconnect_cmd.reconnect_port = nullptr;
            }

        }
    );
    

    //is_start_check_box = new QCheckBox(this);
    //is_start_check_box->setObjectName("is_output_check_box");
    //is_start_check_box->setGeometry(QRect(20, 210, 100, 31));
    //is_start_check_box->setText("is start");
    //connect(is_start_check_box, &QCheckBox::clicked, this,
    //    [&]()
    //    {
    //        if (current_component_ptr->comp_type == COMPONENT_TYPE_PASS)
    //        {
    //            auto pass_comp_ptr = static_cast<pass_component_invoker*>(current_component_ptr);
    //            if (is_start_check_box->isChecked())
    //            {
    //                pipeline_window_widget_ptr->pass_comp_level_map[0].insert(pass_comp_ptr);
    //            }
    //            else
    //            {
    //                pipeline_window_widget_ptr->pass_comp_level_map[0].erase(pass_comp_ptr);
    //            }
    //        }
    //    }
    //);
}

QString open_file_path(QWidget* in_widget,const QString file_param)
{
    QString OpenFile, OpenFilePath;
    
    //打开文件夹中的图片文件
    OpenFile = QFileDialog::getOpenFileName(in_widget,
        "Please choose an file",
        "",
        file_param);//"Image Files(*.jpg *.png *.bmp *.pgm *.pbm);;All(*.*)"
    if (OpenFile != "")
    {

    }

    //显示所示图片的路径
    QFileInfo OpenFileInfo;
    OpenFileInfo = QFileInfo(OpenFile);
    OpenFilePath = OpenFileInfo.filePath();
    return OpenFilePath;
}

