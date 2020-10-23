#include "ModelViewer.h"
#include "ViewerGraphicsWindow.h"

#include <QWidget>
#include <QLayout>
#include <QMenuBar>
#include <QMenu>

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

    QMenu* pLoadMenu = pFileMenu->addMenu("Load");
    pLoadMenu->addAction("Model", [=] {m_pGraphicsWindow->loadModel(); });

    // Primitive
    QMenu* pPrimitiveMenu = pLoadMenu->addMenu("Primitive");
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
    pSaveMenu->addAction("Model", [=] { /* TODO: m_pGraphicsWindow->saveModel(); */ });
    pSaveMenu->addAction("Shader", [=] { /* TODO: m_pGraphicsWindow->saveShader(); */ });

    pFileMenu->addAction("Screenshot", [=] { /* TODO: m_pGraphicsWindow->screenshot(); */ });
    pFileMenu->addAction("Quit", [=] { /* TODO: m_pGraphicsWindow->exitGracefully(); */ });
    // -> Edit menu
    QMenu* pEditMenu = menuBar()->addMenu("Edit");

    pEditMenu->addAction("Current Shader", [=] { /* TODO: m_pGraphicsWindow->editCurrentShader(); */ });
    // -> View menu
    QMenu* pViewMenu = menuBar()->addMenu("View");

    pViewMenu->addAction("Reset", [=] { /* TODO: m_pGraphicsWindow->resetView(); */ });
    // -> Help menu
    menuBar()->addAction("Help", [=] { /* TODO: m_pGraphicsWindow->displayHelpDoc(); */ }); 
};

ViewerGraphicsWindow* ModelViewer::GetGraphicsWindow() {
    return m_pGraphicsWindow;
}