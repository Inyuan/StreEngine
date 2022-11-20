#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_stre_editor.h"
#include "s_invoker.h"

class stre_editor : public QMainWindow
{
    Q_OBJECT

public:
    stre_editor(QWidget *parent = nullptr);
    ~stre_editor();




private:
    Ui::stre_editorClass ui;

    view_port_invoker* view_port_instance = nullptr;

    pipeline_window_invoker* pipeline_window_instance = nullptr;
};
