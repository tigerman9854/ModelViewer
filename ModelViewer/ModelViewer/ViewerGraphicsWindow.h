#pragma once
#include "OpenGLWindow.h"
#include "ModelLoader.h"

#include <QOpenGLShaderProgram>

class ViewerGraphicsWindow : public OpenGLWindow
{
public:
    using OpenGLWindow::OpenGLWindow;

    ViewerGraphicsWindow(QWindow* parent = nullptr);

    void initialize() override;
    void render() override;

    bool loadModel(QString filepath = QString());

private:
    bool initialized = false;

    GLint m_posAttr = 0;
    GLint m_normAttr = -1;
    GLint m_uvAttr = -1;
    GLint m_colAttr = 0;
    GLint m_matrixUniform = 0;

    QOpenGLShaderProgram* m_program = nullptr;
    int m_frame = 0;

    Model m_currentModel;

    // Mouse vars & functions
    void mousePressEvent(QMouseEvent*);
    void mouseMoveEvent(QMouseEvent*);
    void mouseReleaseEvent(QMouseEvent*);
    QMatrix4x4 mouseMatrix;
    int lastX;
    int lastY;
    bool leftMousePressed;
    bool rightMousePressed;
};
