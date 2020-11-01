#pragma once
#include <QGridLayout>
#include <QListWidget>
#include <QWidget>
#include <QLabel>

// Forward Declares
class ViewerGraphicsWindow;

class SettingsMenu : public QWidget
{
	Q_OBJECT
public:
	SettingsMenu(ViewerGraphicsWindow* graphicsWindow, QWidget* parent = nullptr);
	void LoadSettingsConf();
	void ChangeWindow(QListWidgetItem* current, QListWidgetItem* previous);

//private slots:

private:
	ViewerGraphicsWindow* m_pGraphicsWindow = nullptr;
	QGridLayout* m_pMainLayout = nullptr;
	QListWidget* m_pSettingsList = nullptr;
	QWidget* m_pCurrentSettingsWidget = nullptr;

	// Settings menus
	QWidget* m_pMouseSettings = nullptr;
	QWidget* m_pKeybindSettings = nullptr;
	QWidget* m_pShaderSettings = nullptr;
	QWidget* m_pModelSettings = nullptr;
};