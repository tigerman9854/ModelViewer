#pragma once

#include <QMainWindow>
#include <QMatrix4x4>

class ViewerGraphicsWindow;

class Q_DECL_EXPORT ModelViewer : public QMainWindow
{
    Q_OBJECT

public:
    ModelViewer(QWidget *parent = Q_NULLPTR);

    ViewerGraphicsWindow* GetGraphicsWindow();
    

private:
    ViewerGraphicsWindow* m_pGraphicsWindow;
};
