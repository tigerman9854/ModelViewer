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
        QVector3D xAxis(1, 0, 0);
        QVector3D yAxis(0, 1, 0);
        
        QMatrix4x4 newRot;
        newRot.rotate(-deltaX * xRotateSensitivity, yAxis);
        newRot.rotate(-deltaY * yRotateSensitivity, xAxis);

        // Perform the new rotation AFTER the previous rotations
        rotMatrix = newRot * rotMatrix;
    }

    // MMB: Pan off of x y movement
    if (event->buttons() & Qt::LeftButton) {
        // Adjust pan sensitivity based on the size of the window
        const float panAdj = 480.f / (float)height();

        transMatrix.translate(-deltaX * panXSensitivity * panAdj, 0, 0);
        transMatrix.translate(0, deltaY * panYSensitivity * panAdj, 0);
    }

    // After moving update the lastX/Y
    lastX = event->x();
    lastY = event->y();

    // Call the parent class 
    QWindow::mouseMoveEvent(event);
}

void ViewerGraphicsWindow::wheelEvent(QWheelEvent* event)
{
    const float zoomAmount = zoomSensitivity * event->angleDelta().y();
    scaleMatrix.scale(1.f + zoomAmount);
}

void ViewerGraphicsWindow::initialize()
{
    m_program = new QOpenGLShaderProgram(this);
    m_program->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSource);
    m_program->addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSource);
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

    initialized = true;
}

void ViewerGraphicsWindow::render()
{
    const qreal retinaScale = devicePixelRatio();
    glViewport(0, 0, width() * retinaScale, height() * retinaScale);

    QMatrix4x4 viewMatrix;
    viewMatrix.perspective(fieldOfView, float(width() * retinaScale) / float(height() * retinaScale), nearPlane, farPlane);

    QMatrix4x4 modelMatrix;
    modelMatrix = transMatrix * rotMatrix * scaleMatrix;

    QMatrix4x4 modelViewProjectionMatrix;
    modelViewProjectionMatrix = viewMatrix * modelMatrix;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_program->bind();

    m_program->setUniformValue(m_matrixUniform, modelViewProjectionMatrix);

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
    // Reset matrices to default values
    scaleMatrix = QMatrix4x4();
    rotMatrix = QMatrix4x4();
    transMatrix = QMatrix4x4();
    transMatrix.translate(0, 0, -4);
}

bool ViewerGraphicsWindow::addPrimitive(QString primitiveName) {
    // Load  model
    QString filepath = QString("../Data/Primitives/%1").arg(primitiveName);
    return loadModel(filepath);
}
