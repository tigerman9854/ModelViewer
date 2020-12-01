#include "ViewerGraphicsWindow.h"
#include "SettingsMenu.h"
#include "ModelLoader.h"
#include "KeySequenceParse.h"
#include "Axes.h"

#include <QGuiApplication>
#include <QMatrix4x4>
#include <QOpenGLShaderProgram>
#include <QScreen>
#include <QtMath>
#include <QFileDialog>
#include <QFileInfo>
#include <QMouseEvent>
#include <QDesktopServices>
#include <QUrl>
#include <QtMath>
#include <QKeyEvent>
#include <QImage>
#include <QPainter>
#include <QColor>
#include <QFont>
#include <QFontMetrics>
#include <QTimer>

ViewerGraphicsWindow::ViewerGraphicsWindow(QWidget* parent)
    : QOpenGLWidget(parent)
{
    QSurfaceFormat format;
    format.setSamples(16);
    setFormat(format);

    resetView();

    loadSettings();

    // Call update every millisecond to force repainting
    // Calling update() several times normally results in just one paintGL() call
    // This results in locking the fps to the monitor refresh rate
    QTimer* updateTrigger = new QTimer(this);
    updateTrigger->start();
    connect(updateTrigger, &QTimer::timeout, this, [=] {
        update();
        updateTrigger->start(1);
    });
}

void ViewerGraphicsWindow::loadSettings() {
    panXSensitivity = settings->value("ViewerGraphicsWindow/panXSensitivity", .01f).toFloat();
    panYSensitivity = settings->value("ViewerGraphicsWindow/panYSensitivity", .01f).toFloat();
    xRotateSensitivity = settings->value("ViewerGraphicsWindow/xRotateSensitivity", 0.6f).toFloat();
    yRotateSensitivity = settings->value("ViewerGraphicsWindow/yRotateSensitivity", 0.6f).toFloat();
    movementSensitivity = settings->value("ViewerGraphicsWindow/movementSensitivity", 4.0f).toFloat();
    zoomSensitivity = settings->value("ViewerGraphicsWindow/zoomSensitivity", 0.001f).toFloat();
    fieldOfView = settings->value("ViewerGraphicsWindow/fieldOfView", 45.f).toFloat();
    nearPlane = settings->value("ViewerGraphicsWindow/nearPlane", 0.1f).toFloat();
    farPlane = settings->value("ViewerGraphicsWindow/farPlane", 100.f).toFloat();
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

    // Let other widgets know that we are beginning a load operation (may take some time)
    emit BeginModelLoading(filepath);

    // Load the model
    ModelLoader m;
    m_currentModel = m.LoadModel(filepath);
    
    // Let other widgets know that a model has been loaded
    emit EndModelLoading(m_currentModel.m_isValid, filepath);

    // Reset the view to size properly for the new model
    resetView();

    return m_currentModel.m_isValid;
}

bool ViewerGraphicsWindow::unloadModel()
{
    m_currentModel = Model();
    emit ModelUnloaded();

    return true;
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
        emit Error("Failed to link shader program.");

        currentVertFile = vertfilepath;
        return false;
    }

    currentVertFile = vertfilepath;
    setUniformLocations();
    
    emit ClearError();
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

        emit Error("Failed to link shader program.");

        return false;
    }
    
    currentFragFile = fragfilepath;
    setUniformLocations();

    emit ClearError();
    return true;
}

