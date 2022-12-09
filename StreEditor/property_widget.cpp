#include "property_widget.h"
#include <QFileDialog>
#include "s_invoker.h"
#include "concrete_command.h"

QString open_file_path(QWidget* in_widget, const QString file_param);

//�������������
//��������� ��property_tab_widget ����Լ�
//�ڹ��캯�� д���ƿؼ�
//��externȫ�ֱ���
//��s_switch_property_widget_command ����Լ��ķ�����Ϣ

//ûд��ô�ã�ֻ�ܸ�ֵһ��
property_tab_widget* current_property_tab_widget = nullptr;
property_widget* current_empty_property_widget = nullptr;
mesh_property_widget* current_mesh_property_widget = nullptr;
shader_property_widget* current_shader_property_widget = nullptr;
texture_property_widget* current_texture_property_widget = nullptr;
texture_group_property_widget* current_texture_group_property_widget = nullptr;
pass_property_widget* current_pass_property_widget = nullptr;

bool had_output_pass = false;

extern component_invoker* current_component_ptr;
extern pipeline_window_invoker* pipeline_window_widget_ptr;
extern const int pass_max_level;

property_tab_widget::property_tab_widget(QWidget* in_parent) :QTabWidget(in_parent)
{
    current_property_tab_widget = this;
    //ûд��ô�ã�ֻ�ܹ���һ��
    empty_property_window_instance = new property_widget(this);
    mesh_property_window_instance = new mesh_property_widget(this);
    shader_property_window_instance = new shader_property_widget(this);
    texture_property_window_instance = new texture_property_widget(this);
    texture_group_property_window_instance = new texture_group_property_widget(this);
    pass_property_window_instance = new pass_property_widget(this);

    addTab(empty_property_window_instance,"");
    addTab(mesh_property_window_instance, "mesh property");
    addTab(shader_property_window_instance, "shader property");
    addTab(texture_property_window_instance, "texture property");
    addTab(texture_group_property_window_instance, "texture group property");
    addTab(pass_property_window_instance, "pass property");
}

property_widget::property_widget(QTabWidget* in_parent_tab_widget) : QWidget(in_parent_tab_widget)
{
    current_empty_property_widget = this;
}

