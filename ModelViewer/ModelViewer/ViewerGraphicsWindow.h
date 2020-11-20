#pragma once
#include "OpenGLWindow.h"
#include "ModelLoader.h"
#include "SettingsMenu.h"

#include <QOpenGLWidget>
#include <QOpenGLShaderProgram>
#include <QElapsedTimer>
#include <QList>

class ViewerGraphicsWindow : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT

public:
    ViewerGraphicsWindow(QWidget* parent = nullptr);

    void resetView();

    bool loadModel(QString filepath = QString());
    bool unloadModel();
    bool addPrimitive(QString filepath);
    bool loadVertexShader(QString vertfilepath = QString());
    bool loadFragmentShader(QString fragfilepath = QString());

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
    float GetRotY();
    float GetRotX();
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

public slots:
    ////uniform slots
    ////color
    void colorRChanged(int val);
    void colorGChanged(int val);
    void colorBChanged(int val);
    void colorRChanged64(double val);
    void colorGChanged64(double val);
    void colorBChanged64(double val);
    ////settings
    void lightingSwitch(bool val);
    void smoothingSwitch(bool val);
    ////effect
    void effectType(int val);
    ////lighting
    void lightAmbient(float val);
    void lightDiffuse(float val);
    void lightSpecular(float val);

signals:
    void Error(QString message);
    void ClearError();
    void Initialized();
    void BeginModelLoading(QString filepath);
    void EndModelLoading(bool success, QString filepath);
    void ModelUnloaded();

protected:
    // Rendering functions
    virtual void initializeGL() override;
    virtual void resizeGL(int w, int h) override;
    virtual void paintGL() override;

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
    QSettings* settings = new QSettings("The Model Viewers team", "Model Viewer");

    // Modifies the matrices based on how much time has passed
    void Update(float sec);
    QElapsedTimer m_updateTimer;
    QList<float> m_frametimes;

    void RenderText();
    void RenderGrid(QMatrix4x4 mvp);
    void RenderAxes();
    float ComputeOptimalScale();

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

    GLint m_flatShaderPosAttr = 0;
    GLint m_flatShaderColAttr = 0;
    GLint m_flatShaderMatrixAttr = 0;

    QOpenGLShaderProgram* m_program = nullptr;
    QOpenGLShaderProgram* m_flatShader = nullptr;
    int m_frame = 0;

    Model m_currentModel;

    // Stores the scale for drawing the grid under the object
    float m_gridScale = 1.f;

    QString currentVertFile;
    QString currentFragFile;

    // Mouse vars
    int lastX;
    int lastY;
    float rotX;
    float rotY;
    bool m_leftMousePressed = false;
    bool m_rightMousePressed = false;
    QSet<QKeySequence> m_pressedKeys;
    QMatrix4x4 m_scaleMatrix;
    QMatrix4x4 m_transMatrix;
};
