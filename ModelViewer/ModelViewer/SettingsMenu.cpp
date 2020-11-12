#include "SettingsMenu.h"
#include "ViewerGraphicsWindow.h"
#include "TreeItem.h"

#include <QGridLayout>
#include <QListWidget>
#include <QLabel>
#include <qstringlist.h>
#include <QStandardItemModel>
#include <QFile>
#include <QFormLayout>
#include <QLineEdit>
#include <QCoreApplication>
#include <QSettings>


SettingsMenu::SettingsMenu(ViewerGraphicsWindow* graphicsWindow, QWidget* parent)
	: m_pGraphicsWindow(graphicsWindow), QWidget(parent)
{
	setWindowTitle(QString::fromLatin1("Settings"));
	setContentsMargins(0, 0, 0, 0);
	resize(400, 300);

	m_pMainLayout = new QGridLayout(this);

	// Set up all of the settings
	m_pSettingsList = new QListWidget(this);
	new QListWidgetItem(m_MouseTitle, m_pSettingsList, SettingsMenu::SETTINGSWIDGET::mouse);
	new QListWidgetItem(m_KebindTitle, m_pSettingsList, SettingsMenu::SETTINGSWIDGET::keybind);
	new QListWidgetItem(m_ShaderTitle, m_pSettingsList, SettingsMenu::SETTINGSWIDGET::shader);
	new QListWidgetItem(m_ModelTitle, m_pSettingsList, SettingsMenu::SETTINGSWIDGET::model);
	m_pSettingsList->setMaximumWidth(100);

	// Set up the indvidual setting menus
	SetupMouseSettings();
	SetupKeybindSettings();
	SetupShaderSettings();
	SetupModelSettings();

	m_pCurrentSettingsWidget = m_pMouseSettings;
	m_pMainLayout->addWidget(m_pSettingsList,0,0,5,1);
	m_pMainLayout->addWidget(m_pCurrentSettingsWidget, 0, 1, 5, 3);

	connect(m_pSettingsList, &QListWidget::currentItemChanged, this, &SettingsMenu::ChangeWindow);
}

void SettingsMenu::LoadSettingsConf()
{
	// TODO: Load up a file and set all values.
}

void SettingsMenu::SaveSettingsConf() {
	// TODO: Grab all the current values then dump them to a file
}

void SettingsMenu::ChangeWindow(QListWidgetItem* current, QListWidgetItem* previous)
{
	this->setWindowTitle(QString::fromLatin1("Settings/") + current->text());
	// Swap the current window. Might want to use something other then the text of the window...
	auto swapCurrentWidget = [=](QWidget* toSwap) {
		if (m_pCurrentSettingsWidget == toSwap) {
			return;
		}

		m_pMainLayout->replaceWidget(m_pCurrentSettingsWidget, toSwap);
		toSwap->show();
		m_pCurrentSettingsWidget->hide();
		m_pCurrentSettingsWidget = toSwap;
	};

	switch (current->type())
	{
		case int(SettingsMenu::SETTINGSWIDGET::mouse) :
			swapCurrentWidget(m_pMouseSettings);
			break;
		case int(SettingsMenu::SETTINGSWIDGET::keybind) :
			swapCurrentWidget(m_pKeybindSettings);
			break;
		case int(SettingsMenu::SETTINGSWIDGET::shader):
			swapCurrentWidget(m_pShaderSettings);
			break;
		case int(SettingsMenu::SETTINGSWIDGET::model):
			swapCurrentWidget(m_pModelSettings);
			break;
	}
}

