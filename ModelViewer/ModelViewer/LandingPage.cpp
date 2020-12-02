#include "LandingPage.h"
#include "ViewerGraphicsWindow.h"

#include <QPushButton>
#include <QFileInfo>
#include <QLayoutItem>


LandingPage::LandingPage(ViewerGraphicsWindow* gWindow, QWidget* parent)
	: m_pGraphicsWindow(gWindow), QWidget(parent)
{
	// Build our objects
	settings = new QSettings("The Model Viewers team", "Model Viewer");
	m_pMainLayout = new QGridLayout(this);
	m_pMainLayout->setAlignment(Qt::AlignCenter);
	m_pWelcomeText = new QLabel("Welcome to Model Viewer!");
	QPushButton* pLoadButton = new QPushButton("Load New Model", this);
	previousFilesLayout = new QFormLayout();
	QLayoutItem* spacer = new QSpacerItem(10,60);
	emptyWidget = new QWidget();
	// Do a first-time setup of the menu
	setupPreviousFiles(true);
	
	// Set up the layout format
	m_pMainLayout->addWidget(m_pWelcomeText, 1, 0, 1, 3);
	m_pMainLayout->addWidget(pLoadButton, 3, 1, 1, 1);
	m_pMainLayout->addItem(spacer, 4,1,1,5);
	m_pMainLayout->addLayout(previousFilesLayout,6,0,1,2);

	// Connect up all the buttons
	connect(loadFile1, &QPushButton::pressed, this, [=] {m_pGraphicsWindow->loadModel(settings->value("LandingPage/file1", "").toString()); });
	connect(loadFile2, &QPushButton::pressed, this, [=] {m_pGraphicsWindow->loadModel(settings->value("LandingPage/file2", "").toString()); });
	connect(loadFile3, &QPushButton::pressed, this, [=] {m_pGraphicsWindow->loadModel(settings->value("LandingPage/file3", "").toString()); });
	connect(pLoadButton, &QPushButton::pressed, this, [=] {m_pGraphicsWindow->loadModel(); });
	connect(m_pGraphicsWindow, &ViewerGraphicsWindow::ModelUnloaded, this, [=] {setupPreviousFiles();});
	connect(m_pGraphicsWindow, &ViewerGraphicsWindow::EndModelLoading, this, [=] (bool ok, QString filepath) {
		if (ok) {
			settings->setValue("LandingPage/file3", settings->value("LandingPage/file2", "").toString());
			settings->setValue("LandingPage/file2", settings->value("LandingPage/file1", "").toString());
			settings->setValue("LandingPage/file1", filepath);
		}
	});

	// Setup style scheets
	this->setStyleSheet("alignment: center");
	m_pWelcomeText->setStyleSheet("font-size: 32px");
	pLoadButton->setStyleSheet("font-size: 32px");

	this->setLayout(m_pMainLayout);
}


void LandingPage::setupPreviousFiles(bool first) {
	// Load the 3 previous files
	QString file1 = settings->value("LandingPage/file1", "").toString();
	QString file2 = settings->value("LandingPage/file2", "").toString();
	QString file3 = settings->value("LandingPage/file3", "").toString();

	QPushButton* loadFile1 = new QPushButton("Load Model");
	loadFile1->setMaximumWidth(80);
	QPushButton* loadFile2 = new QPushButton("Load Model");
	loadFile2->setMaximumWidth(80);
	QPushButton* loadFile3 = new QPushButton("Load Model");
	loadFile3->setMaximumWidth(80);

	// Remove old entries if it's not the first time
	if (!first) {
		if (!file3.isEmpty()) {
			previousFilesLayout->removeRow(3);
		}
		if (!file2.isEmpty()) {
			previousFilesLayout->removeRow(2);
		}
		if (!file1.isEmpty()) {
			previousFilesLayout->removeRow(1);
		}
	}
	
	// Set up new entries
	if (!file1.isEmpty()) {
		if (first) {
			previousFilesLayout->addRow(QString::fromLatin1("Previous files:"), emptyWidget);
		}
		previousFilesLayout->addRow(QFileInfo(file1).fileName(), loadFile1);
	}
	if (!file2.isEmpty()) {
		previousFilesLayout->addRow(QFileInfo(file2).fileName(), loadFile2);
	}
	if (!file3.isEmpty()) {
		previousFilesLayout->addRow(QFileInfo(file3).fileName(), loadFile3);
	}
	
	connect(loadFile1, &QPushButton::pressed, this, [=] {m_pGraphicsWindow->loadModel(settings->value("LandingPage/file1", "").toString()); });
	connect(loadFile2, &QPushButton::pressed, this, [=] {m_pGraphicsWindow->loadModel(settings->value("LandingPage/file2", "").toString()); });
	connect(loadFile3, &QPushButton::pressed, this, [=] {m_pGraphicsWindow->loadModel(settings->value("LandingPage/file3", "").toString()); });
}
