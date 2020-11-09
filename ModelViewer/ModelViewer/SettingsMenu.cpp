#include "SettingsMenu.h"
#include "ViewerGraphicsWindow.h"
#include "TreeItem.h"

#include <QGridLayout>
#include <QListWidget>
#include <QLabel>
#include <qstringlist.h>
#include <QStandardItemModel>
#include <QFile>


SettingsMenu::SettingsMenu(ViewerGraphicsWindow* graphicsWindow, QWidget* parent)
	: m_pGraphicsWindow(graphicsWindow), QWidget(parent)
{
	setWindowTitle(QString::fromLatin1("Settings"));
	setContentsMargins(0, 0, 0, 0);
	resize(400, 300);

	m_pMainLayout = new QGridLayout(this);

	// Set up all of the settings
	m_pSettingsList = new QListWidget(this);
	new QListWidgetItem(m_MouseTitle, m_pSettingsList,settingsWidget::mouse);
	new QListWidgetItem(m_KebindTitle, m_pSettingsList, settingsWidget::keybind);
	new QListWidgetItem(m_ShaderTitle, m_pSettingsList, settingsWidget::shader);
	new QListWidgetItem(m_ModelTitle, m_pSettingsList, settingsWidget::model);
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
		case int(settingsWidget::mouse) :
			swapCurrentWidget(m_pMouseSettings);
			break;
		case int(settingsWidget::keybind) :
			swapCurrentWidget(m_pKeybindSettings);
			break;
		case int(settingsWidget::shader):
			swapCurrentWidget(m_pShaderSettings);
			break;
		case int(settingsWidget::model):
			swapCurrentWidget(m_pModelSettings);
			break;
	}
}



void SettingsMenu::SetupMouseSettings()
{
	m_pMouseSettings = new QTreeView(this);
	TreeItem* rootItem = new TreeItem({ tr("Key"), tr("Value") });
	TreeModel* model = new TreeModel(rootItem);

	TreeItem* panXSensitivity = new TreeItem({ tr("Horizontal Pan Sensitivity"), m_pGraphicsWindow->panXSensitivity }, rootItem);
	TreeItem* panYSensitivity = new TreeItem({ tr("Vertical Pan Sensitivity"), m_pGraphicsWindow->panYSensitivity }, rootItem);
	TreeItem* xRotateSensitivity = new TreeItem({ tr("Horizontal Rotation Sensitivity"), m_pGraphicsWindow->xRotateSensitivity }, rootItem);
	TreeItem* yRotateSensitivity = new TreeItem({ tr("Vertical Rotation Sensitivity"), m_pGraphicsWindow->yRotateSensitivity }, rootItem);
	TreeItem* zoomSensitivity = new TreeItem({ tr("Zoom Sensitivity"), m_pGraphicsWindow->zoomSensitivity }, rootItem);
	TreeItem* fieldOfView = new TreeItem({ tr("nearPlane"), m_pGraphicsWindow->fieldOfView }, rootItem);
	TreeItem* nearPlane = new TreeItem({ tr("Near Plane"), m_pGraphicsWindow->nearPlane }, rootItem);
	TreeItem* farPlane = new TreeItem({ tr("Far plane"), m_pGraphicsWindow->farPlane }, rootItem);
	rootItem->appendChild(panXSensitivity);
	rootItem->appendChild(panYSensitivity);
	rootItem->appendChild(xRotateSensitivity);
	rootItem->appendChild(yRotateSensitivity);
	rootItem->appendChild(zoomSensitivity);
	rootItem->appendChild(fieldOfView);
	rootItem->appendChild(nearPlane);
	rootItem->appendChild(farPlane);
	

	m_pMouseSettings->setModel(model);
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