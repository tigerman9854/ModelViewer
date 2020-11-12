#include "SettingsMenu.h"
#include "ViewerGraphicsWindow.h"
#include "TreeItem.h"
#include "ModelViewer.h"

#include <QGridLayout>
#include <QListWidget>
#include <QLabel>
#include <qstringlist.h>
#include <QStandardItemModel>
#include <QFile>
#include <QFormLayout>
#include <QLineEdit>
#include <QPushButton>


SettingsMenu::SettingsMenu(QWidget* parent)
	: QWidget(parent)
{
	setWindowTitle(QString::fromLatin1("Settings"));
	setContentsMargins(0, 0, 0, 0);
	resize(400, 300);

	m_pMainLayout = new QGridLayout(this);

	// Set up the settings
	settings = new QSettings("TBD", "Model Viewer");	
}

void SettingsMenu::SetupSettings(ViewerGraphicsWindow* graphicsWindow)
{
	// Set the required refrences
	m_pGraphicsWindow = graphicsWindow;

	// Set up all of the widgets
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
	m_pMainLayout->addWidget(m_pSettingsList, 0, 0, 5, 1);
	m_pMainLayout->addWidget(m_pCurrentSettingsWidget, 0, 1, 5, 3);

	connect(m_pSettingsList, &QListWidget::currentItemChanged, this, &SettingsMenu::ChangeWindow);
}

QSettings* SettingsMenu::getSettings()
{
	return settings;
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
	QPushButton* reset = new QPushButton(QString::fromLatin1("Reset"));

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
	layout->addRow(QString(), reset);

	// Whenever a fields is edited & valadated update the corresponding value.
	connect(panXSensitivity, &QLineEdit::editingFinished, this, [=] {
		settings->setValue("ViewerGraphicsWindow/panXSensitivity", panXSensitivity->text().toFloat());
		m_pGraphicsWindow->loadSettings();
	});
	connect(panYSensitivity, &QLineEdit::editingFinished, this, [=] {
		settings->setValue("ViewerGraphicsWindow/panYSensitivity", panYSensitivity->text().toFloat());
		m_pGraphicsWindow->loadSettings();
	});
	connect(xRotateSensitivity, &QLineEdit::editingFinished, this, [=] {
		settings->setValue("ViewerGraphicsWindow/xRotateSensitivity", xRotateSensitivity->text().toFloat());
		m_pGraphicsWindow->loadSettings();
	});
	connect(yRotateSensitivity, &QLineEdit::editingFinished, this, [=] {
		settings->setValue("ViewerGraphicsWindow/yRotateSensitivity", yRotateSensitivity->text().toFloat());
		m_pGraphicsWindow->loadSettings();
	});
	connect(zoomSensitivity, &QLineEdit::editingFinished, this, [=] {
		settings->setValue("ViewerGraphicsWindow/zoomSensitivity", zoomSensitivity->text().toFloat());
		m_pGraphicsWindow->loadSettings();
	});
	connect(fieldOfView, &QLineEdit::editingFinished, this, [=] {
		settings->setValue("ViewerGraphicsWindow/fieldOfView", fieldOfView->text().toFloat());
		m_pGraphicsWindow->loadSettings();
	});
	connect(nearPlane, &QLineEdit::editingFinished, this, [=] {
		settings->setValue("ViewerGraphicsWindow/nearPlane", nearPlane->text().toFloat());
		m_pGraphicsWindow->loadSettings();
	});
	connect(farPlane, &QLineEdit::editingFinished, this, [=] {
		settings->setValue("ViewerGraphicsWindow/farPlane", farPlane->text().toFloat());
		m_pGraphicsWindow->loadSettings();
	});
	connect(reset, &QPushButton::released, this, [=] {
		// Remove the saved values
		settings->remove("ViewerGraphicsWindow/panXSensitivity");
		settings->remove("ViewerGraphicsWindow/panYSensitivity");
		settings->remove("ViewerGraphicsWindow/xRotateSensitivity");
		settings->remove("ViewerGraphicsWindow/yRotateSensitivity");
		settings->remove("ViewerGraphicsWindow/zoomSensitivity");
		settings->remove("ViewerGraphicsWindow/fieldOfView");
		settings->remove("ViewerGraphicsWindow/nearPlane");
		settings->remove("ViewerGraphicsWindow/farPlane");

		// Reset the defalut values
		m_pGraphicsWindow->loadSettings();

		// Update the labels
		panXSensitivity->setText(QString::number(m_pGraphicsWindow->panXSensitivity));
		panYSensitivity->setText(QString::number(m_pGraphicsWindow->panYSensitivity));
		xRotateSensitivity->setText(QString::number(m_pGraphicsWindow->xRotateSensitivity));
		xRotateSensitivity->setText(QString::number(m_pGraphicsWindow->yRotateSensitivity));
		zoomSensitivity->setText(QString::number(m_pGraphicsWindow->zoomSensitivity));
		fieldOfView->setText(QString::number(m_pGraphicsWindow->fieldOfView));
		nearPlane->setText(QString::number(m_pGraphicsWindow->nearPlane));
		farPlane->setText(QString::number(m_pGraphicsWindow->farPlane));
	});
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