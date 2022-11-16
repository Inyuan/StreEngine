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

    pipeline_window_invoker* pipeline_window_instance = nullptr;
};
