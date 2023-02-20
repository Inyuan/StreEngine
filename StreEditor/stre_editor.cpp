#include "stre_editor.h"
#include "concrete_command.h"
stre_editor::stre_editor(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
    
    resize(1600,900);

    view_window_widget_instance = new QWidget();
    view_window_widget_instance->resize(1920,1080);
    view_port_instance = new view_port_invoker(view_window_widget_instance);

    stre_engine::get_instance()->render_system_init(
        (HWND)view_port_instance->winId(),
        view_port_instance->geometry().width(),
        view_port_instance->geometry().height());

    pipeline_window_instance = new pipeline_window_invoker(this);
    pipeline_window_instance->setObjectName("pipeline_blue_print_widget");
    pipeline_window_instance->setGeometry(QRect(0, 0, 1280, 850));
    
    information_window_instance = new property_tab_widget(this);
    information_window_instance->setObjectName("information_widget");
    information_window_instance->setGeometry(QRect(1280,0, 319, 899));

    debug_output_window_instance = new debug_text_invoker(this);

    s_switch_property_widget_command().execute();
    view_window_widget_instance->show();
    view_port_instance->repaint();
}



stre_editor::~stre_editor()
{
    delete(view_port_instance);
    delete(pipeline_window_instance);
    delete(view_window_widget_instance);
}
