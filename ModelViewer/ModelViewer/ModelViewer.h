#pragma once

#include <QMainWindow>

class ViewerGraphicsWindow;

class Q_DECL_EXPORT ModelViewer : public QMainWindow
{
    Q_OBJECT

public:
    ModelViewer(QWidget *parent = Q_NULLPTR);

    ViewerGraphicsWindow* GetGraphicsWindow();
    
    void GetHelp();
    void GetQuit();
    

private:
    ViewerGraphicsWindow* m_pGraphicsWindow;
};