bool ViewerGraphicsWindow::editCurrentShaders()
{
    QDesktopServices desk;
    QDir dir;
    QString currentPath = dir.currentPath();
    //qDebug() << currentPath << endl;

    QString fullVertPath = "//" + currentPath + "/" + currentVertFile;
    QString fullFragPath = "//" + currentPath + "/" + currentFragFile;
    //qDebug() << fullVertPath << endl;
    
    if (!desk.openUrl(QUrl::fromLocalFile(fullVertPath)))
    {
        return false;
    }
    if(!desk.openUrl(QUrl::fromLocalFile(fullFragPath)))
    {
        return false;
    }
    return true;
}
void ViewerGraphicsWindow::setUniformVars() {
    if (m_lightPosUniform != -1)
    {
        m_program->setUniformValue(m_lightPosUniform, lightPos);
    }
    if (m_uKa != -1)
    {
        m_program->setUniformValue(m_uKa, uKa);
    }
    if (m_uKd != -1)
    {
        m_program->setUniformValue(m_uKd, uKd);
    }
    if (m_uKs != -1)
    {
        m_program->setUniformValue(m_uKs, uKs);
    }
    if (m_uSpecularColor != -1)
    {
        m_program->setUniformValue(m_uSpecularColor, specularColor);
    }
    if (m_uShininess != -1)
    {
        m_program->setUniformValue(m_uShininess, shininess);
    }
    if (m_colAttr != -1)
    {
        m_program->setAttributeValue(m_colAttr, ADColor);
    }
    if (m_uMat4_1 != -1)
    {
        m_program->setUniformValue(m_uMat4_1, uMat4_1);
    }
    if (m_uVec3_1 != -1)
    {
        m_program->setUniformValue(m_uVec3_1, uVec3_1);
    }
    if (m_uVec4_1 != -1)
    {
        m_program->setUniformValue(m_uVec4_1, uVec4_1);
    }
    if (m_uFloat_1 != -1)
    {
        m_program->setUniformValue(m_uFloat_1, uFloat_1);
    }
    if (m_uInt_1 != -1)
    {
        m_program->setUniformValue(m_uInt_1, uInt_1);
    }
}

void ViewerGraphicsWindow::setUniformLocations()
{
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

    m_uMat4_1 = m_program->uniformLocation("uMat4_1");
    m_uVec3_1 = m_program->uniformLocation("uVec3_1");
    m_uVec4_1 = m_program->uniformLocation("uVec4_1");
    m_uFloat_1 = m_program->uniformLocation("uFloat_1");
    m_uInt_1 = m_program->uniformLocation("uInt_1");
}

bool ViewerGraphicsWindow::reloadCurrentShaders()
{
    if (loadVertexShader(currentVertFile) &&
        loadFragmentShader(currentFragFile)) {
        return true;
    }
    return false;
}

bool ViewerGraphicsWindow::openShaderFile(QString filepath)
{
    if (filepath.isEmpty()) {
        filepath = QFileDialog::getOpenFileName(nullptr, "Open Shader File", "../Data/Shaders/", "");
        if (filepath.isEmpty()) {
            return false;
        }
    }

    QDesktopServices desk;
    QDir dir;
    QString currentPath = dir.currentPath();
    QString fullPath = "//" + currentPath + "/" + filepath;
    if (!desk.openUrl(QUrl::fromLocalFile(filepath)))
    {
        return false;
    }
    return true;
}

void ViewerGraphicsWindow::mousePressEvent(QMouseEvent* event)
{
    // Focus this object
    setFocus();

    // Set class vars
    if (event->button() == Qt::LeftButton) {
        m_leftMousePressed = true;
    }

    if (event->button() == Qt::RightButton) {
        m_rightMousePressed = true;
    }

    // Make sure that these are set before the mouseMoveEvent triggers
    lastX = event->x();
    lastY = event->y();

    // Call the parent class 
    QOpenGLWidget::mousePressEvent(event);
}

void ViewerGraphicsWindow::mouseReleaseEvent(QMouseEvent* event)
{
    // Set class vars
    if (event->button() == Qt::LeftButton) {
        m_leftMousePressed = false;
    }

    if (event->button() == Qt::RightButton) {
        m_rightMousePressed = false;
    }

    // Call the parent class
    QOpenGLWidget::mouseReleaseEvent(event);
}

