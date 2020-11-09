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


SettingsMenu::SettingsMenu(ViewerGraphicsWindow* graphicsWindow, QWidget* parent)
	: m_pGraphicsWindow(graphicsWindow), QWidget(parent)
{
	setWindowTitle(QString::fromLatin1("Settings"));
	setContentsMargins(0, 0, 0, 0);
	resize(400, 300);

	m_pMainLayout = new QGridLayout(this);

	// Set up all of the settings
	m_pSettingsList = new QListWidget(this);
	new QListWidgetItem(m_MouseTitle, m_pSettingsList, SETTINGSWIDGET::mouse);
	new QListWidgetItem(m_KebindTitle, m_pSettingsList, SETTINGSWIDGET::keybind);
	new QListWidgetItem(m_ShaderTitle, m_pSettingsList, SETTINGSWIDGET::shader);
	new QListWidgetItem(m_ModelTitle, m_pSettingsList, SETTINGSWIDGET::model);
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
	// Todo att 
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
		case int(SETTINGSWIDGET::mouse) :
			swapCurrentWidget(m_pMouseSettings);
			break;
		case int(SETTINGSWIDGET::keybind) :
			swapCurrentWidget(m_pKeybindSettings);
			break;
		case int(SETTINGSWIDGET::shader):
			swapCurrentWidget(m_pShaderSettings);
			break;
		case int(SETTINGSWIDGET::model):
			swapCurrentWidget(m_pModelSettings);
			break;
	}
}

void SettingsMenu::mouseReturnPressed()
{
	// FIXME: This is a tempory test function
	m_pGraphicsWindow->panXSensitivity = panXSensitivity->text().toFloat();
	m_pGraphicsWindow->panYSensitivity = panYSensitivity->text().toFloat();
	m_pGraphicsWindow->xRotateSensitivity = xRotateSensitivity->text().toFloat();
	m_pGraphicsWindow->yRotateSensitivity = yRotateSensitivity->text().toFloat();
	m_pGraphicsWindow->zoomSensitivity = zoomSensitivity->text().toFloat();
	m_pGraphicsWindow->fieldOfView = fieldOfView->text().toFloat();
	m_pGraphicsWindow->nearPlane = nearPlane->text().toFloat();
	m_pGraphicsWindow->farPlane = farPlane->text().toFloat();
}

void SettingsMenu::SetupMouseSettings()
{
	m_pMouseSettings = new QWidget(this);
	QFormLayout* layout = new QFormLayout(m_pMouseSettings);
	panXSensitivity = new QLineEdit(QString::number(m_pGraphicsWindow->panXSensitivity));
	panXSensitivity->setValidator(new QDoubleValidator(0.00001, 10000.0, 5));

	panYSensitivity = new QLineEdit(QString::number(m_pGraphicsWindow->panYSensitivity));
	panYSensitivity->setValidator(new QDoubleValidator(0.00001, 10000.0, 5));

	xRotateSensitivity = new QLineEdit(QString::number(m_pGraphicsWindow->xRotateSensitivity));
	xRotateSensitivity->setValidator(new QDoubleValidator(0.00001, 10000.0, 5));

	yRotateSensitivity = new QLineEdit(QString::number(m_pGraphicsWindow->yRotateSensitivity));
	yRotateSensitivity->setValidator(new QDoubleValidator(0.00001, 10000.0, 5));

	zoomSensitivity = new QLineEdit(QString::number(m_pGraphicsWindow->zoomSensitivity));
	zoomSensitivity->setValidator(new QDoubleValidator(0.00001, 10000.0, 5));

	fieldOfView = new QLineEdit(QString::number(m_pGraphicsWindow->fieldOfView));
	fieldOfView->setValidator(new QDoubleValidator(0.00001, 10000.0, 5));

	nearPlane = new QLineEdit(QString::number(m_pGraphicsWindow->nearPlane));
	nearPlane->setValidator(new QDoubleValidator(0.00001, 10000.0, 5));

	farPlane = new QLineEdit(QString::number(m_pGraphicsWindow->farPlane));
	farPlane->setValidator(new QDoubleValidator(0.00001, 10000.0, 5));

	layout->addRow(tr("Pan X Sensitivty"), panXSensitivity);
	layout->addRow(tr("Pan Y Sensitivty"), panYSensitivity);
	layout->addRow(tr("Rotate X Sensitivty"), xRotateSensitivity);
	layout->addRow(tr("Rotate X Sensitivty"), yRotateSensitivity);
	layout->addRow(tr("Zoom Sensitivty"), zoomSensitivity);
	layout->addRow(tr("Feild of View"), fieldOfView);
	layout->addRow(tr("Near Plane"), nearPlane);
	layout->addRow(tr("Far Plane"), farPlane);

	// FIXME: There should be a unique handler and we should be using QLineEdit TextEdited signal
	connect(panXSensitivity, &QLineEdit::returnPressed, this, &SettingsMenu::mouseReturnPressed);
	connect(panYSensitivity, &QLineEdit::returnPressed, this, &SettingsMenu::mouseReturnPressed);
	connect(xRotateSensitivity, &QLineEdit::returnPressed, this, &SettingsMenu::mouseReturnPressed);
	connect(yRotateSensitivity, &QLineEdit::returnPressed, this, &SettingsMenu::mouseReturnPressed);
	connect(zoomSensitivity, &QLineEdit::returnPressed, this, &SettingsMenu::mouseReturnPressed);
	connect(fieldOfView, &QLineEdit::returnPressed, this, &SettingsMenu::mouseReturnPressed);
	connect(nearPlane, &QLineEdit::returnPressed, this, &SettingsMenu::mouseReturnPressed);
	connect(farPlane, &QLineEdit::returnPressed, this, &SettingsMenu::mouseReturnPressed);
}