mesh_property_widget::mesh_property_widget(QTabWidget* in_parent_tab_widget) : QWidget(in_parent_tab_widget)
{
    current_mesh_property_widget = this;
	setObjectName("mesh_property_tab");

    //����ѡ��
	type_select_comcobox = new QComboBox(this);
	type_select_comcobox->setObjectName("type_select_comcobox");
	type_select_comcobox->setGeometry(QRect(20, 30, 271, 22));
    
    //�ļ�·��ѡ��
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
    transform_x_spinbox->setObjectName("transform_x_spinbox");
    transform_x_spinbox->setGeometry(QRect(30, 110, 62, 22));
    transform_y_spinbox = new QDoubleSpinBox(this);
    transform_y_spinbox->setObjectName("transform_y_spinbox");
    transform_y_spinbox->setGeometry(QRect(100, 110, 62, 22));
    transform_z_spinbox = new QDoubleSpinBox(this);
    transform_z_spinbox->setObjectName("transform_z_spinbox");
    transform_z_spinbox->setGeometry(QRect(170, 110, 62, 22));
    transform_label->setText("transform");

    rotation_label = new QLabel(this);
    rotation_label->setObjectName("rotation_label");
    rotation_label->setGeometry(QRect(30, 130, 53, 16));
    rotation_x_spinbox = new QDoubleSpinBox(this);
    rotation_x_spinbox->setObjectName("rotation_x_spinbox");
    rotation_x_spinbox->setGeometry(QRect(30, 150, 62, 22));
    rotation_y_spinbox = new QDoubleSpinBox(this);
    rotation_y_spinbox->setObjectName("rotation_y_spinbox");
    rotation_y_spinbox->setGeometry(QRect(100, 150, 62, 22));
    rotation_z_spinbox = new QDoubleSpinBox(this);
    rotation_z_spinbox->setObjectName("rotation_z_spinbox");
    rotation_z_spinbox->setGeometry(QRect(170, 150, 62, 22));
    rotation_label->setText("rotation");

    scale_label = new QLabel(this);
    scale_label->setObjectName("scale_label");
    scale_label->setGeometry(QRect(30, 170, 53, 16));
    scale_x_spinbox = new QDoubleSpinBox(this);
    scale_x_spinbox->setObjectName("scale_x_spinbox");
    scale_x_spinbox->setGeometry(QRect(30, 190, 62, 22));
    scale_y_spinbox = new QDoubleSpinBox(this);
    scale_y_spinbox->setObjectName("scale_y_spinbox");
    scale_y_spinbox->setGeometry(QRect(100, 190, 62, 22));
    scale_z_spinbox = new QDoubleSpinBox(this);
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
            if (current_component_ptr->comp_type == COMPONENT_TYPE_MESH)
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
            if (current_component_ptr->comp_type == COMPONENT_TYPE_MESH)
            {
                auto mesh_comp_ptr = static_cast<mesh_component_invoker*>(current_component_ptr);
                mesh_comp_ptr->object_helper.Transform[0] = transform_x_spinbox->value();
            }
            update_mesh();
        });
    connect(transform_y_spinbox, &QDoubleSpinBox::valueChanged, this,
        [&]()
        {
            if (current_component_ptr->comp_type == COMPONENT_TYPE_MESH)
            {
                auto mesh_comp_ptr = static_cast<mesh_component_invoker*>(current_component_ptr);
                mesh_comp_ptr->object_helper.Transform[1] = transform_y_spinbox->value();
            }
            update_mesh();
        });
    connect(transform_z_spinbox, &QDoubleSpinBox::valueChanged, this,
        [&]()
        {
            if (current_component_ptr->comp_type == COMPONENT_TYPE_MESH)
            {
                auto mesh_comp_ptr = static_cast<mesh_component_invoker*>(current_component_ptr);
                mesh_comp_ptr->object_helper.Transform[2] = transform_z_spinbox->value();
            }
            update_mesh();
        });
    connect(rotation_x_spinbox, &QDoubleSpinBox::valueChanged, this,
        [&]()
        {
            if (current_component_ptr->comp_type == COMPONENT_TYPE_MESH)
            {
                auto mesh_comp_ptr = static_cast<mesh_component_invoker*>(current_component_ptr);
                mesh_comp_ptr->object_helper.Rotation[0] = rotation_x_spinbox->value();
            }
            update_mesh();
        });
    connect(rotation_y_spinbox, &QDoubleSpinBox::valueChanged, this,
        [&]()
        {
            if (current_component_ptr->comp_type == COMPONENT_TYPE_MESH)
            {
                auto mesh_comp_ptr = static_cast<mesh_component_invoker*>(current_component_ptr);
                mesh_comp_ptr->object_helper.Rotation[1] = rotation_y_spinbox->value();
            }
            update_mesh();
        });
    connect(rotation_z_spinbox, &QDoubleSpinBox::valueChanged, this,
        [&]()
        {
            if (current_component_ptr->comp_type == COMPONENT_TYPE_MESH)
            {
                auto mesh_comp_ptr = static_cast<mesh_component_invoker*>(current_component_ptr);
                mesh_comp_ptr->object_helper.Rotation[2] = rotation_z_spinbox->value();
            }
            update_mesh();
        });
    connect(scale_x_spinbox, &QDoubleSpinBox::valueChanged, this,
        [&]()
        {
            if (current_component_ptr->comp_type == COMPONENT_TYPE_MESH)
            {
                auto mesh_comp_ptr = static_cast<mesh_component_invoker*>(current_component_ptr);
                mesh_comp_ptr->object_helper.Scale[0] = scale_x_spinbox->value();
            }
            update_mesh();
        });
    connect(scale_y_spinbox, &QDoubleSpinBox::valueChanged, this,
        [&]()
        {
            if (current_component_ptr->comp_type == COMPONENT_TYPE_MESH)
            {
                auto mesh_comp_ptr = static_cast<mesh_component_invoker*>(current_component_ptr);
                mesh_comp_ptr->object_helper.Scale[1] = scale_y_spinbox->value();
            }
            update_mesh();
        });
    connect(scale_z_spinbox, &QDoubleSpinBox::valueChanged, this,
        [&]()
        {
            if (current_component_ptr->comp_type == COMPONENT_TYPE_MESH)
            {
                auto mesh_comp_ptr = static_cast<mesh_component_invoker*>(current_component_ptr);
                mesh_comp_ptr->object_helper.Scale[2] = scale_z_spinbox->value();
            }
            update_mesh();
        });


}
void mesh_property_widget::refresh_spin_box()
{
    if (current_component_ptr->comp_type == COMPONENT_TYPE_MESH)
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
    if (current_component_ptr->comp_type == COMPONENT_TYPE_MESH)
    {
        auto mesh_comp_ptr = static_cast<mesh_component_invoker*>(current_component_ptr);
        if (mesh_comp_ptr->mesh_instance->is_view_mesh)
        {
            return;
        }

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


    //�ļ�·��ѡ��
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
            if (current_component_ptr->comp_type == COMPONENT_TYPE_SHADER)
            {
                auto shader_comp_ptr = static_cast<shader_component_invoker*>(current_component_ptr);
                shader_comp_ptr->shader_layout_instance.shader_path[0] = path_str.toStdWString();
                shader_comp_ptr->shader_layout_instance.shader_path[1] = path_str.toStdWString();
                shader_comp_ptr->shader_layout_instance.shader_path[2] = path_str.toStdWString();
                shader_comp_ptr->shader_layout_instance.shader_path[3] = path_str.toStdWString();
                shader_comp_ptr->shader_layout_instance.shader_path[4] = path_str.toStdWString();

                //���� ������֤������passȫ�����±���
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
            if (current_component_ptr->comp_type == COMPONENT_TYPE_SHADER)
            {
                auto shader_comp_ptr = static_cast<shader_component_invoker*>(current_component_ptr);
                shader_comp_ptr->shader_layout_instance.shader_vaild[0] = vs_check_box->isChecked();
                
                //���� ������֤������passȫ�����±���
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
            if (current_component_ptr->comp_type == COMPONENT_TYPE_SHADER)
            {
                auto shader_comp_ptr = static_cast<shader_component_invoker*>(current_component_ptr);
                shader_comp_ptr->shader_layout_instance.shader_vaild[1] = ds_check_box->isChecked();

                //���� ������֤������passȫ�����±���
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
            if (current_component_ptr->comp_type == COMPONENT_TYPE_SHADER)
            {
                auto shader_comp_ptr = static_cast<shader_component_invoker*>(current_component_ptr);
                shader_comp_ptr->shader_layout_instance.shader_vaild[2] = hs_check_box->isChecked();

                //���� ������֤������passȫ�����±���
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
            if (current_component_ptr->comp_type == COMPONENT_TYPE_SHADER)
            {
                auto shader_comp_ptr = static_cast<shader_component_invoker*>(current_component_ptr);
                shader_comp_ptr->shader_layout_instance.shader_vaild[3] = gs_check_box->isChecked();

                //���� ������֤������passȫ�����±���
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
            if (current_component_ptr->comp_type == COMPONENT_TYPE_SHADER)
            {
                auto shader_comp_ptr = static_cast<shader_component_invoker*>(current_component_ptr);
                shader_comp_ptr->shader_layout_instance.shader_vaild[4] = ps_check_box->isChecked();

                //���� ������֤������passȫ�����±���
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

    //�ļ�·��ѡ��
    path_text = new QTextEdit(this);
    path_text->setObjectName("path_text");
    path_text->setEnabled(false);
    path_text->setGeometry(QRect(20, 60, 221, 31));
    path_select_pushbutton = new QPushButton(this);
    path_select_pushbutton->setObjectName("path_select_pushbutton");
    path_select_pushbutton->setGeometry(QRect(250, 60, 41, 31));
    path_select_pushbutton->setText("/...");

    //����ѡ��
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
            if (current_component_ptr->comp_type == COMPONENT_TYPE_TEXTURE)
            {
                auto texture_comp_ptr = static_cast<texture_element_invoker*>(current_component_ptr);
                texture_comp_ptr->texture_instance->path = path_str.toStdWString();
            }
        }
    );

}

void texture_property_widget::change_combobox_index(int index)
{
    if (current_component_ptr->comp_type != COMPONENT_TYPE_TEXTURE)
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
    //���·�����ͼ
    s_update_texture_gpu_command update_texture_gpu_cmd;
    update_texture_gpu_cmd.texture_update_ptr = texture_comp_ptr->texture_instance;
    update_texture_gpu_cmd.execute();
    update_texture_gpu_cmd.texture_update_ptr = nullptr;
    
    //��ˢ��
    s_update_gpu_command().execute();
    
    //���´����ͼ��
    auto texture_component_package_texture_ptr = static_cast<texture_component_invoker*>(current_component_ptr->parent());
    texture_component_package_texture_ptr->update_package();
    
    //��ˢ��
    s_update_gpu_command().execute();
}

texture_group_property_widget::texture_group_property_widget(QTabWidget* in_parent_tab_widget) : QWidget(in_parent_tab_widget)
{
    current_texture_group_property_widget = this;
    setObjectName("texture_group_property_tab");

    //����ѡ��
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
    if (current_component_ptr->comp_type != COMPONENT_TYPE_TEXTURE_GROUP)
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

    //��ˢ��
    s_update_gpu_command().execute();

    //���´����ͼ��
    texture_comp_ptr->update_package();

    //��ˢ��
    s_update_gpu_command().execute();
}

pass_property_widget::pass_property_widget(QTabWidget* in_parent_tab_widget)
{
    current_pass_property_widget = this;
    setObjectName("pass_property_tab");

    //����ѡ��
    pass_level_comcobox = new QComboBox(this);
    pass_level_comcobox->setObjectName("pass_level_comcobox");
    pass_level_comcobox->setGeometry(QRect(20, 30, 271, 22));

    for (int i = 0; i < pass_max_level; i++)
    {
        pass_level_comcobox->addItem(QString::fromStdString(std::to_string(i)));
    }
    

    connect(pass_level_comcobox, &QComboBox::currentIndexChanged, this, [&](int index)
        {
            if (current_component_ptr->comp_type == COMPONENT_TYPE_PASS)
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
            if (current_component_ptr->comp_type == COMPONENT_TYPE_PASS)
            {
                auto pass_comp_ptr = static_cast<pass_component_invoker*>(current_component_ptr);

                pass_comp_ptr->pass_instance->is_output = is_output_check_box->isChecked();
                had_output_pass = is_output_check_box->isChecked();
                
            }

        }
    );

    connect(is_depth_check_box, &QCheckBox::clicked, this,
        [&]()
        {
            if (current_component_ptr->comp_type == COMPONENT_TYPE_PASS)
            {
                auto pass_comp_ptr = static_cast<pass_component_invoker*>(current_component_ptr);

                pass_comp_ptr->pass_instance->is_depth_check = is_depth_check_box->isChecked();
            }

        }
    );

    connect(is_translate_check_box, &QCheckBox::clicked, this,
        [&]()
        {
            if (current_component_ptr->comp_type == COMPONENT_TYPE_PASS)
            {
                auto pass_comp_ptr = static_cast<pass_component_invoker*>(current_component_ptr);

                pass_comp_ptr->pass_instance->is_translate = is_translate_check_box->isChecked();
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
    
    //���ļ����е�ͼƬ�ļ�
    OpenFile = QFileDialog::getOpenFileName(in_widget,
        "Please choose an file",
        "",
        file_param);//"Image Files(*.jpg *.png *.bmp *.pgm *.pbm);;All(*.*)"
    if (OpenFile != "")
    {

    }

    //��ʾ��ʾͼƬ��·��
    QFileInfo OpenFileInfo;
    OpenFileInfo = QFileInfo(OpenFile);
    OpenFilePath = OpenFileInfo.filePath();
    return OpenFilePath;
}

