#pragma once
#include <QGridLayout>
#include <QListWidget>
#include <QWidget>
#include <QLabel>
#include <QTreeView>
#include <QHeaderView>
#include <QSettings>

// Forward Declares
class ViewerGraphicsWindow;

class SettingsMenu : public QWidget
{
	Q_OBJECT
public:
	SettingsMenu(ViewerGraphicsWindow* gWindow, QWidget* parent = nullptr);
	void ChangeWindow(QListWidgetItem* current, QListWidgetItem* previous);
	QSettings* getSettings();
	

//private slots:

private:
	enum SETTINGSWIDGET {
		mouse,
		keybind,
		view,
	};

	ViewerGraphicsWindow* m_pGraphicsWindow = nullptr;
	QGridLayout* m_pMainLayout = nullptr;
	QListWidget* m_pSettingsList = nullptr;
	QWidget* m_pCurrentSettingsWidget = nullptr;
	QStringList settingHeader = { "Key","Value" };
	QSettings* settings;

	// Settings menus
	void SetupMouseSettings();
	QWidget* m_pMouseSettings = nullptr;
	QString m_MouseTitle = QString::fromLatin1("Mouse");

	void SetupKeybindSettings();
	QWidget* m_pKeybindSettings = nullptr;
	QString m_KebindTitle = QString::fromLatin1("Keybindings");

	void SetupViewSettings();
	QWidget* m_pViewSettings = nullptr;
	QString m_ViewTitle = QString::fromLatin1("View");
};


/* List of all keybindings
ViewerGraphicsWindow/ 
	increase_speed | QT::Key
	decrease_speed | QT::Key
	elevate_forwards | QT::Key
	elevate_backwards | QT::Key
	strafe_left | QT::Key
	strafe_right | QT::Key
	scale_up | QT::Key
	scale_down | QT::Key
	pitch_up | QKeySequence
	pitch_down | QKeySequence
	spin_right | QKeySequence
	spin_left | QKeySequence

	panXSensitivity | Float
	panYSensitivity | Float
	xRotateSensitivity | Float
	yRotateSensitivity | Float
	movementSensitivity | Float
	zoomSensitivity | Float
	fieldOfView | Float
	nearPlane | Float
	farPlane | Float

	gridToggle | Bool
	toggleAxis | Bool
	toggleStats | Bool
	msaaLevel | Int
*/