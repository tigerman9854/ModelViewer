#pragma once

#include <QMainWindow>
#include <QMenu>

class ViewerGraphicsWindow;
class GraphicsWindowDelegate;
class SettingsMenu;
class GraphicsWindowUniform;

class Q_DECL_EXPORT ModelViewer : public QMainWindow
{
    Q_OBJECT

public:
    ModelViewer(QWidget *parent = Q_NULLPTR);

    ViewerGraphicsWindow* GetGraphicsWindow();
    GraphicsWindowDelegate* GetGraphicsDelegate();
    SettingsMenu* GetSettingsWindow();
    
    void GetHelp();

    void GetQuit();

    

private:
    ViewerGraphicsWindow* m_pGraphicsWindow = nullptr;
    GraphicsWindowDelegate* m_pGraphicsWindowDelegate = nullptr;
    GraphicsWindowUniform* m_pGraphicsWindowUniform = nullptr;
    SettingsMenu* m_pSettingsMenu = nullptr;
};


// Warning: Slightly hacky solution
// Special case of QMenu that returns focus to the graphics window when closed
class FocusMenu : public QMenu {
    Q_OBJECT

public:
    FocusMenu(ViewerGraphicsWindow* pGraphicsWindow, const QString& title, QWidget* parent = nullptr);
};
