#include "ModelViewer.h"
#include "ViewerGraphicsWindow.h"
#include "GraphicsWindowDelegate.h"
#include "SettingsMenu.h"

#include <QWidget>
#include <QLayout>
#include <QMenuBar>
#include <QMenu>
#include <QShortcut>
#include <QMatrix4x4>
#include <QLabel>
#include <QUrl>
#include <QDesktopServices>
#include <QSettings>

ModelViewer::ModelViewer(QWidget *parent)
    : QMainWindow(parent)
{
    // Create the settings menu (Contains the 'global' settings object)
    m_pSettingsMenu = new SettingsMenu();
    
    // Create a new graphics window, and set it as the central widget
    m_pGraphicsWindow = new ViewerGraphicsWindow(m_pSettingsMenu);
    m_pGraphicsWindowDelegate = new GraphicsWindowDelegate(m_pGraphicsWindow);
    m_pSettingsMenu->SetupSettings(m_pGraphicsWindow);
    
    setCentralWidget(m_pGraphicsWindowDelegate);

    // Change the size to something usable
    resize(640, 480);

    // Menu bar
    // -> File menu
    QMenu* pFileMenu = new FocusMenu(m_pGraphicsWindow, "File", this);
    menuBar()->addMenu(pFileMenu);
    pFileMenu->setObjectName("FileMenu");

    QMenu* pLoadMenu = pFileMenu->addMenu("Load");
    pLoadMenu->setObjectName("LoadMenu");
    pLoadMenu->addAction("Model", [=] {m_pGraphicsWindow->loadModel(); }, QKeySequence(Qt::CTRL + Qt::Key_O));

    QMenu* pShaderMenu = pLoadMenu->addMenu("Shader");
    pShaderMenu->addAction("Vertex", [=]{m_pGraphicsWindow->loadVertexShader(); });
    pShaderMenu->addAction("Fragment", [=]{m_pGraphicsWindow->loadFragmentShader(); });
    pShaderMenu->addAction("Reload Current Shaders", [=]{m_pGraphicsWindow->reloadCurrentShaders(); });


    // Primitive
    QMenu* pPrimitiveMenu = pLoadMenu->addMenu("Primitive");
    pPrimitiveMenu->setObjectName("PrimitiveMenu");
    pPrimitiveMenu->addAction("Sphere", [=]{m_pGraphicsWindow->addPrimitive("Sphere.obj"); });
    pPrimitiveMenu->addAction("Cube", [=] {m_pGraphicsWindow->addPrimitive("Cube.obj"); });
    pPrimitiveMenu->addAction("Torus", [=]{m_pGraphicsWindow->addPrimitive("Torus.obj"); });
    pPrimitiveMenu->addAction("Cylinder", [=] {m_pGraphicsWindow->addPrimitive("Cylinder.obj"); });
    pPrimitiveMenu->addAction("Diamond", [=] {m_pGraphicsWindow->addPrimitive("diamond.obj"); });
    pPrimitiveMenu->addAction("Tetrahedron", [=] {m_pGraphicsWindow->addPrimitive("Tetrahedron.obj"); });
    pPrimitiveMenu->addAction("Octahedron", [=] {m_pGraphicsWindow->addPrimitive("Octahedron.stl"); });
    pPrimitiveMenu->addAction("Icosahedron", [=] {m_pGraphicsWindow->addPrimitive("Icosahedron.stl"); });
    pPrimitiveMenu->addAction("Dodecahedron", [=] {m_pGraphicsWindow->addPrimitive("Dodecahedron.stl"); });

    QMenu* pSaveMenu = pFileMenu->addMenu("Save");
    pSaveMenu->setObjectName("SaveMenu");
    pSaveMenu->addAction("Model", [=] { /* TODO: m_pGraphicsWindow->saveModel(); */ });
    pSaveMenu->addAction("Shader", [=] { /* TODO: m_pGraphicsWindow->saveShader(); */ });
    
    //Screenshot
    pFileMenu->addAction("Screenshot", [=] {  m_pGraphicsWindow->screenshotDialog(); }, QKeySequence(Qt::CTRL + Qt::Key_P));

    // Close
    pFileMenu->addAction("Close", [=] { m_pGraphicsWindow->unloadModel(); }, QKeySequence(Qt::CTRL + Qt::Key_W));

    // quit button
    pFileMenu->addAction("Quit", [=] { GetQuit();}, QKeySequence(Qt::CTRL + Qt::Key_Q));

    // setings menu
    pFileMenu->addAction("Settings", [=] { m_pSettingsMenu->show(); });

    // -> Edit menu
    QMenu* pEditMenu = new FocusMenu(m_pGraphicsWindow, "Edit", this);
    menuBar()->addMenu(pEditMenu);
    pEditMenu->setObjectName("EditMenu");

    pEditMenu->addAction("Shader File", [=] { m_pGraphicsWindow->openShaderFile(); });
    pEditMenu->addAction("Current Shaders", [=] { m_pGraphicsWindow->editCurrentShaders(); });

    // -> View menu
    QMenu* pViewMenu = new FocusMenu(m_pGraphicsWindow, "View", this);
    menuBar()->addMenu(pViewMenu);
    pViewMenu->setObjectName("ViewMenu");
    pViewMenu->addAction("Reset", [=] { m_pGraphicsWindow->resetView(); }, QKeySequence(Qt::CTRL + Qt::Key_R));

    // -> Help menu

    // if user click help menu, it will let user go to github page to read the Wiki
    QMenu* pHelpMenu = new FocusMenu(m_pGraphicsWindow, "Help", this);
    menuBar()->addMenu(pHelpMenu);
    pHelpMenu->setObjectName("HelpMenu");
    pHelpMenu->addAction("Help", [=] {GetHelp(); }, QKeySequence(Qt::CTRL + Qt::Key_F1));
}


ViewerGraphicsWindow* ModelViewer::GetGraphicsWindow() {
    return m_pGraphicsWindow;
}

SettingsMenu* ModelViewer::GetSettingsWindow() {
    return m_pSettingsMenu;
}

GraphicsWindowDelegate* ModelViewer::GetGraphicsDelegate() {
    return m_pGraphicsWindowDelegate;
}


void ModelViewer::GetHelp() {
    
    QString link = "https://github.com/tigerman9854/ModelViewer/wiki";
    QDesktopServices::openUrl(QUrl(link));
}


void ModelViewer::GetQuit() {
    close();
}

FocusMenu::FocusMenu(ViewerGraphicsWindow* pGraphicsWindow, const QString& title, QWidget* parent)
    : QMenu(title, parent)
{
    // When the menu is shown, clear all currently pressed keys so the graphics window
    // does not keep moving while the menu is shown
    connect(this, &QMenu::aboutToShow, this, [=] {pGraphicsWindow->ClearKeyboard(); });

    // When the menu is hidden, return focus to the graphics window so it can capture
    // future key presses
    connect(this, &QMenu::aboutToHide, this, [=] {pGraphicsWindow->requestActivate(); });
}
