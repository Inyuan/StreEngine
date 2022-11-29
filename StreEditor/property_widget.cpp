#include "property_widget.h"
#include <QFileDialog>
#include "s_invoker.h"
#include "concrete_command.h"

QString open_file_path(QWidget* in_widget, const QString file_param);

//属性面板代码设计
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
               s_change_mesh_data_command().execute();
            }

        }
    );

    connect(select_fbx_mesh_act, &QAction::triggered, this, 
        [&]() 
        {
            path_select_pushbutton->setEnabled(true);
        }
    );

    //
    connect(path_select_pushbutton, &QPushButton::clicked,this,
        [&]() 
        {
            auto path_str = open_file_path(this, "FBX Files(*.fbx);;All(*.*)");
            path_text->setText(path_str);
            if (current_component_ptr->comp_type == COMPONENT_TYPE_MESH)
            {
                auto mesh_comp_ptr = static_cast<mesh_component_invoker*>(current_component_ptr);
                mesh_comp_ptr->mesh_instance->path = path_str.toStdWString();
                mesh_comp_ptr->mesh_instance->is_view_mesh = false;
                s_change_mesh_data_command().execute();
            }
        }
    );


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
            if (current_component_ptr->comp_type == COMPONENT_TYPE_SHADER)
            {
                auto shader_comp_ptr = static_cast<shader_component_invoker*>(current_component_ptr);
                shader_comp_ptr->shader_layout_instance.shader_path[0] = path_str.toStdWString();
                shader_comp_ptr->shader_layout_instance.shader_path[1] = path_str.toStdWString();
                shader_comp_ptr->shader_layout_instance.shader_path[2] = path_str.toStdWString();
                shader_comp_ptr->shader_layout_instance.shader_path[3] = path_str.toStdWString();
                shader_comp_ptr->shader_layout_instance.shader_path[4] = path_str.toStdWString();

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
}

pass_property_widget::pass_property_widget(QTabWidget* in_parent_tab_widget)
{
    current_pass_property_widget = this;
    setObjectName("pass_property_tab");

    //类型选择
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

