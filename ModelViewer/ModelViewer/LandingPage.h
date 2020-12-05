#pragma once
#include <QWidget>
#include <QGridLayout>
#include <QLabel>
#include <QSettings>
#include <QFormLayout>
#include <QPushButton>

// Forward Declares
class ViewerGraphicsWindow;

class LandingPage : public QWidget
{
	Q_OBJECT
public:
	LandingPage(ViewerGraphicsWindow* gWindow, QWidget* parent = nullptr);
	void setupPreviousFiles(bool first = false);

	//private slots:

private:
	ViewerGraphicsWindow* m_pGraphicsWindow = nullptr;
	QFormLayout* previousFilesLayout = nullptr;
	QGridLayout* m_pMainLayout = nullptr;
	QLabel* m_pWelcomeText = nullptr;
	QWidget* emptyWidget = nullptr;
	QSettings* settings;

	QPushButton* loadFile1 = nullptr;
	QPushButton* loadFile2 = nullptr;
	QPushButton* loadFile3 = nullptr;
	
};