void SettingsMenu::SetupKeybindSettings()
{
	m_pKeybindSettings = new QWidget(this);
	QGridLayout* pKeybindLayout = new QGridLayout(m_pKeybindSettings);
	QLabel* keyBindText = new QLabel(QString::fromLatin1("Keybind Placeholder"));
	pKeybindLayout->addWidget(keyBindText, 0, 0);
	m_pKeybindSettings->setLayout(pKeybindLayout);
	m_pKeybindSettings->hide();
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


// FIXME: Remove the src before merge
/*
https://doc.qt.io/qt-5/qtwidgets-module.html
https://doc.qt.io/qt-5/qgridlayout.html
https://doc.qt.io/qt-5/qtwidgets-layouts-basiclayouts-example.html
https://www.bogotobogo.com/Qt/Qt5_GridLayout.php

https://doc.qt.io/qt-5/qlistwidget.html
https://doc.qt.io/qt-5/qlistwidgetitem.html
https://doc.qt.io/qt-5/qlistwidgetitem.html#data

https://doc.qt.io/qt-5/qtreeview.html
https://doc.qt.io/qt-5/qstandarditemmodel.html
https://doc.qt.io/qt-5/qstandarditem.html
*/

/* DEAD CODE:
* 
* m_pMouseSettings = new QTreeWidget(this);
	m_pMouseSettings->setEditTriggers(QAbstractItemView::NoEditTriggers);
	m_pMouseSettings->setColumnCount(2);
	m_pMouseSettings->setHeaderLabels(settingHeader);
	connect(m_pMouseSettings, SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)), this, SLOT(onTreeWidgetItemDoubleClicked(QTreeWidgetItem*,int)));
	connect(m_pMouseSettings->itemDelegate(), &QAbstractItemDelegate::commitData, this, SLOT(onTreeWidgetCommitData(QTreeWidgetItem*)));

	QStringList panXSensitivity = {"Pan X Sensitivty"};
	QStringList panYSensitivity = {"Pan Y Sensitivty"};
	QStringList xRotateSensitivity = {"Rotate X Sensitivty"};
	QStringList yRotateSensitivity = {"Pan Y Sensitivty"};
	QStringList zoomSensitivity = {"Zoom Sensitivty"};
	QStringList fieldOfView = {"Field of View"};
	QStringList nearPlane = {"Near Plane"};
	QStringList farPlane = {"Far Plane"};

	QList<QTreeWidgetItem*> items;
	// FIXME: The bellow will have to be populated from a loaded config not form the default values we have.
	items.append(new QTreeWidgetItem(m_pMouseSettings,panXSensitivity));
	items[0]->setData(1, Qt::EditRole, m_pGraphicsWindow->panXSensitivity);
	items[0]->setFlags(Qt::ItemIsEditable | Qt::ItemIsSelectable | Qt::ItemIsEnabled);

	items.append(new QTreeWidgetItem(m_pMouseSettings, panYSensitivity));
	items[1]->setData(1, Qt::EditRole, m_pGraphicsWindow->panYSensitivity);
	items[1]->setFlags(Qt::ItemIsEditable | Qt::ItemIsSelectable | Qt::ItemIsEnabled);

	items.append(new QTreeWidgetItem(m_pMouseSettings, xRotateSensitivity));
	items[2]->setData(1, Qt::EditRole, m_pGraphicsWindow->xRotateSensitivity);
	items[2]->setFlags(Qt::ItemIsEditable | Qt::ItemIsSelectable | Qt::ItemIsEnabled);

	items.append(new QTreeWidgetItem(m_pMouseSettings, yRotateSensitivity));
	items[3]->setData(1, Qt::EditRole, m_pGraphicsWindow->yRotateSensitivity);
	items[3]->setFlags(Qt::ItemIsEditable | Qt::ItemIsSelectable | Qt::ItemIsEnabled);

	items.append(new QTreeWidgetItem(m_pMouseSettings, zoomSensitivity));
	items[4]->setData(1, Qt::EditRole, m_pGraphicsWindow->zoomSensitivity);
	items[4]->setFlags(Qt::ItemIsEditable | Qt::ItemIsSelectable | Qt::ItemIsEnabled);

	items.append(new QTreeWidgetItem(m_pMouseSettings, fieldOfView));
	items[5]->setData(1, Qt::EditRole, m_pGraphicsWindow->fieldOfView);
	items[5]->setFlags(Qt::ItemIsEditable | Qt::ItemIsSelectable | Qt::ItemIsEnabled);

	items.append(new QTreeWidgetItem(m_pMouseSettings, nearPlane));
	items[6]->setData(1, Qt::EditRole, m_pGraphicsWindow->nearPlane);
	items[6]->setFlags(Qt::ItemIsEditable | Qt::ItemIsSelectable | Qt::ItemIsEnabled);

	items.append(new QTreeWidgetItem(m_pMouseSettings, farPlane));
	items[7]->setData(1, Qt::EditRole, m_pGraphicsWindow->farPlane);
	items[7]->setFlags(Qt::ItemIsEditable | Qt::ItemIsSelectable | Qt::ItemIsEnabled);

	m_pMouseSettings->insertTopLevelItems(0, items);

	void SettingsMenu::onTreeWidgetItemDoubleClicked(QTreeWidgetItem* item, int column)
{
	m_pMouseSettings->editItem(item, 1);
}

void SettingsMenu::onTreeWidgetCommitData(QTreeWidgetItem* item, int column)
{
	m_pGraphicsWindow->panXSensitivity = item->data(1, Qt::EditRole).toFloat();
}
*/