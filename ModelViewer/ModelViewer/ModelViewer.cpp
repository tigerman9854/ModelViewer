#include "ModelViewer.h"
#include "ViewerGraphicsWindow.h"

#include <QWidget>
#include <QLayout>
#include <QMenuBar>
#include <QMenu>
#include <QShortcut>
#include <QMatrix4x4>

ModelViewer::ModelViewer(QWidget *parent)
    : QMainWindow(parent)
{
    // Create a new graphics window, and set it as the central widget
    m_pGraphicsWindow = new ViewerGraphicsWindow();
    QWidget* pContainer = QWidget::createWindowContainer(m_pGraphicsWindow);
    setCentralWidget(pContainer);

    // Change the size to something usable
    resize(640, 480);

    // Menu bar
    // -> File menu
    QMenu* pFileMenu = menuBar()->addMenu("File");
    pFileMenu->setObjectName("FileMenu");

    QMenu* pLoadMenu = pFileMenu->addMenu("Load");
    pLoadMenu->setObjectName("LoadMenu");
    pLoadMenu->addAction("Model", [=] {m_pGraphicsWindow->loadModel(); });

    // Primitive
    QMenu* pPrimitiveMenu = pLoadMenu->addMenu("Primitive");
    pPrimitiveMenu->setObjectName("PrimitiveMenu");
    pPrimitiveMenu->addAction("Sphere", [=]{m_pGraphicsWindow->addPrimitive("Sphere.obj"); });
    pPrimitiveMenu->addAction("Cube", [=] {m_pGraphicsWindow->addPrimitive("Cube.obj"); });
    pPrimitiveMenu->addAction("Torus", [=]{m_pGraphicsWindow->addPrimitive("Torus.obj"); });
    pPrimitiveMenu->addAction("Cylinder", [=] {m_pGraphicsWindow->addPrimitive("Cylinder.obj"); });
    pPrimitiveMenu->addAction("Diamond", [=] {m_pGraphicsWindow->addPrimitive("diamond.obj"); });
    pPrimitiveMenu->addAction("Tetrahedron", [=] {m_pGraphicsWindow->addPrimitive("Tetrahedron.obj"); });
    pPrimitiveMenu->addAction("Octahedron", [=] {m_pGraphicsWindow->addPrimitive("Octahedron.stl"); });
    pPrimitiveMenu->addAction("Icosahedron", [=] {m_pGraphicsWindow->addPrimitive("Icosahedron.stl"); });
    pPrimitiveMenu->addAction("Dodecahedron", [=] {m_pGraphicsWindow->addPrimitive("Dodecahedron.stl"); });

    pLoadMenu->addAction("Shader", [=] { /* TODO: m_pGraphicsWindow->loadShader(); */ });

    QMenu* pSaveMenu = pFileMenu->addMenu("Save");
    pSaveMenu->setObjectName("SaveMenu");
    pSaveMenu->addAction("Model", [=] { /* TODO: m_pGraphicsWindow->saveModel(); */ });
    pSaveMenu->addAction("Shader", [=] { /* TODO: m_pGraphicsWindow->saveShader(); */ });
    
    //Screenshot
    QMenu* pScreenshotMenu = pFileMenu->addMenu("Screenshot");
    pScreenshotMenu->addAction("JPG", [=] {  m_pGraphicsWindow->screenshotDialog("JPG"); });
    pScreenshotMenu->addAction("BMP", [=] {  m_pGraphicsWindow->screenshotDialog("BMP"); });
    pScreenshotMenu->addAction("PNG", [=] {  m_pGraphicsWindow->screenshotDialog("PNG"); });
    pScreenshotMenu->addAction("XBM", [=] {  m_pGraphicsWindow->screenshotDialog("XBM"); });
    pScreenshotMenu->addAction("XPM", [=] {  m_pGraphicsWindow->screenshotDialog("XPM"); });
    pScreenshotMenu->addAction("JPEG", [=] {  m_pGraphicsWindow->screenshotDialog("JPEG"); });
    pScreenshotMenu->addAction("PPM", [=] {  m_pGraphicsWindow->screenshotDialog("PPM"); });

    pFileMenu->addAction("Quit", [=] { /* TODO: m_pGraphicsWindow->exitGracefully(); */ });

    // -> Edit menu
    QMenu* pEditMenu = menuBar()->addMenu("Edit");
    pEditMenu->setObjectName("EditMenu");
    pEditMenu->addAction("Current Shader", [=] { /* TODO: m_pGraphicsWindow->editCurrentShader(); */ });

    // -> View menu
    QMenu* pViewMenu = menuBar()->addMenu("View");
    pViewMenu->setObjectName("ViewMenu");
    pViewMenu->addAction("Reset", [=] { m_pGraphicsWindow->resetView(); });

    // -> Help menu
    menuBar()->addAction("Help", [=] { /* TODO: m_pGraphicsWindow->displayHelpDoc(); */ }); 
};

ViewerGraphicsWindow* ModelViewer::GetGraphicsWindow() {
    return m_pGraphicsWindow;
}


