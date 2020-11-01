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
	new QListWidgetItem(tr("Mouse Settings"), m_pSettingsList);
	new QListWidgetItem(tr("Keybindings"), m_pSettingsList);
	new QListWidgetItem(tr("Shader Settings"), m_pSettingsList);
	new QListWidgetItem(tr("Model Settings"), m_pSettingsList);

	// Set up the mouse settings
	m_pMouseSettings = new QWidget(this);
	QGridLayout* pMouseLayout = new QGridLayout(m_pMouseSettings);
	pMouseLayout->addWidget(&QLabel(QString::fromLatin1("Test")),0,0);
	m_pMouseSettings->setLayout(pMouseLayout);

	m_pCurrentSettingsWidget = m_pMouseSettings;
	m_pMainLayout->addWidget(m_pSettingsList,0,0,5,2);
	m_pMainLayout->addWidget(m_pCurrentSettingsWidget, 0, 2, 5, 3);

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
}
