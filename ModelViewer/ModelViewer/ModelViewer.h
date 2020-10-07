#pragma once

#include <QMainWindow>

class ViewerGraphicsWindow;

class ModelViewer : public QMainWindow
{
    Q_OBJECT

public:
    ModelViewer(QWidget *parent = Q_NULLPTR);

private:
    ViewerGraphicsWindow* m_pGraphicsWindow;
};
