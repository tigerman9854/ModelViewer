#pragma once
#include "OpenGLWindow.h"
#include "ModelLoader.h"
#include "SettingsMenu.h"

#include <QOpenGLShaderProgram>
#include <QElapsedTimer>

class ViewerGraphicsWindow : public OpenGLWindow
{
    Q_OBJECT

public:
    using OpenGLWindow::OpenGLWindow;

    ViewerGraphicsWindow(SettingsMenu* settings, QWindow* parent = nullptr);

    void initialize() override;
    void render() override;
    void resetView();

    bool loadModel(QString filepath = QString());
    bool unloadModel();

    bool loadVertexShader(QString vertfilepath = QString());
    bool loadFragmentShader(QString fragfilepath = QString());

    bool addPrimitive(QString filepath);
    void screenshotDialog();
    void saveDialog(QString filePath);
    void exportFrame(QString filePath);
    bool IsModelValid();

    bool editCurrentShaders();
    bool reloadCurrentShaders();
    bool openShaderFile(QString filepath = QString());

    bool GetLeftMousePressed();
    bool GetRightMousePressed();
    QMatrix4x4 GetScaleMatrix();
    void SetScale(float scale);
    QMatrix4x4 GetRotationMatrix();
    QMatrix4x4 GetTranslationMatrix();
    QMatrix4x4 GetModelMatrix();

    void ClearKeyboard();

    // Mouse settings | % adjustment
    float panXSensitivity;
    float panYSensitivity;
    float xRotateSensitivity;
    float yRotateSensitivity;
    float movementSensitivity;
    float zoomSensitivity;
    float fieldOfView;
    float nearPlane;
    float farPlane;

    void loadSettings();

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
    virtual void keyPressEvent(QKeyEvent*) override;
    virtual void keyReleaseEvent(QKeyEvent*) override;
    virtual void focusOutEvent(QFocusEvent*) override;

private:
    SettingsMenu* m_pSettingsMenu = nullptr;
    QSettings* settings = nullptr;

    // Modifies the matrices based on how much time has passed
    void Update(float sec);
    QElapsedTimer m_updateTimer;

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
    bool m_leftMousePressed = false;
    bool m_rightMousePressed = false;
    QSet<int> m_pressedKeys;

    QMatrix4x4 m_scaleMatrix;
    QMatrix4x4 m_rotMatrix;
    QMatrix4x4 m_transMatrix;
};