void ViewerGraphicsWindow::mouseMoveEvent(QMouseEvent* event)
{
    float deltaX = lastX - event->x();
    float deltaY = lastY - event->y();

    // RMB: Rotate off of x-y movement
    if (event->buttons() & Qt::LeftButton && m_leftMousePressed) {
        rotX += -deltaX * xRotateSensitivity;
        rotY += -deltaY * yRotateSensitivity;

        rotY = std::max(-90.f, std::min(rotY, 90.f));
    }

    // MMB: Pan off of x y movement
    if (event->buttons() & Qt::RightButton && m_rightMousePressed) {
        // Adjust pan sensitivity based on the size of the window and FOV
        const float panAdj = (480.f / (float)height()) * (fieldOfView / 60.f);

        m_transMatrix.translate(-deltaX * panXSensitivity * panAdj, 0, 0);
        m_transMatrix.translate(0, deltaY * panYSensitivity * panAdj, 0);
    }

    // After moving update the lastX/Y
    lastX = event->x();
    lastY = event->y();

    // Call the parent class 
    QOpenGLWidget::mouseMoveEvent(event);
}

void ViewerGraphicsWindow::keyPressEvent(QKeyEvent* event)
{
    // Store that this key is pressed
    m_pressedKeys.insert(event->key());

    QOpenGLWidget::keyPressEvent(event);
}
void ViewerGraphicsWindow::keyReleaseEvent(QKeyEvent* event)
{
    // The key has been released
    m_pressedKeys.remove(event->key());

    QOpenGLWidget::keyReleaseEvent(event);
}

void ViewerGraphicsWindow::focusOutEvent(QFocusEvent* event)
{
    ClearKeyboard();

    QOpenGLWidget::focusOutEvent(event);
}

void ViewerGraphicsWindow::ClearKeyboard()
{
    // Clear all pressed keys when the window loses focus
    m_pressedKeys.clear();
}

void ViewerGraphicsWindow::wheelEvent(QWheelEvent* event)
{
    const float zoomAmount = zoomSensitivity * event->angleDelta().y();
    m_scaleMatrix.scale(1.f + zoomAmount);
}

void ViewerGraphicsWindow::initializeGL()
{
    initializeOpenGLFunctions();

    m_program = new QOpenGLShaderProgram(this);
    currentVertFile = "../Data/Shaders/ads.vert";
    currentFragFile = "../Data/Shaders/ads.frag";
    m_program->addShaderFromSourceFile(QOpenGLShader::Vertex, currentVertFile);
    m_program->addShaderFromSourceFile(QOpenGLShader::Fragment, currentFragFile);
    m_program->link();

    setUniformLocations();
    
    // Load the flat shader
    m_flatShader = new QOpenGLShaderProgram(this);
    m_flatShader->addShaderFromSourceCode(QOpenGLShader::Vertex, flatVertexShaderSource);
    m_flatShader->addShaderFromSourceCode(QOpenGLShader::Fragment, flatFragmentShaderSource);
    m_flatShader->link();
    m_flatShaderPosAttr = m_flatShader->attributeLocation("posAttr");
    Q_ASSERT(m_flatShaderPosAttr != -1);
    m_flatShaderColAttr = m_flatShader->attributeLocation("colAttr");
    Q_ASSERT(m_flatShaderColAttr != -1);
    m_flatShaderMatrixAttr = m_flatShader->uniformLocation("matrix");
    Q_ASSERT(m_flatShaderMatrixAttr != -1);

    // Set up the default view
    resetView();

    lightPos = QVector3D(1., 1., -1.);
    uKa = 0.30;
    uKd = 0.40;
    uKs = 0.35;
    ADColor = QVector4D(0., 1., 0., 1.);
    specularColor = QVector4D(1., 1., 1., 1.);
    shininess = 1.0;

    uMat4_1 = QMatrix4x4();
    uVec3_1 = QVector3D(0.5, 0.5, 0.);
    uVec4_1 = QVector4D(1., 1., 1., 1.);
    uFloat_1 = 0.;
    uInt_1 = 0;

    emit Initialized();

    initialized = true;
}

