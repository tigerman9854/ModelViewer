#include "ViewerGraphicsWindow.h"

#include "ModelLoader.h"

#include <QGuiApplication>
#include <QMatrix4x4>
#include <QOpenGLShaderProgram>
#include <QScreen>
#include <QtMath>
#include <QFileDialog>
#include <QMouseEvent>

// Define default shaders
static const char* vertexShaderSource =
    "attribute highp vec4 posAttr;\n"
    "attribute lowp vec4 colAttr;\n"
    "varying lowp vec4 col;\n"
    "uniform highp mat4 matrix;\n"
    "void main() {\n"
    "   col = colAttr;\n"
    "   gl_Position = matrix * posAttr;\n"
    "}\n";

static const char* fragmentShaderSource =
    "varying lowp vec4 col;\n"
    "void main() {\n"
    "   gl_FragColor = col;\n"
    "}\n";


ViewerGraphicsWindow::ViewerGraphicsWindow(QWindow* parent)
    : OpenGLWindow(parent)
{
    QSurfaceFormat format;
    format.setSamples(16);
    setFormat(format);

    setAnimating(true);
}

bool ViewerGraphicsWindow::loadModel(QString filepath) {
    if (!initialized) {
        return false;
    }

    // If no filepath was provided, open a file dialog for the user to choose a model
    if (filepath.isEmpty()) {
        filepath = QFileDialog::getOpenFileName(nullptr, "Load Model", "../Data/Models/", "");
        if (filepath.isEmpty()) {
            return false;
        }
    }

    // Load the model
    ModelLoader m;
    m_currentModel = m.LoadModel(filepath);
    return m_currentModel.m_isValid;
}

bool ViewerGraphicsWindow::loadVertexShader(QString vertfilepath)
{
    if (!initialized)
    {
        return false;
    }
    if (vertfilepath.isEmpty()) {
        vertfilepath = QFileDialog::getOpenFileName(nullptr, "Load Vertex Shader", "../Data/Shaders/", "");
        if (vertfilepath.isEmpty()) {
            return false;
        }
    }

    m_program->removeAllShaders();

    if (!m_program->addShaderFromSourceFile(QOpenGLShader::Vertex, vertfilepath)) 
    {
        m_program->addShaderFromSourceFile(QOpenGLShader::Vertex, currentVertFile);
        vertfilepath = currentVertFile;
    }
    m_program->addShaderFromSourceFile(QOpenGLShader::Fragment, currentFragFile);
    if (!m_program->link())
    {
        QString error = m_program->log();
        qDebug() << error << endl;
        currentVertFile = vertfilepath;
        return false;
    }

    currentVertFile = vertfilepath;
    m_posAttr = m_program->attributeLocation("posAttr");
    Q_ASSERT(m_posAttr != -1);
    m_colAttr = m_program->attributeLocation("colAttr");
    Q_ASSERT(m_colAttr != -1);
    m_matrixUniform = m_program->uniformLocation("matrix");
    Q_ASSERT(m_matrixUniform != -1);

    m_normAttr = m_program->attributeLocation("normAttr");
    m_uvAttr = m_program->attributeLocation("uvAttr");

    m_modelviewUniform = m_program->uniformLocation("modelview");
    m_normalUniform = m_program->uniformLocation("normalMat");

    m_lightPosUniform = m_program->uniformLocation("uLightPos");
    m_uKa = m_program->uniformLocation("uKa");
    m_uKd = m_program->uniformLocation("uKd");
    m_uKs = m_program->uniformLocation("uKs");
    m_uSpecularColor = m_program->uniformLocation("uSpecularColor");
    m_uShininess = m_program->uniformLocation("uShininess");
    
    return true;
}

bool ViewerGraphicsWindow::loadFragmentShader(QString fragfilepath)
{
    if (!initialized)
    {
        return false;
    }
    
    if (fragfilepath.isEmpty()) {
        fragfilepath = QFileDialog::getOpenFileName(nullptr, "Load Fragment Shader", "../Data/Shaders/", "");
        if (fragfilepath.isEmpty()) {
            return false;
        }
    }

    m_program->removeAllShaders();

    m_program->addShaderFromSourceFile(QOpenGLShader::Vertex, currentVertFile);
    if (!m_program->addShaderFromSourceFile(QOpenGLShader::Fragment, fragfilepath)) 
    {
        m_program->addShaderFromSourceFile(QOpenGLShader::Fragment, currentFragFile);
        fragfilepath = currentFragFile;
    }
    if (!m_program->link())
    {
        QString error = m_program->log();
        qDebug() << error << endl;

        currentFragFile = fragfilepath;

        return false;
    }
    
    currentFragFile = fragfilepath;
    m_posAttr = m_program->attributeLocation("posAttr");
    Q_ASSERT(m_posAttr != -1);
    m_colAttr = m_program->attributeLocation("colAttr");
    Q_ASSERT(m_colAttr != -1);
    m_matrixUniform = m_program->uniformLocation("matrix");
    Q_ASSERT(m_matrixUniform != -1);

    m_normAttr = m_program->attributeLocation("normAttr");
    m_uvAttr = m_program->attributeLocation("uvAttr");

    m_modelviewUniform = m_program->uniformLocation("modelview");
    m_normalUniform = m_program->uniformLocation("normalMat");

    m_lightPosUniform = m_program->uniformLocation("uLightPos");
    m_uKa = m_program->uniformLocation("uKa");
    m_uKd = m_program->uniformLocation("uKd");
    m_uKs = m_program->uniformLocation("uKs");
    m_uSpecularColor = m_program->uniformLocation("uSpecularColor");
    m_uShininess = m_program->uniformLocation("uShininess");

    return true;
}

