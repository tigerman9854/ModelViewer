#include "SettingsMenu.h"

#include <QGridLayout>
#include <QListWidget>
#include <QLabel>

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
}

void SettingsMenu::LoadSettingsConf()
{
	// TODO: This
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
	m_pMouseSettings = new QWidget(this);
	QGridLayout* pMouseLayout = new QGridLayout(m_pMouseSettings);
	QLabel* mouseText = new QLabel(QString::fromLatin1("Mouse Placeholder (And some extra padding)"));
	pMouseLayout->addWidget(mouseText, 0, 0);
	m_pMouseSettings->setLayout(pMouseLayout);
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
*/