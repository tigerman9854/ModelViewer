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

    // add
    QMenu* pAddMenu = menuBar()->addMenu("Add");
    QMenu* pPrimetiveMenu = pAddMenu->addMenu("Primetive");
    pPrimetiveMenu->addAction("Sphere", [=]{m_pGraphicsWindow->addPrimitive("Sphere"); });
    pPrimetiveMenu->addAction("Cube", [=] {m_pGraphicsWindow->addPrimitive("Cube"); });
    pPrimetiveMenu->addAction("Torus", [=]{m_pGraphicsWindow->addPrimitive("Torus"); });
    pPrimetiveMenu->addAction("Cylinder", [=] {m_pGraphicsWindow->addPrimitive("Cylinder"); });
    pPrimetiveMenu->addAction("diamond", [=] {m_pGraphicsWindow->addPrimitive("diamond"); });
    pPrimetiveMenu->addAction("Tetrahedron", [=] {m_pGraphicsWindow->addPrimitive("Tetrahedron"); });
    pPrimetiveMenu->addAction("Octahedron", [=] {m_pGraphicsWindow->addPrimitive("Octahedron"); });
    pPrimetiveMenu->addAction("Icosahedron", [=] {m_pGraphicsWindow->addPrimitive("Icosahedron"); });
    pPrimetiveMenu->addAction("Dodecahedron", [=] {m_pGraphicsWindow->addPrimitive("Dodecahedron"); });

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