void ViewerGraphicsWindow::mousePressEvent(QMouseEvent* event)
{
    // Set class vars
    if (event->button() == Qt::LeftButton) {
        leftMousePressed = true;
    }

    if (event->button() == Qt::RightButton) {
        rightMousePressed = true;
    }

    // Make sure that these are set before the mouseMoveEvent triggers
    lastX = event->x();
    lastY = event->y();
    // Call the parent class 
    QWindow::mouseReleaseEvent(event);
}

void ViewerGraphicsWindow::mouseReleaseEvent(QMouseEvent* event)
{
    // Set class vars
    if (event->button() == Qt::LeftButton) {
        leftMousePressed = false;
    }

    if (event->button() == Qt::RightButton) {
        rightMousePressed = false;
    }
    // Call the parent class
    QWindow::mouseReleaseEvent(event);
}

void ViewerGraphicsWindow::mouseMoveEvent(QMouseEvent* event)
{
    float deltaX = lastX - event->x();
    float deltaY = lastY - event->y();

    // RMB: Rotate off of x y movement
    if (event->buttons() & Qt::RightButton) {
        // TODO: This could be better. If we keep track of the normal of the model
        // we could make sure that translateing in x & y won't pitch the object.
        sceneMatrix.rotate(-deltaX * xRotateSensitivity, 0, 1, 0);
        sceneMatrix.rotate(-deltaY * yRotateSensitivity, 1, 0, 0);
        modelview.rotate(-deltaX * xRotateSensitivity, 0, 1, 0);
        modelview.rotate(-deltaY * yRotateSensitivity, 1, 0, 0);
    }

    // MMB: Pan off of x y movement
    if (event->buttons() & Qt::LeftButton) {
        viewportX += -deltaX * viewportXSensitivity;
        viewportY += deltaY * viewportYSensitivity;
    }

    // After moving update the lastX/Y
    lastX = event->x();
    lastY = event->y();
    // Call the parent class 
    QWindow::mouseMoveEvent(event);
}

void ViewerGraphicsWindow::wheelEvent(QWheelEvent* event)
{
    if ((event->angleDelta().y()) > 0) {
        sceneMatrix.scale(0.5f * zoomSensitivity);
    }
    else {
        sceneMatrix.scale(2 * zoomSensitivity);
    }
}

void ViewerGraphicsWindow::initialize()
{
    m_program = new QOpenGLShaderProgram(this);
    currentVertFile = "../Data/Shaders/basic.vert";
    currentFragFile = "../Data/Shaders/basic.frag";
    m_program->addShaderFromSourceFile(QOpenGLShader::Vertex, currentVertFile);
    m_program->addShaderFromSourceFile(QOpenGLShader::Fragment, currentFragFile);
    m_program->link();
    m_posAttr = m_program->attributeLocation("posAttr");
    Q_ASSERT(m_posAttr != -1);
    m_colAttr = m_program->attributeLocation("colAttr");
    Q_ASSERT(m_colAttr != -1);
    m_matrixUniform = m_program->uniformLocation("matrix");
    Q_ASSERT(m_matrixUniform != -1);

    // Set up the default view
    resetView();

    // TODO: Set attribute locations for m_normAttr and m_uvAttr once our shader supports these

    m_normAttr = m_program->attributeLocation("normAttr");
    m_uvAttr = m_program->attributeLocation("uvAttr");

    m_modelviewUniform = m_program->uniformLocation("modelview");
    m_normalUniform = m_program->uniformLocation("normalMat");

    m_lightPosUniform = m_program->uniformLocation("uLightPos");
    m_uKa = m_program->uniformLocation("uKa");
    m_uKd = m_program->uniformLocation("uKd");
    m_uKs = m_program->uniformLocation("uKs");
    //m_uADColor = m_program->uniformLocation("uADColor");
    m_uSpecularColor = m_program->uniformLocation("uSpecularColor");
    m_uShininess = m_program->uniformLocation("uShininess");

    //m_program->bind();

    initialized = true;
}