void ViewerGraphicsWindow::resizeGL(int w, int h) {
    // Compute viewport with support for high DPI monitors
    const qreal retinaScale = devicePixelRatio();
    glViewport(0, 0,w * retinaScale, h * retinaScale);
}

void ViewerGraphicsWindow::paintGL()
{
    // Determine how much time has passed since the last update,
    // call update, and reset the timer
    const qint64 nsec = m_updateTimer.nsecsElapsed();
    m_updateTimer.restart();
    const float seconds = (float)nsec * 1e-9f;
    m_frametimes.push_back(seconds);
    Update(seconds);

    // Compute viewport with support for high DPI monitors
    const qreal retinaScale = devicePixelRatio();
    glViewport(0, 0, width() * retinaScale, height() * retinaScale);

    QMatrix4x4 viewMatrix;
    viewMatrix.perspective(fieldOfView, float(width() * retinaScale) / float(height() * retinaScale), nearPlane, farPlane);

    QMatrix4x4 modelMatrix = GetModelMatrix();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_program->bind();

    setUniformVars();

    glEnable(GL_DEPTH_TEST);

    if (m_currentModel.m_isValid)
    {
        // Loop through all meshes in the current model
        for (Mesh& mesh : m_currentModel.m_meshes) {
            mesh.m_vertexBuffer.bind();
            mesh.m_indexBuffer.bind();
            
            // Handle transformation for each mesh
            QMatrix4x4 modelTransformed = modelMatrix * mesh.m_transform;
            QMatrix4x4 modelViewProjectionMatrix;
            modelViewProjectionMatrix = viewMatrix * modelTransformed;

            m_program->setUniformValue(m_matrixUniform, modelViewProjectionMatrix);

            m_program->setUniformValue(m_modelviewUniform, modelTransformed);

            QMatrix3x3 normal = modelTransformed.normalMatrix();
            m_program->setUniformValue(m_normalUniform, normal);

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

            mesh.m_vertexBuffer.release();
            mesh.m_indexBuffer.release();
        }
    }
    m_program->release();

    // Draw a grid for the object
    RenderGrid(viewMatrix * modelMatrix);

    // Draw axes so the user understands direction
    RenderAxes();

    // Draw the framerate counter and size of this mesh
    RenderText();

    // Increase the frame counter by one
    ++m_frame;
}

void ViewerGraphicsWindow::RenderGrid(QMatrix4x4 mvp)
{
    // Bind the flat shader for drawing gridlines
    m_flatShader->bind();

    // Bind the grid attributes and colors
    glVertexAttribPointer(m_flatShaderPosAttr, 3, GL_FLOAT, GL_FALSE, 0, grid);
    glVertexAttribPointer(m_flatShaderColAttr, 4, GL_FLOAT, GL_FALSE, 0, gridColors);

    glEnableVertexAttribArray(m_flatShaderPosAttr);
    glEnableVertexAttribArray(m_flatShaderColAttr);

    // Enable alpha blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Move the model so it's sitting on the grid
    mvp.translate(0, m_currentModel.m_AABBMin.y(), 0);
    m_flatShader->setUniformValue(m_flatShaderMatrixAttr, mvp);

    // Helper function to draw grids
    auto drawGrid = [&](float scale, QMatrix4x4 modelViewProj, float offset) {
        // Scale the drawing
        modelViewProj.scale(scale);
        modelViewProj.translate(offset, 0, offset);

        // Draw the grid
        m_flatShader->setUniformValue(m_flatShaderMatrixAttr, modelViewProj);
        glDrawArrays(GL_LINES, 0, sizeof(grid) / 3 / sizeof(float));

        // Rotate 90 degrees for the next iteration
        modelViewProj.rotate(90.f, 0, 1, 0);
        m_flatShader->setUniformValue(m_flatShaderMatrixAttr, modelViewProj);
        glDrawArrays(GL_LINES, 0, sizeof(grid) / 3 / sizeof(float));
    };

    glLineWidth(1.f);
    drawGrid(m_gridScale, mvp, 0);

    // Disable all the stuff we enabled
    glDisable(GL_BLEND);

    glDisableVertexAttribArray(m_flatShaderColAttr);
    glDisableVertexAttribArray(m_flatShaderPosAttr);

    m_flatShader->release();
}

