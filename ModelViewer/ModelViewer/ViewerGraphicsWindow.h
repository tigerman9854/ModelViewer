#pragma once
#include "OpenGLWindow.h"
#include "ModelLoader.h"

#include <QOpenGLShaderProgram>

class ViewerGraphicsWindow : public OpenGLWindow
{
    Q_OBJECT

public:
    using OpenGLWindow::OpenGLWindow;

    ViewerGraphicsWindow(QWindow* parent = nullptr);

    void initialize() override;
    void render() override;
    void resetView();

    bool loadModel(QString filepath = QString());
    bool unloadModel();

    bool loadVertexShader(QString vertfilepath = QString());
    bool loadFragmentShader(QString fragfilepath = QString());

    bool addPrimitive(QString filepath);
    bool IsModelValid();

    bool editCurrentShaders();
    bool reloadCurrentShaders();
    bool openShaderFile(QString filepath = QString());

    QVector3D getLightLocation();
    void setLightLocation(float x, float y, float z);
    QVector3D getADS();
    void setADS(float a, float d, float s);
    QVector4D getADColor();
    void setADColor(float r, float g, float b);
    QVector4D getSpecularColor();
    void setSpecularColor(float r, float g, float b);
    float getShininess();
    void setShininess(float new_shininess);

    bool GetLeftMousePressed();
    bool GetRightMousePressed();
    QMatrix4x4 GetScaleMatrix();
    void SetScale(float scale);
    QMatrix4x4 GetRotationMatrix();
    QMatrix4x4 GetTranslationMatrix();
    QMatrix4x4 GetModelMatrix();

    // Mouse settings | % adjustment
    float panXSensitivity = .01f;
    float panYSensitivity = .01f;
    float xRotateSensitivity = 0.6f;
    float yRotateSensitivity = 0.6f;
    float zoomSensitivity = 0.001f;
    float fieldOfView = 45.f;
    float nearPlane = 0.1f;
    float farPlane = 100.f;

signals:
    void Error(QString message);
    void ClearError();
    void Initialized();
    void BeginModelLoading(QString filepath);
    void EndModelLoading(bool success, QString filepath);
    void ModelUnloaded();

protected:
    // Mouse functions
    virtual void mousePressEvent(QMouseEvent*) override;
    virtual void mouseMoveEvent(QMouseEvent*) override;
    virtual void mouseReleaseEvent(QMouseEvent*) override;
    virtual void wheelEvent(QWheelEvent*) override;

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

    QVector3D lightPos;
    QVector4D ADColor;
    QVector4D specularColor;
    float uKa;
    float uKd;
    float uKs;
    float shininess = 1.0;


    QOpenGLShaderProgram* m_program = nullptr;
    int m_frame = 0;

    Model m_currentModel;

    QString currentVertFile;
    QString currentFragFile;

    // Mouse vars
    int lastX;
    int lastY;
    bool m_leftMousePressed = false;
    bool m_rightMousePressed = false;

    QMatrix4x4 m_scaleMatrix;
    QMatrix4x4 m_rotMatrix;
    QMatrix4x4 m_transMatrix;
};
