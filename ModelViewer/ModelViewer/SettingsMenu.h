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
	enum settingsWidget {
		mouse,
		keybind,
		shader,
		model
	};

	ViewerGraphicsWindow* m_pGraphicsWindow = nullptr;
	QGridLayout* m_pMainLayout = nullptr;
	QListWidget* m_pSettingsList = nullptr;
	QWidget* m_pCurrentSettingsWidget = nullptr;

	// Settings menus
	void SetupMouseSettings();
	QWidget* m_pMouseSettings = nullptr;
	QString m_MouseTitle = QString::fromLatin1("Mouse");

	void SetupKeybindSettings();
	QWidget* m_pKeybindSettings = nullptr;
	QString m_KebindTitle = QString::fromLatin1("Keybindings");

	void SetupShaderSettings();
	QWidget* m_pShaderSettings = nullptr;
	QString m_ShaderTitle = QString::fromLatin1("Shader");

	void SetupModelSettings();
	QWidget* m_pModelSettings = nullptr;
	QString m_ModelTitle = QString::fromLatin1("Model");

	
};