void ViewerGraphicsWindow::RenderAxes()
{
    // Viewport to the bottom right corner
    const qreal retinaScale = devicePixelRatio();
    const float size = width() * retinaScale * 0.1f;
    const float padding = 4.f * retinaScale;
    glViewport(width() * retinaScale - size - padding, padding, size, size);

    // Orithographic projection
    QMatrix4x4 viewMatrix;
    viewMatrix.ortho(-1, 1, -1, 1, 0.01, 5);

    // Rotate the axes
    QMatrix4x4 modelMatrix;
    modelMatrix.translate(0, 0, -1);
    modelMatrix.rotate(rotY, 1, 0, 0);
    modelMatrix.rotate(rotX, 0, 1, 0);

    // Bind the flat shader for drawing axes
    m_flatShader->bind();

    m_flatShader->setUniformValue(m_flatShaderMatrixAttr, viewMatrix * modelMatrix);

    // Bind the grid attributes and colors
    glVertexAttribPointer(m_flatShaderPosAttr, 3, GL_FLOAT, GL_FALSE, 0, axes);
    glVertexAttribPointer(m_flatShaderColAttr, 3, GL_FLOAT, GL_FALSE, 0, axesColors);

    glEnableVertexAttribArray(m_flatShaderPosAttr);
    glEnableVertexAttribArray(m_flatShaderColAttr);

    glLineWidth(2.f);
    glDrawArrays(GL_LINES, 0, sizeof(axes) / 3 / sizeof(float));

    // Disable all the stuff we enabled
    glDisableVertexAttribArray(m_flatShaderColAttr);
    glDisableVertexAttribArray(m_flatShaderPosAttr);

    m_flatShader->release();
}

void ViewerGraphicsWindow::RenderText()
{
    // Prepare to draw text on the screen
    QPainter painter(this);
    QColor col(165, 165, 165, 200);
    QFontMetrics f(painter.font());
    painter.setPen(col);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);

    // Compute framerate
    float total = 0;
    for (auto it : m_frametimes) {
        total += it;
    }
    const int fps = round(m_frametimes.size() / total);

    // Remove any frametimes older than 0.25 seconds
    total = 0;
    QList<float> newFrametimes;
    for (auto it = m_frametimes.rbegin(); it != m_frametimes.rend(); ++it) {
        if (total < 0.25f) {
            total += *it;
            newFrametimes.push_front(*it);
        }
        else {
            break;
        }
    }
    m_frametimes = newFrametimes;

    // Compute number of polygons, assume triangles
    int polyCount = 0;
    if (m_currentModel.m_isValid) {
        for (auto it : m_currentModel.m_meshes) {
            polyCount += (it.m_indexCount / 3);
        }
    }

    // Format the text for drawing
    const QString framerateText = QString("FPS: %1").arg(fps);
    const QString polygonText = QString("Polys: %1").arg(polyCount);
    QString sizeText;
    if (m_currentModel.m_isValid) {
        QVector3D max = m_currentModel.m_AABBMax;
        QVector3D min = m_currentModel.m_AABBMin;
        // Round to 2 decimal
        sizeText = QString("Model Size\nX: %1\nY: %2\nZ: %3")
            .arg(max.x() - min.x(), 0, 'f', 2)
            .arg(max.y() - min.y(), 0, 'f', 2)
            .arg(max.z() - min.z(), 0, 'f', 2);
    }
    const QString gridText = QString("Grid: %1x%1").arg(m_gridScale);

    const QString topLeft = framerateText + "\n" + polygonText;
    const QString bottomLeft = gridText + "\n" + sizeText;

    // Draw the text
    const int padding = 4;
    painter.drawText(padding, padding, 500, 500, Qt::AlignTop, topLeft);
    painter.drawText(padding, height() - (f.lineSpacing() * 5) - padding, 500, 500, Qt::AlignTop, bottomLeft);
}