void ViewerGraphicsWindow::render()
{
    const qreal retinaScale = devicePixelRatio();
    glViewport(viewportX, viewportY, width() * retinaScale, height() * retinaScale);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_program->bind();

    m_program->setUniformValue(m_matrixUniform, sceneMatrix);

    m_program->setUniformValue(m_modelviewUniform, modelview);

    QMatrix3x3 normal = modelview.normalMatrix();
    m_program->setUniformValue(m_normalUniform, normal);

    QVector3D lightPos = QVector3D(1., 1., -1.);
    m_program->setUniformValue(m_lightPosUniform, lightPos);

    float uKa = 0.35;
    m_program->setUniformValue(m_uKa, uKa);
    float uKd = 0.45;
    m_program->setUniformValue(m_uKd, uKd);
    float uKs = 0.15;
    m_program->setUniformValue(m_uKs, uKs);

    //QVector4D uADColor = QVector4D(1., 1., 1., 1.);
    //m_program->setUniformValue(m_uADColor, uADColor);

    QVector4D uSpecularColor = QVector4D(1., 1., 1., 1.);
    m_program->setUniformValue(m_uSpecularColor, uSpecularColor);

    GLfloat uShininess = 0.3;
    m_program->setUniformValue(m_uShininess, uShininess);

    glEnable(GL_DEPTH_TEST);

    if (m_currentModel.m_isValid)
    {
        // Loop through all meshes in the current model
        for (Mesh& mesh : m_currentModel.m_meshes) {
            mesh.m_vertexBuffer.bind();
            mesh.m_indexBuffer.bind();

            // Positions
            glVertexAttribPointer(m_posAttr, mesh.m_numPositionComponents, GL_FLOAT, GL_FALSE, 0, (void*)mesh.m_positionOffset);
            glEnableVertexAttribArray(m_posAttr);

            // Normals
            if (mesh.m_hasNormals) {
                glVertexAttribPointer(m_normAttr, mesh.m_numNormalComponents, GL_FLOAT, GL_FALSE, 0, (void*)mesh.m_normalOffset);
                glEnableVertexAttribArray(m_normAttr);
            }

            // UV Coords
            if (mesh.m_hasUVCoordinates) {
                glVertexAttribPointer(m_uvAttr, mesh.m_numUVComponents, GL_FLOAT, GL_FALSE, 0, (void*)mesh.m_uvOffset);
                glEnableVertexAttribArray(m_uvAttr);
            }

            // Colors
            if (mesh.m_hasColors) {
                glVertexAttribPointer(m_colAttr, mesh.m_numColorComponents, GL_FLOAT, GL_FALSE, 0, (void*)mesh.m_colorOffset);
                glEnableVertexAttribArray(m_colAttr);
            }

            glDrawElements(GL_TRIANGLES, mesh.m_indexCount, GL_UNSIGNED_INT, nullptr);

            // Disable all attributes
            if (mesh.m_hasColors) {
                glDisableVertexAttribArray(m_colAttr);
            }
            if (mesh.m_hasNormals) {
                glDisableVertexAttribArray(m_normAttr);
            }
            if (mesh.m_hasUVCoordinates) {
                glDisableVertexAttribArray(m_uvAttr);
            }

            glDisableVertexAttribArray(m_posAttr);
        }
    }
    else
    {
        // Default triangle
        static const GLfloat vertices[] = {
             0.0f,  0.707f,
            -0.5f, -0.5f,
             0.5f, -0.5f
        };

        static const GLfloat colors[] = {
            1.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 1.0f
        };

        glVertexAttribPointer(m_posAttr, 2, GL_FLOAT, GL_FALSE, 0, vertices);
        glVertexAttribPointer(m_colAttr, 3, GL_FLOAT, GL_FALSE, 0, colors);

        glEnableVertexAttribArray(m_posAttr);
        glEnableVertexAttribArray(m_colAttr);

        glDrawArrays(GL_TRIANGLES, 0, 3);

        glDisableVertexAttribArray(m_colAttr);
        glDisableVertexAttribArray(m_posAttr);
    }

    m_program->release();

    ++m_frame;
}

void ViewerGraphicsWindow::resetView()
{
    // Reset the sceneMatrix
    sceneMatrix.setToIdentity();
    sceneMatrix.perspective(60.0f, 4.0f / 3.0f, 0.1f, 100.0f);
    sceneMatrix.translate(0, 0, -4);

    modelview.setToIdentity();
    modelview.translate(0, 0, -4);

    // Rest the mouse variables
    viewportX = 0;
    viewportY = 0;
}

bool ViewerGraphicsWindow::addPrimitive(QString primitiveName) {
    // Load  model
    QString filepath = QString("../Data/Primitives/%1").arg(primitiveName);
    return loadModel(filepath);
}
