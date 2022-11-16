#include "stre_editor.h"
#include "concrete_command.h"
stre_editor::stre_editor(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
    
    pipeline_window_instance 
        = new pipeline_window_invoker(
            this,
            new s_create_pass_command(),
            new s_create_texture_command(),
            new s_create_mesh_command(),
            new s_create_shader_commd());

    pipeline_window_instance->setObjectName("pipeline_blue_print_widget");
    pipeline_window_instance->setGeometry(QRect(10, 10, 551, 281));

}



stre_editor::~stre_editor()
{}