void ViewerGraphicsWindow::Update(float sec)
{
    // Allow shift and ctrl to increase/decrease speed
    float effectiveSpeed = movementSensitivity * sec;
  
    if (m_pressedKeys.contains(KeySequenceParse(settings->value("ViewerGraphicsWindow/increase_speed", "Shift").toString()).get())) {
        effectiveSpeed *= 3.f;
    }
    if (m_pressedKeys.contains(KeySequenceParse(settings->value("ViewerGraphicsWindow/decrease_speed", "Ctrl").toString()).get())) {
        effectiveSpeed /= 3.f;
    }

    // W/S to elevate
    if (m_pressedKeys.contains(KeySequenceParse(settings->value("ViewerGraphicsWindow/elevate_forwards", QString(Qt::Key::Key_W)).toString()).get())) {
        m_transMatrix.translate(0, -effectiveSpeed, 0);
    }
    if (m_pressedKeys.contains(KeySequenceParse(settings->value("ViewerGraphicsWindow/elevate_backwards", QString(Qt::Key::Key_S)).toString()).get())) {
        m_transMatrix.translate(0, effectiveSpeed, 0);
    }

    // A/D to strafe
    if (m_pressedKeys.contains(KeySequenceParse(settings->value("ViewerGraphicsWindow/strafe_left", QString(Qt::Key::Key_A)).toString()).get())) {
        m_transMatrix.translate(effectiveSpeed, 0, 0);
    }
    if (m_pressedKeys.contains(KeySequenceParse(settings->value("ViewerGraphicsWindow/strafe_right", QString(Qt::Key::Key_D)).toString()).get())) {
        m_transMatrix.translate(-effectiveSpeed, 0, 0);
    }

    // Implement Q and E as scale instead of translate so the user cannot
    // move behind the object
    if (m_pressedKeys.contains(KeySequenceParse(settings->value("ViewerGraphicsWindow/scale_up", QString(Qt::Key::Key_E)).toString()).get())) {
        m_scaleMatrix.scale(1 + (effectiveSpeed / 2.f));
    }
    if (m_pressedKeys.contains(KeySequenceParse(settings->value("ViewerGraphicsWindow/scale_down", QString(Qt::Key::Key_Q)).toString()).get())) {
        m_scaleMatrix.scale(1 - (effectiveSpeed / 2.f));
    }


    // Up and down arrows to pitch
    float rotSpeed = qRadiansToDegrees(effectiveSpeed);
    if (m_pressedKeys.contains(KeySequenceParse(settings->value("ViewerGraphicsWindow/pitch_up", "Up").toString()).get())) {
        rotY += rotSpeed;
    }
    if (m_pressedKeys.contains(KeySequenceParse(settings->value("ViewerGraphicsWindow/pitch_down", "Down").toString()).get())) {
        rotY += -rotSpeed;
    }

    // Clamp
    rotY = std::max(-90.f, std::min(rotY, 90.f));

    // Left and right to spin
    if (m_pressedKeys.contains(KeySequenceParse(settings->value("ViewerGraphicsWindow/spin_right", "Right").toString()).get())) {
        rotX += -rotSpeed;
    }
    if (m_pressedKeys.contains(KeySequenceParse(settings->value("ViewerGraphicsWindow/spin_left", "Left").toString()).get())) {
        rotX += rotSpeed;
    }
}

void ViewerGraphicsWindow::resetView()
{
    // Reset matrices to default values
    m_scaleMatrix = QMatrix4x4();
    rotY = 30.f;
    rotX = 0;
    m_transMatrix = QMatrix4x4();
    m_transMatrix.translate(0, 0, -4);

    const float optimalScale = ComputeOptimalScale();
    m_scaleMatrix.scale(optimalScale);

    // Compute the scale for the grid under the object
    // Use the optimal scale, but then round to the nearest power of 10
    const float gridScale = 1.f / optimalScale;
    const float logGridScale = (int)(log10f(gridScale));
    m_gridScale = powf(10.f, logGridScale);
}

