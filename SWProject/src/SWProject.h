#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_SWProject.h"

class SWProject : public QMainWindow
{
    Q_OBJECT

public:
    SWProject(QWidget *parent = nullptr);
    ~SWProject();

private:
    Ui::SWProjectClass ui;
};
