#include "stre_editor.h"
#include "concrete_command.h"
stre_editor::stre_editor(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
    
    view_port_instance = new view_port_invoker(this);

    stre_engine::get_instance()->render_system_init(
        (HWND)view_port_instance->winId(),
        ui.rendering_view_widget->geometry().width(),
        ui.rendering_view_widget->geometry().height());

    pipeline_window_instance = new pipeline_window_invoker(this);
    pipeline_window_instance->setObjectName("pipeline_blue_print_widget");
    pipeline_window_instance->setGeometry(QRect(10, 10, 1071, 511));
    
    information_window_instance = new property_tab_widget(this);
    information_window_instance->setObjectName("information_widget");
    information_window_instance->setGeometry(QRect(1100, 270, 321, 481));

    debug_output_window_instance = new debug_text_invoker(this);

    s_switch_property_widget_command().execute();
    
    view_port_instance->repaint();
}



stre_editor::~stre_editor()
{
    delete(view_port_instance);
    delete(pipeline_window_instance);
}
