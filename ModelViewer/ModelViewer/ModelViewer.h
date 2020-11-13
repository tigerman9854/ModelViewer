#pragma once

#include <QMainWindow>
#include <QMenu>

class ViewerGraphicsWindow;
class GraphicsWindowDelegate;
class GraphicsWindowUniform;

class Q_DECL_EXPORT ModelViewer : public QMainWindow
{
    Q_OBJECT

public:
    ModelViewer(QWidget *parent = Q_NULLPTR);

    ViewerGraphicsWindow* GetGraphicsWindow();
    GraphicsWindowDelegate* GetGraphicsDelegate();
    
    void GetHelp();

    void GetQuit();

private:
    ViewerGraphicsWindow* m_pGraphicsWindow;
    GraphicsWindowDelegate* m_pGraphicsWindowDelegate;
    GraphicsWindowUniform* m_pGraphicsWindowUniform;
};


// Warning: Slightly hacky solution
// Special case of QMenu that returns focus to the graphics window when closed
class FocusMenu : public QMenu {
    Q_OBJECT

public:
    FocusMenu(ViewerGraphicsWindow* pGraphicsWindow, const QString& title, QWidget* parent = nullptr);
};