float ViewerGraphicsWindow::ComputeOptimalScale() {
    // Scale the scene so the entire model can be viewed
    if (m_currentModel.m_isValid)
    {
        // Adjust the effective field of view if the window is taller than it is wide
        const float effectiveFOV = std::min(fieldOfView, fieldOfView * float(width()) / float(height()));

        // Compute optimal viewing distance as modelSize / atan(fov)
        const float modelSize = std::max(m_currentModel.m_AABBMax.length(), m_currentModel.m_AABBMin.length());
        const float optimalViewingDistance = modelSize / qAtan(qDegreesToRadians(effectiveFOV)) * 1.6f;

        // Scale the world so 4 looks like optimalViewingDistance
        return 4.f / optimalViewingDistance;
    }
    return 1.f;
}

bool ViewerGraphicsWindow::addPrimitive(QString primitiveName) 
{
    // Load  model
    QString filepath = QString("../Data/Primitives/%1").arg(primitiveName);
    return loadModel(filepath);
}


// ***************************************************
// This piece of code is used to receive and process the signal 
// sent by uniform controller.
// ***************************************************

void ViewerGraphicsWindow::colorRChanged(int val)
{
    double double_val = (double(val) -127.0) / 255.0;
    setLightLocation(double_val, lightPos[1], lightPos[2]);
}

void ViewerGraphicsWindow::colorGChanged(int val)
{
    double double_val = (double(val) - 127.0) / 255.0;
    setLightLocation(lightPos[0], double_val, lightPos[2]);
}
void ViewerGraphicsWindow::colorBChanged(int val)
{
    double double_val = (double(val) - 127.0) / 255.0;
    setLightLocation(lightPos[0], lightPos[1], double_val);
}

void ViewerGraphicsWindow::colorRainbowChanged(int val)
{
    float color = float(val)/100.0;
    float temp = 1.0/6.0;
    if (color < temp)
    {
        setADColor(1.0, 0.0 + (color*6.0), 0.0);
    }
    else if (color < temp * 2.0)
    {
        setADColor(1 - ((color-temp) * 6.0), 1.0, 0.0);
    }
    else if (color < temp * 3.0) 
    {
        setADColor(0.0, 1.0, 0.0 + ((color - (temp*2))*6.0));
    }
    else if (color < temp * 4.0)
    {
        setADColor(0.0, 1.0 - ((color - (temp * 3)) * 6.0), 1.0);
    }
    else if (color < temp * 5.0)
    {
        setADColor(0.0 + ((color - (temp * 4)) * 6.0), 0.0, 1.0);
    }
    else if (color < temp * 6.0)
    {
        setADColor(1.0, 0.0, 1.0 - ((color - (temp * 5)) * 6.0));
    }
}

void ViewerGraphicsWindow::colorRChanged64(double val)
{
    //TO DO
}

void ViewerGraphicsWindow::colorGChanged64(double val)
{
    //TO DO
}
void ViewerGraphicsWindow::colorBChanged64(double val)
{
    //TO DO
}
void ViewerGraphicsWindow::lightingSwitch(bool val)
{
    //TO DO, maybe ...
}

void ViewerGraphicsWindow::smoothingSwitch(bool val)
{
    //TO DO, maybe ...
}

void ViewerGraphicsWindow::effectType(int val)
{
    //TO DO, maybe ...
}

void ViewerGraphicsWindow::lightAmbient(float val)
{
    setADS(val, uKd, uKs);
}

void ViewerGraphicsWindow::lightDiffuse(float val)
{
    setADS(uKa, val, uKs);
}

void ViewerGraphicsWindow::lightSpecular(float val)
{
    setADS(uKa, uKd, val);
}

