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

    bool loadVertexShader(QString vertfilepath = QString());
    bool loadFragmentShader(QString fragfilepath = QString());

    bool addPrimitive(QString filepath);

    // Mouse variables
    QMatrix4x4 sceneMatrix;
    float viewportX = 0;
    float viewportY = 0;

    QMatrix4x4 modelview;

    // Mouse state
    bool leftMousePressed = false;
    bool rightMousePressed = false;

    // Mouse settings | % adjustment
    float viewportXSensitivity = 1;
    float viewportYSensitivity = 1;
    float panXSensitivity = .01;
    float panYSensitivity = .01;
    float xRotateSensitivity = 1;
    float yRotateSensitivity = 1;
    float zoomSensitivity = 1;

private:
    bool initialized = false;

    GLint m_posAttr = 0;
    GLint m_normAttr = 0;
    GLint m_uvAttr = 0;
    GLint m_colAttr = 0;
    
    GLint m_matrixUniform = 0;
    GLint m_modelviewUniform = 0;
    GLint m_normalUniform = 0;
    GLint m_lightPosUniform = 0;
    GLint m_uKa = 0;
    GLint m_uKd = 0;
    GLint m_uKs = 0;
    //GLint m_uADColor = 0;
    GLint m_uSpecularColor = 0;
    GLint m_uShininess = 0;

    QOpenGLShaderProgram* m_program = nullptr;
    int m_frame = 0;

    Model m_currentModel;

    QString currentVertFile;
    QString currentFragFile;

    // Mouse vars
    int lastX;
    int lastY;

protected:
    // Mouse functions
    void mousePressEvent(QMouseEvent*) override;
    void mouseMoveEvent(QMouseEvent*) override;
    void mouseReleaseEvent(QMouseEvent*) override;
    void wheelEvent(QWheelEvent*) override;
};
