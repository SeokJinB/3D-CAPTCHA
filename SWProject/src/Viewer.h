#pragma once
#include "QGLViewer/qglviewer.h"
#include "DrawComponent.h"


class Viewer : public QGLViewer
{
    public:
    Viewer(QWidget* parent);
    virtual ~Viewer();

    void initializeGL() override;
    void draw() override;
    void keyPressEvent(QKeyEvent* e) override;
    void mousePressEvent(QMouseEvent* e) override;
    void mouseMoveEvent(QMouseEvent* e) override;
    void mouseReleaseEvent(QMouseEvent* e) override;

    DrawComponent dc;
};
