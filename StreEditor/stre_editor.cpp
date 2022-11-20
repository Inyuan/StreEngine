#include "stre_editor.h"
#include "concrete_command.h"
stre_editor::stre_editor(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
    
    view_port_instance = new view_port_invoker(this, new s_draw_command());

    stre_engine::get_instance()->render_system_init(
        (HWND)view_port_instance->winId(),
        ui.rendering_view_widget->geometry().width(),
        ui.rendering_view_widget->geometry().height());

    pipeline_window_instance 
        = new pipeline_window_invoker(
            this,
            new s_create_pass_command(),
            new s_create_texture_group_command(),
            new s_create_mesh_command(),
            new s_create_shader_command());

    pipeline_window_instance->setObjectName("pipeline_blue_print_widget");
    pipeline_window_instance->setGeometry(QRect(10, 10, 551, 281));
    
    view_port_instance->repaint();
}



stre_editor::~stre_editor()
{

}
