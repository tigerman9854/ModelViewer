#pragma once

#include <QMainWindow>
#include <QMenu>

class ViewerGraphicsWindow;
class GraphicsWindowDelegate;
class SettingsMenu;

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
    ViewerGraphicsWindow* m_pGraphicsWindow = nullptr;
    GraphicsWindowDelegate* m_pGraphicsWindowDelegate = nullptr;
    SettingsMenu* m_pSettingsMenu = nullptr;
};


// Warning: Slightly hacky solution
// Special case of QMenu that returns focus to the graphics window when closed
class FocusMenu : public QMenu {
    Q_OBJECT

public:
    FocusMenu(ViewerGraphicsWindow* pGraphicsWindow, const QString& title, QWidget* parent = nullptr);
};
