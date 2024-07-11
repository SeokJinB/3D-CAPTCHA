#include <qKeyEvent>
#include "Viewer.h"
#include "qglviewer/camera.h"
#include "qglviewer/manipulatedCameraFrame.h"
#include "qglviewer/manipulatedFrame.h"
#include "GL/glut.h"
#include "GL/freeglut.h"
#include <cmath>

// ���� �������κ��� ������ �Ÿ�
#define DIST 0.55

Viewer::Viewer(QWidget* parent) : QGLViewer(parent)
{
    setParent(parent);
}

Viewer::~Viewer()
{
}

void Viewer::initializeGL()
{
    glClearColor(1, 1, 1, 1);
    glClearDepth(1.0);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_NORMALIZE);
    glEnable(GL_POINT_SMOOTH);
    glEnable(GL_LINE_SMOOTH);
    glFrontFace(GL_CCW);
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
    GLfloat ambient[] = { 0.1,0.1,0.1 };
    GLfloat diffuse[] = { 0.4,0.4,0.4 };
    GLfloat light0Pos[] = { 100,100,100,0 };
    GLfloat light1Pos[] = { -100,100,100,0 };
    GLfloat specular[] = { 0.4,0.4,0.4 };
    glEnable(GL_LIGHTING);
    glLightfv(GL_LIGHT0, GL_POSITION, light0Pos);
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
    glEnable(GL_LIGHT0);
    /*glLightfv(GL_LIGHT1, GL_POSITION, light1Pos);
    glLightfv(GL_LIGHT1, GL_AMBIENT, ambient);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuse);
    glLightfv(GL_LIGHT1, GL_SPECULAR, specular);
    glEnable(GL_LIGHT1);*/
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
    glMateriali(GL_FRONT_AND_BACK, GL_SHININESS, 10);

    glPointSize(10);
    glLineWidth(3.0);
    //restoreStateFromFile();

    dc.Init();
}

void Viewer::draw()
{
    dc.Draw();
}

float angle = M_PI / 2.0; //�ʱ� ����
void Viewer::keyPressEvent(QKeyEvent* e)
{
    float rotationAngle = 5.0f;

    int key = e->key();

    if (key == Qt::Key_S)
    {
        saveStateToFile();
    }
    else if (key == Qt::Key_L)
    {
        restoreStateFromFile();
    }
    else if (key == Qt::Key_Comma)
    {
        setSceneRadius(sceneRadius() * 10.0);
    }
    else if (key == Qt::Key_Period)
    {
        setSceneRadius(sceneRadius() * 10.0);
    }
    else if (key == '1')
    {
        qglviewer::Vec position(DIST * cos(angle), -DIST * sin(angle), 0); // ��ġ ����
        camera()->frame()->setPosition(position); // ��ġ ����
        angle -= 0.1; // ���� ��ȯ + ���� - ������
        camera()->lookAt(qglviewer::Vec(0, 0, 0)); // ī�޶� �ٶ󺸴�  ����
    }
    else
        QGLViewer::keyPressEvent(e);

    update();
}

void Viewer::mousePressEvent(QMouseEvent* e)
{
    QGLViewer::mousePressEvent(e);
}

void Viewer::mouseMoveEvent(QMouseEvent* e)
{
    QGLViewer::mouseMoveEvent(e);
}

void Viewer::mouseReleaseEvent(QMouseEvent* e)
{
    QGLViewer::mouseReleaseEvent(e);
}