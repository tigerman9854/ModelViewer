#pragma once
#include "OpenGLWindow.h"

#include <QOpenGLShaderProgram>

class ViewerGraphicsWindow : public OpenGLWindow
{
public:
    using OpenGLWindow::OpenGLWindow;

    ViewerGraphicsWindow(QWindow* parent = nullptr);

    void initialize() override;
    void render() override;

private:
    GLint m_posAttr = 0;
    GLint m_colAttr = 0;
    GLint m_matrixUniform = 0;

    QOpenGLShaderProgram* m_program = nullptr;
    int m_frame = 0;
};