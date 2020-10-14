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

    // Build menu
    QMenu* pFileMenu = menuBar()->addMenu("File");
    QMenu* pLoadMenu = pFileMenu->addMenu("Load");
    pLoadMenu->addAction("Model", [=] {m_pGraphicsWindow->loadModel(); });
};

ViewerGraphicsWindow* ModelViewer::GetGraphicsWindow() {
    return m_pGraphicsWindow;
}