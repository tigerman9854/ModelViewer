#pragma once

#include <QMainWindow>

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

private:
    ViewerGraphicsWindow* m_pGraphicsWindow = nullptr;
    GraphicsWindowDelegate* m_pGraphicsWindowDelegate = nullptr;
    SettingsMenu* m_pSettingsMenu = nullptr;
};