void ViewerGraphicsWindow::screenshotDialog() {
    if (!initialized) {
        return;
    }

    // Create a screenshot folder
    QString defaultFolder("../data/Screenshots/");
    if (!QDir(defaultFolder).exists()) {
        QDir().mkdir(defaultFolder);
    }

    // Have the user choose a file location
    QString filepath = QFileDialog::getSaveFileName(nullptr,
        tr("Save screenshot"),
        defaultFolder + "capture.png",
        tr("Images (*.bmp *.jpg *.jpeg *.png *.ppm *.xbm *.xpm)"));

    if (!filepath.isEmpty())
    {
        ViewerGraphicsWindow::exportFrame(filepath);
    }
}

void ViewerGraphicsWindow::saveDialog(QString filePath) {
    if (!initialized) {
        return;
    }

    QString filepath = QFileDialog::getSaveFileName(nullptr,
        tr("Save"),
        QString(),
        tr("all (*)"));

    if (!filepath.isEmpty())
    {
        // TO DO...
    }
}

void ViewerGraphicsWindow::exportFrame(QString filePath) {
    QImage frameCapture = grabFramebuffer();
    frameCapture.save(filePath);
}


// ***************************************************
// Getters & Setters
// ***************************************************

bool ViewerGraphicsWindow::GetLeftMousePressed()
{
    return m_leftMousePressed;
}
bool ViewerGraphicsWindow::GetRightMousePressed()
{
    return m_rightMousePressed;
}
QMatrix4x4 ViewerGraphicsWindow::GetScaleMatrix()
{
    return m_scaleMatrix;
}
void ViewerGraphicsWindow::SetScale(float scale)
{
    QMatrix4x4 newMat;
    newMat.scale(scale);
    m_scaleMatrix = newMat;
}
float ViewerGraphicsWindow::GetRotY()
{
    return rotY;
}
float ViewerGraphicsWindow::GetRotX()
{
    return rotX;
}
QMatrix4x4 ViewerGraphicsWindow::GetTranslationMatrix()
{
    return m_transMatrix;
}
QMatrix4x4 ViewerGraphicsWindow::GetModelMatrix()
{
    QVector3D xAxis(1, 0, 0);
    QVector3D yAxis(0, 1, 0);

    QMatrix4x4 rotMatrix;
    rotMatrix.rotate(rotY, xAxis);
    rotMatrix.rotate(rotX, yAxis);

    return m_transMatrix * rotMatrix * m_scaleMatrix;
}
bool ViewerGraphicsWindow::IsModelValid() 
{
    return m_currentModel.m_isValid;
}

//uniform vars getters and setters
QVector3D ViewerGraphicsWindow::getLightLocation()
{
    return lightPos;
}

void ViewerGraphicsWindow::setLightLocation(float x, float y, float z)
{
    lightPos = QVector3D(x, y, z);
    //m_program->setUniformValue(m_lightPosUniform, lightPos);
}

QVector3D ViewerGraphicsWindow::getADS()
{
    QVector3D ADS = QVector3D(uKa, uKd, uKs);
    return(ADS);
}

void ViewerGraphicsWindow::setADS(float a, float d, float s)
{
    uKa = a;
    uKd = d;
    uKs = s;
}

QVector4D ViewerGraphicsWindow::getSpecularColor()
{
    return specularColor;
}

void ViewerGraphicsWindow::setSpecularColor(float r, float g, float b)
{
    specularColor = QVector4D(r, g, b, 1.);
}

float ViewerGraphicsWindow::getShininess()
{
    return shininess;
}
void ViewerGraphicsWindow::setShininess(float new_shininess)
{
    shininess = new_shininess;
}
QVector4D ViewerGraphicsWindow::getADColor()
{
    return ADColor;
}
void ViewerGraphicsWindow::setADColor(float r, float g, float b)
{
    ADColor = QVector4D(r, g, b, 1.);
}
