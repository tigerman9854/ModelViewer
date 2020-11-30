#include "SettingsMenu.h"
#include "ViewerGraphicsWindow.h"
#include "ModelViewer.h"
#include "KeyBindEdit.h"
#include "KeySequenceParse.h"

#include <math.h>
#include <QGridLayout>
#include <QListWidget>
#include <QLabel>
#include <qstringlist.h>
#include <QStandardItemModel>
#include <QFile>
#include <QFormLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QScrollArea>
#include <QComboBox>


SettingsMenu::SettingsMenu(ViewerGraphicsWindow* gWindow, QWidget* parent)
	: m_pGraphicsWindow(gWindow), QWidget(parent)
{
	setWindowTitle(QString::fromLatin1("Settings"));
	setContentsMargins(0, 0, 0, 0);
	resize(400, 300);

	m_pMainLayout = new QGridLayout(this);

	// Set up the settings
	settings = new QSettings("The Model Viewers team", "Model Viewer");	

	// Set up all of the widgets
	m_pSettingsList = new QListWidget(this);
	new QListWidgetItem(m_MouseTitle, m_pSettingsList, SettingsMenu::SETTINGSWIDGET::mouse);
	new QListWidgetItem(m_KebindTitle, m_pSettingsList, SettingsMenu::SETTINGSWIDGET::keybind);
	new QListWidgetItem(m_ViewTitle, m_pSettingsList, SettingsMenu::SETTINGSWIDGET::view);
	m_pSettingsList->setMaximumWidth(100);

	// Set up the indvidual setting menus
	SetupMouseSettings();
	SetupKeybindSettings();
	SetupViewSettings();

	m_pCurrentSettingsWidget = m_pMouseSettings;
	m_pMainLayout->addWidget(m_pSettingsList, 0, 0, 5, 1);
	m_pMainLayout->addWidget(m_pCurrentSettingsWidget, 0, 1, 5, 3);

	connect(m_pSettingsList, &QListWidget::currentItemChanged, this, &SettingsMenu::ChangeWindow);
	this->setStyleSheet("background-color: rgb(28,30,37); color: rgb(186,186,186); border-color: blue; selection-color: rgb(100,100,100);");
	m_pMouseSettings->setStyleSheet("QLineEdit {border-radius: 4px; border: 1px solid rgb(100,100,100)} QPushButton { border-style: outset; border-width: 1px; border-radius: 4px; border-color: rgb(100,100,100); padding: 3px;} QPushButton:pressed {background-color: rgb(128,130,137); border-style: inset;}");
	m_pKeybindSettings->setStyleSheet("QLineEdit {border-radius: 4px; border: 1px solid rgb(100,100,100)} QPushButton { border-style: outset; border-width: 1px; border-radius: 4px; border-color: rgb(100,100,100); padding: 3px;} QPushButton:pressed {background-color: rgb(128,130,137); border-style: inset;}");
	m_pViewSettings->setStyleSheet("QPushButton { border-style: outset; border-width: 1px; border-radius: 4px; border-color: rgb(100,100,100); padding: 3px;} QPushButton:pressed {background-color: rgb(128,130,137); border-style: inset;} QComboBox {border-style: inset; border-radius: 4px; border-width: 1px; border-color: rgb(100,100,100);}");


	// Make sure that this window closes on main window close. Why set it to faslse? Only the QT gods know.
	this->setAttribute(Qt::WA_QuitOnClose, false);
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
		case int(SettingsMenu::SETTINGSWIDGET::view):
			swapCurrentWidget(m_pViewSettings);
			break;
	}
}

