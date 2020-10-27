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
    void resetView();

    bool loadModel(QString filepath = QString());
    bool addPrimitive(QString filepath);

    // Mouse state
    bool leftMousePressed = false;
    bool rightMousePressed = false;

    // Mouse settings | % adjustment
    float panXSensitivity = .01f;
    float panYSensitivity = .01f;
    float xRotateSensitivity = 0.6f;
    float yRotateSensitivity = 0.6f;
    float zoomSensitivity = 0.001f;
    float fieldOfView = 60.f;
    float nearPlane = 0.1f;
    float farPlane = 100.f;

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

    // Mouse vars
    int lastX;
    int lastY;
    
    QVector3D rotationVector;

    QMatrix4x4 scaleMatrix;
    QMatrix4x4 rotMatrix;
    QMatrix4x4 transMatrix;

protected:
    // Mouse functions
    void mousePressEvent(QMouseEvent*) override;
    void mouseMoveEvent(QMouseEvent*) override;
    void mouseReleaseEvent(QMouseEvent*) override;
    void wheelEvent(QWheelEvent*) override;
};