void SettingsMenu::SetupMouseSettings()
{
	// Set up the widget...
	m_pMouseSettings = new QWidget(this);
	// and it's layout.
	QFormLayout* layout = new QFormLayout(m_pMouseSettings);

	// Set up each of the form fields with the default values
	panXSensitivity = new QLineEdit(QString::number(m_pGraphicsWindow->panXSensitivity));
	panYSensitivity = new QLineEdit(QString::number(m_pGraphicsWindow->panYSensitivity));
	xRotateSensitivity = new QLineEdit(QString::number(m_pGraphicsWindow->xRotateSensitivity));
	yRotateSensitivity = new QLineEdit(QString::number(m_pGraphicsWindow->yRotateSensitivity));
	zoomSensitivity = new QLineEdit(QString::number(m_pGraphicsWindow->zoomSensitivity));
	fieldOfView = new QLineEdit(QString::number(m_pGraphicsWindow->fieldOfView));
	nearPlane = new QLineEdit(QString::number(m_pGraphicsWindow->nearPlane));
	farPlane = new QLineEdit(QString::number(m_pGraphicsWindow->farPlane));

	// Set up input valadation for each field
	panXSensitivity->setValidator(new QDoubleValidator(0.00001, 10000.0, 5));
	panYSensitivity->setValidator(new QDoubleValidator(0.00001, 10000.0, 5));
	xRotateSensitivity->setValidator(new QDoubleValidator(0.00001, 10000.0, 5));
	yRotateSensitivity->setValidator(new QDoubleValidator(0.00001, 10000.0, 5));
	zoomSensitivity->setValidator(new QDoubleValidator(0.00001, 10000.0, 5));
	fieldOfView->setValidator(new QDoubleValidator(0.00001, 10000.0, 5));
	nearPlane->setValidator(new QDoubleValidator(0.00001, 10000.0, 5));
	farPlane->setValidator(new QDoubleValidator(0.00001, 10000.0, 5));
	
	// Add the fields to the layout
	layout->addRow(tr("Pan X Sensitivty"), panXSensitivity);
	layout->addRow(tr("Pan Y Sensitivty"), panYSensitivity);
	layout->addRow(tr("Rotate X Sensitivty"), xRotateSensitivity);
	layout->addRow(tr("Rotate X Sensitivty"), yRotateSensitivity);
	layout->addRow(tr("Zoom Sensitivty"), zoomSensitivity);
	layout->addRow(tr("Feild of View"), fieldOfView);
	layout->addRow(tr("Near Plane"), nearPlane);
	layout->addRow(tr("Far Plane"), farPlane);

	// Whenever a fields is edited & valadated update the corresponding value.
	connect(panXSensitivity, &QLineEdit::editingFinished, this, [=] {m_pGraphicsWindow->panXSensitivity = panXSensitivity->text().toFloat(); });
	connect(panYSensitivity, &QLineEdit::editingFinished, this, [=] {m_pGraphicsWindow->panYSensitivity = panYSensitivity->text().toFloat(); });
	connect(xRotateSensitivity, &QLineEdit::editingFinished, this, [=] {m_pGraphicsWindow->xRotateSensitivity = xRotateSensitivity->text().toFloat(); });
	connect(yRotateSensitivity, &QLineEdit::editingFinished, this, [=] {m_pGraphicsWindow->yRotateSensitivity = yRotateSensitivity->text().toFloat(); });
	connect(zoomSensitivity, &QLineEdit::editingFinished, this, [=] {m_pGraphicsWindow->zoomSensitivity = zoomSensitivity->text().toFloat(); });
	connect(fieldOfView, &QLineEdit::editingFinished, this, [=] {m_pGraphicsWindow->fieldOfView = fieldOfView->text().toFloat(); });
	connect(nearPlane, &QLineEdit::editingFinished, this, [=] {m_pGraphicsWindow->nearPlane = nearPlane->text().toFloat(); });
	connect(farPlane, &QLineEdit::editingFinished, this, [=] {m_pGraphicsWindow->farPlane = farPlane->text().toFloat(); });
}

void SettingsMenu::SetupKeybindSettings()
{
	m_pKeybindSettings = new QWidget(this);
	QGridLayout* pKeybindLayout = new QGridLayout(m_pKeybindSettings);
	QLabel* keyBindText = new QLabel(QString::fromLatin1("Keybind Placeholder"));
	pKeybindLayout->addWidget(keyBindText, 0, 0);
	m_pKeybindSettings->setLayout(pKeybindLayout);
	m_pKeybindSettings->hide();
	// https://doc.qt.io/qt-5/qkeysequenceedit.html
}

void SettingsMenu::SetupShaderSettings()
{
	m_pShaderSettings = new QWidget(this);
	QGridLayout* pShaderLayout = new QGridLayout(m_pShaderSettings);
	QLabel* shaderText = new QLabel(QString::fromLatin1("Shader Placeholder"));
	pShaderLayout->addWidget(shaderText, 0, 0);
	m_pShaderSettings->setLayout(pShaderLayout);
	m_pShaderSettings->hide();
}

void SettingsMenu::SetupModelSettings()
{
	m_pModelSettings = new QWidget(this);
	QGridLayout* pModelLayout = new QGridLayout(m_pModelSettings);
	QLabel* modelText = new QLabel(QString::fromLatin1("Model Placeholder"));
	pModelLayout->addWidget(modelText, 0, 0);
	m_pModelSettings->setLayout(pModelLayout);
	m_pModelSettings->hide();
}