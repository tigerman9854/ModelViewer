#include "GraphicsWindowDelegate.h"
#include "ViewerGraphicsWindow.h"

#include <QGridLayout>
#include <QLabel>
#include <QFont>
#include <QFileInfo>
#include <QPushButton>

GraphicsWindowDelegate::GraphicsWindowDelegate(ViewerGraphicsWindow* graphicsWindow, QWidget* parent)
	: m_pGraphicsWindow(graphicsWindow), QWidget(parent)
{
	setContentsMargins(0, 0, 0, 0);

	m_pMainLayout = new QGridLayout(this);
	m_pMainLayout->setMargin(0);


	auto styleWidget = [](QWidget* pWidget) {
		QPalette pal = pWidget->palette();
		pal.setColor(QPalette::Background, Qt::black);
		pal.setColor(QPalette::WindowText, Qt::white);
		pWidget->setPalette(pal);
		pWidget->setAutoFillBackground(true);

		QFont font = pWidget->font();
		font.setPointSize(24);
		pWidget->setFont(font);
	};

	// Empty screen (i.e. before any model has been loaded)
	m_pEmptyWidget = new QWidget(); 
	QGridLayout* pEmptyLayout = new QGridLayout(m_pEmptyWidget);
	pEmptyLayout->setAlignment(Qt::AlignCenter);
	m_pEmptyText = new QLabel("Welcome to Model Viewer!");
	QPushButton* pLoadButton = new QPushButton("Load Model", m_pEmptyWidget);
	connect(pLoadButton, &QPushButton::pressed, this, [=] {m_pGraphicsWindow->loadModel(); });

	pEmptyLayout->setRowStretch(0, 5);
	pEmptyLayout->addWidget(m_pEmptyText, 1, 0, 1, 3);
	pEmptyLayout->setRowStretch(2, 1);
	pEmptyLayout->addWidget(pLoadButton, 3, 1, 1, 1);
	pEmptyLayout->setRowStretch(4, 5);
	m_pEmptyWidget->setLayout(pEmptyLayout);

	// Error screen, shown when there is an issue loading
	m_pErrorWidget = new QWidget();
	QGridLayout* pErrorLayout = new QGridLayout(m_pErrorWidget);
	pErrorLayout->setAlignment(Qt::AlignCenter);
	m_pErrorText = new QLabel("Error");
	pErrorLayout->addWidget(m_pErrorText);
	m_pErrorWidget->setLayout(pErrorLayout);

	// Loading screen, shown during synchronous model loading
	m_pLoadingWidget = new QWidget();
	QGridLayout* pLoadingLayout = new QGridLayout(m_pLoadingWidget);
	pLoadingLayout->setAlignment(Qt::AlignCenter);
	m_pLoadingText = new QLabel("Loading");
	pLoadingLayout->addWidget(m_pLoadingText);
	m_pLoadingWidget->setLayout(pLoadingLayout);

	// Setup the style for placeholder widgets
	styleWidget(m_pEmptyWidget);
	styleWidget(m_pErrorWidget);
	styleWidget(m_pLoadingWidget);


	// OpenGL screen, shown when viewing a model
	m_pModelWidget = graphicsWindow;

	// Start by showing the graphics window briefly so the OpenGl context can be 
	// initialized.  It will automatically be swapped with the empty screen
	// once loaded.
	m_pCurrentWidget = m_pModelWidget;
	m_pMainLayout->addWidget(m_pCurrentWidget);
	m_status = Status::k_model;


	// Connect to signals from the graphics window
	connect(m_pGraphicsWindow, &ViewerGraphicsWindow::Initialized, this, &GraphicsWindowDelegate::OnViewerInitialized);
	connect(m_pGraphicsWindow, &ViewerGraphicsWindow::BeginModelLoading, this, &GraphicsWindowDelegate::OnBeginModelLoading);
	connect(m_pGraphicsWindow, &ViewerGraphicsWindow::EndModelLoading, this, &GraphicsWindowDelegate::OnEndModelLoading);
	connect(m_pGraphicsWindow, &ViewerGraphicsWindow::Error, this, &GraphicsWindowDelegate::OnError);
	connect(m_pGraphicsWindow, &ViewerGraphicsWindow::ClearError, this, &GraphicsWindowDelegate::OnClearError);
	connect(m_pGraphicsWindow, &ViewerGraphicsWindow::ModelUnloaded, this, &GraphicsWindowDelegate::OnModelUnloaded);
}

void GraphicsWindowDelegate::SetStatus(Status s)
{
	if (m_status == s) {
		return;
	}

	m_lastStatus = m_status;
	m_status = s;
	DoOnStatusChanged();
}

GraphicsWindowDelegate::Status GraphicsWindowDelegate::GetStatus() {
	return m_status;
}

void GraphicsWindowDelegate::DoOnStatusChanged() {
	auto swapCurrentWidget = [=] (QWidget* toSwap) {
		if (m_pCurrentWidget == toSwap) {
			return;
		}

		m_pMainLayout->replaceWidget(m_pCurrentWidget, toSwap);
		toSwap->show();
		m_pCurrentWidget->hide();
		m_pCurrentWidget = toSwap;
	};

	switch (m_status) {
	case Status::k_empty:
		swapCurrentWidget(m_pEmptyWidget);
		break;
	case Status::k_model:
		swapCurrentWidget(m_pModelWidget);
		break;
	case Status::k_error:
		swapCurrentWidget(m_pErrorWidget);
		break;
	case Status::k_loading:
		swapCurrentWidget(m_pLoadingWidget);
		break;
	}

	// Force repaint so the loading screen will be visible before performing a
	// long synchronous load
	repaint();
}

void GraphicsWindowDelegate::OnBeginModelLoading(QString filepath)
{
	// Change the loading text
	QFileInfo fileInfo(filepath);
	QString text = QString("Loading %1...").arg(fileInfo.fileName());
	m_pLoadingText->setText(text);
	
	SetStatus(Status::k_loading);
}

void GraphicsWindowDelegate::OnViewerInitialized()
{
	// When the viewer is initialized (i.e. the OpenGL context created),
	// revert back to the empty screen widget. The OpenGL context must be created
	// in order for models to be loaded. 
	SetStatus(Status::k_empty);
}

void GraphicsWindowDelegate::OnEndModelLoading(bool success, QString filepath)
{
	if (success) {
		SetStatus(Status::k_model);
	}
	else {
		QFileInfo fileInfo(filepath);
		QString text = QString("Failed to load %1.").arg(fileInfo.fileName());
		OnError(text);
	}
}

void GraphicsWindowDelegate::OnError(QString message)
{
	// Change the error text
	m_pErrorText->setText("Error: " + message);
	SetStatus(Status::k_error);
}

void GraphicsWindowDelegate::OnClearError()
{
	if (m_status == Status::k_error)
	{
		SetStatus(m_lastStatus);
	}
}

void GraphicsWindowDelegate::OnModelUnloaded()
{
	SetStatus(Status::k_empty);
}