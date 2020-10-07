#include "ModelViewer.h"
#include "ViewerGraphicsWindow.h"

#include <QWidget>
#include <QLayout>

ModelViewer::ModelViewer(QWidget *parent)
    : QMainWindow(parent)
{
    // Create a new graphics window, and set it as the central widget
    m_pGraphicsWindow = new ViewerGraphicsWindow();
    QWidget* pContainer = QWidget::createWindowContainer(m_pGraphicsWindow);
    setCentralWidget(pContainer);

    // Change the size to something usable
    resize(640, 480);
};