void SettingsMenu::SetupMouseSettings()
{
	// Set up the widget...
	m_pMouseSettings = new QWidget(this);
	m_pMouseSettings->setObjectName("mouseSettings");
	// and it's layout.
	QFormLayout* layout = new QFormLayout(m_pMouseSettings);

	// Set up each of the form fields with the default values
	QLineEdit* panXSensitivity = new QLineEdit(QString::number(m_pGraphicsWindow->panXSensitivity));
	panXSensitivity->setObjectName("panXSensitivity");
	QLineEdit* panYSensitivity = new QLineEdit(QString::number(m_pGraphicsWindow->panYSensitivity));
	panYSensitivity->setObjectName("panYSensitivity");
	QLineEdit* xRotateSensitivity = new QLineEdit(QString::number(m_pGraphicsWindow->xRotateSensitivity));
	xRotateSensitivity->setObjectName("xRotateSensitivity");
	QLineEdit* yRotateSensitivity = new QLineEdit(QString::number(m_pGraphicsWindow->yRotateSensitivity));
	yRotateSensitivity->setObjectName("yRotateSensitivity");
	QLineEdit* movementSensitivity = new QLineEdit(QString::number(m_pGraphicsWindow->movementSensitivity));
	movementSensitivity->setObjectName("movementSensitivity");
	QLineEdit* zoomSensitivity = new QLineEdit(QString::number(m_pGraphicsWindow->zoomSensitivity));
	zoomSensitivity->setObjectName("zoomSensitivity");
	QLineEdit* fieldOfView = new QLineEdit(QString::number(m_pGraphicsWindow->fieldOfView));
	fieldOfView->setObjectName("fieldOfView");
	QLineEdit* nearPlane = new QLineEdit(QString::number(m_pGraphicsWindow->nearPlane));
	nearPlane->setObjectName("nearPlane");
	QLineEdit* farPlane = new QLineEdit(QString::number(m_pGraphicsWindow->farPlane));
	farPlane->setObjectName("farPlane");
	QPushButton* reset = new QPushButton(QString::fromLatin1("Reset All"));
	reset->setObjectName("resetMouseSettings");

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
	layout->addRow(tr("Pan X Sensitivity"), panXSensitivity);
	layout->addRow(tr("Pan Y Sensitivity"), panYSensitivity);
	layout->addRow(tr("Rotate X Sensitivity"), xRotateSensitivity);
	layout->addRow(tr("Rotate X Sensitivity"), yRotateSensitivity);
	layout->addRow(tr("Movement Sensitivity"), movementSensitivity);
	layout->addRow(tr("Zoom Sensitivty"), zoomSensitivity);
	layout->addRow(tr("Field of View"), fieldOfView);
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
	connect(movementSensitivity, &QLineEdit::editingFinished, this, [=] {
		settings->setValue("ViewerGraphicsWindow/movementSensitivity", movementSensitivity->text().toFloat());
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
		settings->remove("ViewerGraphicsWindow/movementSensitivity");
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
	m_pKeybindSettings->setObjectName("keybindSettings");
	QFormLayout* layout = new QFormLayout(m_pKeybindSettings);

	KeyBindEdit* increaseSpeed = new KeyBindEdit("ViewerGraphicsWindow/increase_speed", Qt::Key::Key_Shift);
	increaseSpeed->setObjectName("increaseSpeed");
	KeyBindEdit* decreaseSpeed = new KeyBindEdit("ViewerGraphicsWindow/decrease_speed", Qt::Key::Key_Control);
	decreaseSpeed->setObjectName("decreaseSpeed");
	KeyBindEdit* elevateForwards = new KeyBindEdit("ViewerGraphicsWindow/elevate_forwards", Qt::Key::Key_W);
	elevateForwards->setObjectName("elevateForwards");
	KeyBindEdit* elevateBackwards = new KeyBindEdit("ViewerGraphicsWindow/elevate_backwards", Qt::Key::Key_S);
	elevateBackwards->setObjectName("elevateBackwards");
	KeyBindEdit* strafeLeft = new KeyBindEdit("ViewerGraphicsWindow/strafeLeft", Qt::Key::Key_A);
	strafeLeft->setObjectName("strafeLeft");
	KeyBindEdit* strafeRight = new KeyBindEdit("ViewerGraphicsWindow/strafeRight", Qt::Key::Key_D);
	strafeRight->setObjectName("strafeRight");
	KeyBindEdit* scaleUp = new KeyBindEdit("ViewerGraphicsWindow/scaleUp", Qt::Key::Key_E);
	scaleUp->setObjectName("scaleUp");
	KeyBindEdit* scaleDown = new KeyBindEdit("ViewerGraphicsWindow/scaleDown", Qt::Key::Key_Q);
	scaleDown->setObjectName("scaleDown");
	KeyBindEdit* pitchUp = new KeyBindEdit("ViewerGraphicsWindow/pitchUp", Qt::Key::Key_Up);
	pitchUp->setObjectName("pitchUp");
	KeyBindEdit* pitchDown = new KeyBindEdit("ViewerGraphicsWindow/pitchDown", Qt::Key::Key_Down);
	pitchDown->setObjectName("pitchDown");
	KeyBindEdit* spinRight = new KeyBindEdit("ViewerGraphicsWindow/spinRight", Qt::Key::Key_Right);
	spinRight->setObjectName("spinRight");
	KeyBindEdit* spinLeft = new KeyBindEdit("ViewerGraphicsWindow/spinLeft", Qt::Key::Key_Left);
	spinLeft->setObjectName("spinLeft");
	QPushButton* reset = new QPushButton(QString::fromLatin1("Reset All"));
	reset->setObjectName("resetKeybinds");

	layout->addRow(tr("Increase Speed"), increaseSpeed);
	layout->addRow(tr("Decrease Speed"), decreaseSpeed);
	layout->addRow(tr("Move Up"), elevateForwards);
	layout->addRow(tr("Move Down"), elevateBackwards);
	layout->addRow(tr("Move Right"), strafeRight);
	layout->addRow(tr("Move Left"), strafeLeft);
	layout->addRow(tr("Zoom In"), scaleUp);
	layout->addRow(tr("Zoom Out"), scaleDown);
	layout->addRow(tr("Pitch Up"), pitchUp);
	layout->addRow(tr("Pitch Down"), pitchDown);
	layout->addRow(tr("Rotate Right"), spinRight);
	layout->addRow(tr("Rotate left"), spinLeft);
	layout->addRow(QString(), reset);

	connect(increaseSpeed, &KeyBindEdit::editingFinished, this, [=]() {
		settings->setValue("ViewerGraphicsWindow/increase_speed", increaseSpeed->getSequence());
	});
	connect(decreaseSpeed, &KeyBindEdit::editingFinished, this, [=]() {
		settings->setValue("ViewerGraphicsWindow/decrease_speed", decreaseSpeed->getSequence());
	});
	connect(elevateForwards, &KeyBindEdit::editingFinished, this, [=]() {
		settings->setValue("ViewerGraphicsWindow/elevate_forwards", elevateForwards->getSequence());
	});
	connect(elevateBackwards, &KeyBindEdit::editingFinished, this, [=]() {
		settings->setValue("ViewerGraphicsWindow/elevate_backwards", elevateBackwards->getSequence());
	});
	connect(strafeLeft, &KeyBindEdit::editingFinished, this, [=]() {
		settings->setValue("ViewerGraphicsWindow/strafe_left", strafeLeft->getSequence());
	});
	connect(strafeRight, &KeyBindEdit::editingFinished, this, [=]() {
		settings->setValue("ViewerGraphicsWindow/strafe_right", strafeRight->getSequence());
	});
	connect(scaleUp, &KeyBindEdit::editingFinished, this, [=]() {
		settings->setValue("ViewerGraphicsWindow/scale_up", scaleUp->getSequence());
	});
	connect(scaleDown, &KeyBindEdit::editingFinished, this, [=]() {
		settings->setValue("ViewerGraphicsWindow/scale_down", scaleDown->getSequence());
	});
	connect(pitchUp, &KeyBindEdit::editingFinished, this, [=]() {
		settings->setValue("ViewerGraphicsWindow/pitch_up", pitchUp->getSequence());
	});
	connect(pitchDown, &KeyBindEdit::editingFinished, this, [=]() {
		settings->setValue("ViewerGraphicsWindow/pitch_down", pitchDown->getSequence());
	});
	connect(spinRight, &KeyBindEdit::editingFinished, this, [=]() {
		settings->setValue("ViewerGraphicsWindow/spin_right", spinRight->getSequence());
	});
	connect(spinLeft, &KeyBindEdit::editingFinished, this, [=]() {
		settings->setValue("ViewerGraphicsWindow/spin_left", spinLeft->getSequence());
	});
	connect(reset, &QPushButton::released, this, [=] {
		// Remove the saved values
		settings->remove("ViewerGraphicsWindow/increase_speed");
		settings->remove("ViewerGraphicsWindow/decrease_speed");
		settings->remove("ViewerGraphicsWindow/elevate_forwards");
		settings->remove("ViewerGraphicsWindow/elevate_backwards");
		settings->remove("ViewerGraphicsWindow/strafe_left");
		settings->remove("ViewerGraphicsWindow/strafe_right");
		settings->remove("ViewerGraphicsWindow/scale_up");
		settings->remove("ViewerGraphicsWindow/scale_down");
		settings->remove("ViewerGraphicsWindow/pitch_up");
		settings->remove("ViewerGraphicsWindow/pitch_down");
		settings->remove("ViewerGraphicsWindow/spin_right");
		settings->remove("ViewerGraphicsWindow/spin_left");

		// Reset the text feilds
		increaseSpeed->reset();
		decreaseSpeed->reset();
		elevateForwards->reset();
		elevateBackwards->reset();
		strafeLeft->reset();
		strafeRight->reset();
		scaleUp->reset();
		scaleDown->reset();
		pitchUp->reset();
		pitchDown->reset();
		spinRight->reset();
		spinLeft->reset();
	});

	m_pKeybindSettings->hide();// As it's not the defalut
}

void SettingsMenu::SetupViewSettings()
{
	m_pViewSettings = new QWidget(this);
	m_pViewSettings->setObjectName("viewsettings");
	QFormLayout* layout = new QFormLayout(m_pViewSettings);

	// Load button states
	QComboBox* msaa = new QComboBox();// Fix the text color
	msaa->insertItem(0, "Off",0);
	msaa->insertItem(1, "2x MSAA",2);
	msaa->insertItem(2, "4x MSAA",4);
	msaa->insertItem(3, "8x MSAA",8);
	msaa->insertItem(4, "16x MSAA",16);
	msaa->setCurrentIndex(int(sqrt(settings->value("ViewerGraphicsWindow/msaaLevel", 0).toInt())));
	msaa->setToolTip("Higher for better quality, lower for better preformance");
	QPushButton* toggleGrid = new QPushButton((settings->value("ViewerGraphicsWindow/toggleGrid", true).toBool()) ? "On" : "Off");
	toggleGrid->setObjectName("toggleGrid");
	QPushButton* toggleAxis = new QPushButton((settings->value("ViewerGraphicsWindow/toggleAxis", true).toBool()) ? "On" : "Off");
	toggleAxis->setObjectName("toggleAxis");
	QPushButton* toggleStats = new QPushButton((settings->value("ViewerGraphicsWindow/toggleStats", true).toBool()) ? "On" : "Off");
	toggleStats->setObjectName("toggleStats");
	QPushButton* reset = new QPushButton(QString::fromLatin1("Reset All"));
	reset->setObjectName("resetKeybinds");

	layout->addRow(tr("Anti Aliasing"), msaa);
	layout->addRow(tr("Refrence Grid"), toggleGrid);
	layout->addRow(tr("Axis Display"), toggleAxis);
	layout->addRow(tr("Stats for Nerds"), toggleStats);
	layout->addRow(QString(), reset);

	connect(msaa, QOverload<int>::of(&QComboBox::currentIndexChanged),[=](int index) { 
		settings->setValue("ViewerGraphicsWindow/msaaLevel", msaa->itemData(index).toInt());
	});
	connect(toggleGrid, &QPushButton::released, this, [=] {
		settings->setValue("ViewerGraphicsWindow/toggleGrid", !settings->value("ViewerGraphicsWindow/toggleGrid", true).toBool());
		toggleGrid->setText((settings->value("ViewerGraphicsWindow/toggleGrid", true).toBool()) ? "On" : "Off");
	});
	connect(toggleAxis, &QPushButton::released, this, [=] {
		settings->setValue("ViewerGraphicsWindow/toggleAxis", !settings->value("ViewerGraphicsWindow/toggleAxis", true).toBool());
		toggleAxis->setText((settings->value("ViewerGraphicsWindow/toggleAxis", true).toBool()) ? "On" : "Off");
	});
	connect(toggleStats, &QPushButton::released, this, [=] {
		settings->setValue("ViewerGraphicsWindow/toggleStats", !settings->value("ViewerGraphicsWindow/toggleStats", true).toBool());
		toggleStats->setText((settings->value("ViewerGraphicsWindow/toggleStats", true).toBool()) ? "On" : "Off");
	});
	connect(reset, &QPushButton::released, this, [=] {
		// Remove the saved values
		settings->remove("ViewerGraphicsWindow/toggleGrid");
		settings->remove("ViewerGraphicsWindow/toggleAxis");
		settings->remove("ViewerGraphicsWindow/toggleStats");
		settings->remove("ViewerGraphicsWindow/msaaLevel");

		toggleGrid->setText("On");
		toggleAxis->setText("On");
		toggleStats->setText("On");
	});

	
	m_pViewSettings->hide();// As it's not the defalut
}
