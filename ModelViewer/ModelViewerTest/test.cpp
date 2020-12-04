#include <QtTest/QtTest>
#include <QMenu>
#include <QMenuBar>
#include <QSignalSpy>
#include <QlineEdit>
#include <QPushButton>
#include <QEvent>
#include <QMouseEvent>

#include "ModelViewer.h"
#include "ModelLoader.h"
#include "ViewerGraphicsWindow.h"
#include "GraphicsWindowDelegate.h"
#include "KeyBindEdit.h"
#include "KeySequenceParse.h"

class ModelViewerTest : public QObject {
	Q_OBJECT

private slots:
	void initTestCase();
	void cleanupTestCase();
	void init();
	void cleanup();

	// Add tests here
	void integration();

	void invalidFile();
	void loadingPage();
	void unloadModel();

	void testShow();
	void loadModel();
	void loadShader();
	void loadCurrentShaders();
	void editCurrentShaders();
	void openShaderFile();
	void displayModel();
	void resetView();
	void rotateWithMouse();
	void panWithMouse();
	void checkMousePressAndRelease();
	void zoomWithMouse();
	void defaultZoom();
	void testKeyboard();
	void testSettings();
	void changeColorUniforms();

private:
	// Helpers
	void ResetViewAndShow();


	ModelViewer* m_pWindow = nullptr;
	QMatrix4x4 resetMatrix;

	// Enable this setting to skip tests which require user input
	bool disableAnnoyingTests = true;
};


void ModelViewerTest::initTestCase() 
{
	// Called once before all test cases
	m_pWindow = new ModelViewer();
	resetMatrix = m_pWindow->GetGraphicsWindow()->GetModelMatrix();

	if (disableAnnoyingTests) {
		printf("WARNING: Annoying tests disabled.\n");
	}
}

void ModelViewerTest::cleanupTestCase()
{
	// Called once after all test casese
	delete m_pWindow;
}

void ModelViewerTest::init()
{
	// Called before each test case
	m_pWindow->hide();
}

void ModelViewerTest::cleanup()
{
	// Called after each test case
}


// Helpers
void ModelViewerTest::ResetViewAndShow()
{
	m_pWindow->show();
	m_pWindow->GetGraphicsWindow()->resetView();
	resetMatrix = m_pWindow->GetGraphicsWindow()->GetModelMatrix();
}


void ModelViewerTest::integration()
{
	// Show the window, make sure it appeared
	QVERIFY(m_pWindow->isHidden());
	m_pWindow->show();
	QTest::qWait(100);
	QVERIFY(m_pWindow->isHidden() == false);

	// Load a primitive through the file menu
	QMenu* pFileMenu = m_pWindow->findChild<QMenu*>("FileMenu");
	QVERIFY(pFileMenu);
	QMenu* pLoadMenu = m_pWindow->findChild<QMenu*>("LoadMenu");
	QVERIFY(pLoadMenu);
	QMenu* pPrimitiveMenu = m_pWindow->findChild<QMenu*>("PrimitiveMenu");
	QVERIFY(pPrimitiveMenu);
	QMenuBar* pMenuBar = m_pWindow->menuBar();
	QVERIFY(pMenuBar);
	pFileMenu->popup(m_pWindow->mapToGlobal(pMenuBar->pos()));
	pFileMenu->setFocus();
	QTest::qWait(20);
	QTest::keyClick(pFileMenu, Qt::Key_Down, Qt::NoModifier, 20);
	QTest::keyClick(pFileMenu, Qt::Key_Enter, Qt::NoModifier, 20);
	QTest::keyClick(pLoadMenu, Qt::Key_Down, Qt::NoModifier, 20);
	QTest::keyClick(pLoadMenu, Qt::Key_Down, Qt::NoModifier, 20);
	QTest::keyClick(pLoadMenu, Qt::Key_Enter, Qt::NoModifier, 20);
	QTest::keyClick(pPrimitiveMenu, Qt::Key_Down, Qt::NoModifier, 20);
	QTest::keyClick(pPrimitiveMenu, Qt::Key_Down, Qt::NoModifier, 20);
	QTest::keyClick(pPrimitiveMenu, Qt::Key_Down, Qt::NoModifier, 20);
	QTest::keyClick(pPrimitiveMenu, Qt::Key_Down, Qt::NoModifier, 20);
	QTest::keyClick(pPrimitiveMenu, Qt::Key_Enter, Qt::NoModifier, 20);

	// Check that the shape was loaded
	GraphicsWindowDelegate* pGraphicsDelegate = m_pWindow->GetGraphicsDelegate();
	QVERIFY(pGraphicsDelegate->GetStatus() == GraphicsWindowDelegate::Status::k_model);

	// Rotate the camera
	QTest::mousePress(m_pWindow->GetGraphicsWindow(), Qt::LeftButton);
	QMouseEvent event(QEvent::MouseMove, QPoint(3, 3), Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
	QApplication::sendEvent(m_pWindow->GetGraphicsWindow(), &event);
	QTest::mouseRelease(m_pWindow->GetGraphicsWindow(), Qt::LeftButton, { 0 }, QPoint(3, 3));

	QVERIFY(m_pWindow->GetGraphicsWindow()->GetModelMatrix() != resetMatrix);
	QTest::qWait(100);


	// Pan the camera
	m_pWindow->GetGraphicsWindow()->resetView();

	QTest::mousePress(m_pWindow->GetGraphicsWindow(), Qt::RightButton, { 0 }, QPoint(0, 0));
	event = QMouseEvent(QEvent::MouseMove, QPoint(150, 150), Qt::RightButton, Qt::RightButton, Qt::NoModifier);
	QApplication::sendEvent(m_pWindow->GetGraphicsWindow(), &event);
	QTest::mouseRelease(m_pWindow->GetGraphicsWindow(), Qt::RightButton, { 0 }, QPoint(150, 150));

	QVERIFY(m_pWindow->GetGraphicsWindow()->GetModelMatrix() != resetMatrix);
	QTest::qWait(100);

	// Unload the shape
	pFileMenu->popup(m_pWindow->mapToGlobal(pMenuBar->pos()));
	pFileMenu->setFocus();
	QTest::keyClick(pFileMenu, Qt::Key_Down, Qt::NoModifier, 20);
	QTest::keyClick(pFileMenu, Qt::Key_Down, Qt::NoModifier, 20);
	QTest::keyClick(pFileMenu, Qt::Key_Down, Qt::NoModifier, 20);
	QTest::keyClick(pFileMenu, Qt::Key_Down, Qt::NoModifier, 20);
	QTest::keyClick(pFileMenu, Qt::Key_Enter, Qt::NoModifier, 20);

	// Check that the status is updated
	QVERIFY(pGraphicsDelegate->GetStatus() == GraphicsWindowDelegate::Status::k_empty);

	QTest::qWait(100);

	m_pWindow->hide();
	QVERIFY(m_pWindow->isHidden());
}

void ModelViewerTest::invalidFile()
{
	ViewerGraphicsWindow* pGraphicsWindow = m_pWindow->GetGraphicsWindow();
	GraphicsWindowDelegate* pGraphicsDelegate = m_pWindow->GetGraphicsDelegate();

	QSignalSpy beginLoadSignalSpy(pGraphicsWindow, SIGNAL(BeginModelLoading(QString)));
	QSignalSpy endLoadSignalSpy(pGraphicsWindow, SIGNAL(EndModelLoading(bool, QString)));

	QString path("../Data/Models/Avent.mtl");
	m_pWindow->GetGraphicsWindow()->loadModel(path);

	// Ensure the correct signals were sent
	QCOMPARE(beginLoadSignalSpy.count(), 1);
	QCOMPARE(endLoadSignalSpy.count(), 1);

	auto arguments = endLoadSignalSpy.takeFirst();
	QVERIFY(arguments.at(0).toBool() == false);

	// Check that the status is updated
	QVERIFY(pGraphicsDelegate->GetStatus() == GraphicsWindowDelegate::Status::k_error);
}

void ModelViewerTest::loadingPage()
{
	ViewerGraphicsWindow* pGraphicsWindow = m_pWindow->GetGraphicsWindow();
	GraphicsWindowDelegate* pGraphicsDelegate = m_pWindow->GetGraphicsDelegate();

	pGraphicsWindow->unloadModel();

	// Check the status before
	QVERIFY(pGraphicsDelegate->GetStatus() == GraphicsWindowDelegate::Status::k_empty);

	QSignalSpy beginLoadSignalSpy(pGraphicsWindow, SIGNAL(BeginModelLoading(QString)));
	QSignalSpy endLoadSignalSpy(pGraphicsWindow, SIGNAL(EndModelLoading(bool, QString)));

	QString path("../Data/Models/cubeColor.ply");
	m_pWindow->GetGraphicsWindow()->loadModel(path);

	// Ensure the correct signals were sent
	QCOMPARE(beginLoadSignalSpy.count(), 1);
	QCOMPARE(endLoadSignalSpy.count(), 1);

	auto arguments = endLoadSignalSpy.takeFirst();
	QVERIFY(arguments.at(0).toBool() == true);
	
	// Check that the status is updated
	QVERIFY(pGraphicsDelegate->GetStatus() == GraphicsWindowDelegate::Status::k_model);
}

void ModelViewerTest::unloadModel()
{
	ViewerGraphicsWindow* pGraphicsWindow = m_pWindow->GetGraphicsWindow();
	GraphicsWindowDelegate* pGraphicsDelegate = m_pWindow->GetGraphicsDelegate();

	// Load model
	QString path("../Data/Models/cubeColor.ply");
	m_pWindow->GetGraphicsWindow()->loadModel(path);

	// Check the status after
	QVERIFY(pGraphicsDelegate->GetStatus() == GraphicsWindowDelegate::Status::k_model);

	pGraphicsWindow->unloadModel();

	// Check the status after
	QVERIFY(pGraphicsDelegate->GetStatus() == GraphicsWindowDelegate::Status::k_empty);
}

void ModelViewerTest::testShow()
{
	QVERIFY(m_pWindow->isHidden());
	m_pWindow->show();
	QTest::qWait(100);
	QVERIFY(m_pWindow->isHidden() == false);
	m_pWindow->hide();
}

void ModelViewerTest::loadModel()
{
	ModelLoader m;
	Model loaded = m.LoadModel("../Data/Primitives/cube.obj");
	QVERIFY(loaded.m_isValid);
}

void ModelViewerTest::loadShader()
{
	bool success = m_pWindow->GetGraphicsWindow()->loadVertexShader("../Data/Shaders/ads.vert");
	QVERIFY(success);
	success = m_pWindow->GetGraphicsWindow()->loadFragmentShader("../Data/Shaders/ads.frag");
	QVERIFY(success);
}

void ModelViewerTest::loadCurrentShaders()
{
	bool success = m_pWindow->GetGraphicsWindow()->reloadCurrentShaders();
	QVERIFY(success);
}

void ModelViewerTest::editCurrentShaders()
{
	if (disableAnnoyingTests) {
		return;
	}

	bool success = m_pWindow->GetGraphicsWindow()->editCurrentShaders();
	QVERIFY(success);
}

void ModelViewerTest::openShaderFile()
{
	if (disableAnnoyingTests) {
		return;
	}

	bool success = m_pWindow->GetGraphicsWindow()->openShaderFile("../Data/Shaders/basic.frag");
	QVERIFY(success);
}

void ModelViewerTest::changeColorUniforms()
{
	m_pWindow->show();
	m_pWindow->GetGraphicsWindow()->colorRChanged(255);
	m_pWindow->GetGraphicsWindow()->colorGChanged(255);
	m_pWindow->GetGraphicsWindow()->colorBChanged(255);
	QVERIFY(m_pWindow->GetGraphicsWindow()->getADColor()[0] == 1.0
		&& m_pWindow->GetGraphicsWindow()->getADColor()[1] == 1.0
		&& m_pWindow->GetGraphicsWindow()->getADColor()[2] == 1.0);

	m_pWindow->GetGraphicsWindow()->lightAmbient(0.20);
	m_pWindow->GetGraphicsWindow()->lightDiffuse(0.40);
	m_pWindow->GetGraphicsWindow()->lightSpecular(0.30);
	m_pWindow->hide();
	QVERIFY(m_pWindow->GetGraphicsWindow()->getADS()[0] > 0.18 && m_pWindow->GetGraphicsWindow()->getADS()[0] < 0.22);
	QVERIFY(m_pWindow->GetGraphicsWindow()->getADS()[1] > 0.38 && m_pWindow->GetGraphicsWindow()->getADS()[1] < 0.42);
	QVERIFY(m_pWindow->GetGraphicsWindow()->getADS()[2] > 0.28 && m_pWindow->GetGraphicsWindow()->getADS()[2] < 0.32);

}

void ModelViewerTest::displayModel()
{
	m_pWindow->show();
	bool success = m_pWindow->GetGraphicsWindow()->loadModel("../Data/Primitives/cube.obj");
	QVERIFY(success);
	success = m_pWindow->GetGraphicsWindow()->loadModel("../Data/Models/cubeColor.ply");
	QVERIFY(success);
	success = m_pWindow->GetGraphicsWindow()->loadModel("../Data/Primitives/Dodecahedron.stl");
	QVERIFY(success);
	success = m_pWindow->GetGraphicsWindow()->loadModel("../Data/Primitives/Icosahedron.stl");
	QVERIFY(success);
	QTest::qWait(100);
	QVERIFY(success);
	m_pWindow->hide();
}

void ModelViewerTest::resetView()
{
	ResetViewAndShow();

	// Check that after altering it we end w/ an idenity matrix
	QTest::mousePress(m_pWindow->GetGraphicsWindow(), Qt::RightButton);
	QMouseEvent event(QEvent::MouseMove, QPoint(5, 5), Qt::RightButton, Qt::RightButton, Qt::NoModifier);
	QApplication::sendEvent(m_pWindow->GetGraphicsWindow(), &event);
	QTest::mouseRelease(m_pWindow->GetGraphicsWindow(), Qt::RightButton, { 0 }, QPoint(3, 3));

	// Reset the view
	m_pWindow->GetGraphicsWindow()->resetView();

	QVERIFY(m_pWindow->GetGraphicsWindow()->GetModelMatrix() == resetMatrix);
	m_pWindow->hide();
}

void ModelViewerTest::rotateWithMouse()
{
	ResetViewAndShow();

	// Test rotating
	QTest::mousePress(m_pWindow->GetGraphicsWindow(), Qt::LeftButton, { 0 }, QPoint(50, 50), 20);
	QMouseEvent event(QEvent::MouseMove, QPoint(60, 60), Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
	QApplication::sendEvent(m_pWindow->GetGraphicsWindow(), &event);
	QTest::mouseRelease(m_pWindow->GetGraphicsWindow(), Qt::LeftButton, { 0 }, QPoint(60, 60), 20);
	QTest::qWait(100);

	QVERIFY(m_pWindow->GetGraphicsWindow()->GetModelMatrix() != resetMatrix);
	m_pWindow->hide();
}


void ModelViewerTest::panWithMouse()
{
	ResetViewAndShow();

	// Test Pan
	QTest::mousePress(m_pWindow->GetGraphicsWindow(), Qt::RightButton);
	QMouseEvent event(QEvent::MouseMove, QPoint(10, 10), Qt::RightButton, Qt::RightButton, Qt::NoModifier);
	QApplication::sendEvent(m_pWindow->GetGraphicsWindow(), &event);
	QTest::mouseRelease(m_pWindow->GetGraphicsWindow(), Qt::RightButton, { 0 }, QPoint(10, 10)); // Release the mouse at (10, 10)

	// Wait for new frame to be drawn
	QTest::qWait(100);

	QVERIFY(m_pWindow->GetGraphicsWindow()->GetModelMatrix() != resetMatrix);
	m_pWindow->hide();
}

void ModelViewerTest::checkMousePressAndRelease()
{
	ResetViewAndShow();

	// Test right click
	QTest::mousePress(m_pWindow->GetGraphicsWindow(), Qt::RightButton);
	QVERIFY(m_pWindow->GetGraphicsWindow()->GetRightMousePressed());
	QTest::mouseRelease(m_pWindow->GetGraphicsWindow(), Qt::RightButton);
	QVERIFY(!m_pWindow->GetGraphicsWindow()->GetRightMousePressed());

	// Test left click
	QTest::mousePress(m_pWindow->GetGraphicsWindow(), Qt::LeftButton);
	QVERIFY(m_pWindow->GetGraphicsWindow()->GetLeftMousePressed());
	QTest::mouseRelease(m_pWindow->GetGraphicsWindow(), Qt::LeftButton);
	QVERIFY(!m_pWindow->GetGraphicsWindow()->GetLeftMousePressed());
	m_pWindow->hide();
}

void ModelViewerTest::zoomWithMouse()
{
	ResetViewAndShow();

	//https://stackoverflow.com/questions/50996997/how-to-simulate-mouse-wheel-events-using-qtestlib-qt5
	ViewerGraphicsWindow* test = m_pWindow->GetGraphicsWindow();
	test->SetScale(.5 * test->zoomSensitivity);
	QVERIFY(test->GetModelMatrix() != resetMatrix);
}

void ModelViewerTest::defaultZoom()
{
	ResetViewAndShow();

	// Load a different model
	bool success = m_pWindow->GetGraphicsWindow()->loadModel("../Data/Models/cubeColor.ply");
	QVERIFY(success);
	
	// Check that the scene was resized to show this model
	ViewerGraphicsWindow* test = m_pWindow->GetGraphicsWindow();
	QVERIFY(test->GetModelMatrix() != resetMatrix);
}

void ModelViewerTest::testKeyboard() 
{
	ResetViewAndShow();

	// Make sure all the keybinds are reset
	QPushButton* resetKeybinds = m_pWindow->GetSettingsWindow()->findChild<QPushButton*>("resetKeybinds");
	resetKeybinds->released();

	ViewerGraphicsWindow* pGraphicsWindow = m_pWindow->GetGraphicsWindow();

	// Load a model
	bool success = m_pWindow->GetGraphicsWindow()->loadModel("../Data/Models/cubeColor.ply");
	QVERIFY(success);

	auto testKey = [=](int key) {
		ResetViewAndShow();
		pGraphicsWindow->setFocus();
		QTest::qWait(10);

		QTest::keyPress(pGraphicsWindow, key);
		QTest::qWait(100);
		pGraphicsWindow->update();
		QTest::keyRelease(pGraphicsWindow, key);

		QVERIFY(pGraphicsWindow->GetModelMatrix() != resetMatrix);
	};

	testKey(Qt::Key_W);
	testKey(Qt::Key_A);
	testKey(Qt::Key_S);
	testKey(Qt::Key_D);
	testKey(Qt::Key_Q);
	testKey(Qt::Key_E);
	testKey(Qt::Key_Up);
	testKey(Qt::Key_Down);

	QTest::keyPress(pGraphicsWindow, Qt::Key_Shift);
	testKey(Qt::Key_W);
	testKey(Qt::Key_A);
	testKey(Qt::Key_S);
	testKey(Qt::Key_D);
	testKey(Qt::Key_Q);
	testKey(Qt::Key_E);
	testKey(Qt::Key_Up);
	testKey(Qt::Key_Down);
	QTest::keyRelease(pGraphicsWindow, Qt::Key_Shift);

	QTest::keyPress(pGraphicsWindow, Qt::Key_Alt);
	testKey(Qt::Key_W);
	testKey(Qt::Key_A);
	testKey(Qt::Key_S);
	testKey(Qt::Key_D);
	testKey(Qt::Key_Q);
	testKey(Qt::Key_E);
	testKey(Qt::Key_Up);
	testKey(Qt::Key_Down);
	QTest::keyRelease(pGraphicsWindow, Qt::Key_Alt);
}

void ModelViewerTest::testSettings()
{
	ResetViewAndShow();

	ViewerGraphicsWindow* pGraphicsWindow = m_pWindow->GetGraphicsWindow();
	SettingsMenu* pSettingsWindow = m_pWindow->GetSettingsWindow();

	QPushButton* resetMouse = pSettingsWindow->findChild<QPushButton*>("resetMouseSettings");
	QPushButton* resetKeybinds = m_pWindow->GetSettingsWindow()->findChild<QPushButton*>("resetKeybinds");
	resetMouse->released();
	resetKeybinds->released();

	// Set & check all of the sensitivity values
	QLineEdit* panXSensitivity = pSettingsWindow->findChild<QLineEdit*>("panXSensitivity");
	panXSensitivity->setText(QString::number(0.1));
	panXSensitivity->editingFinished();
	QLineEdit* panYSensitivity = pSettingsWindow->findChild<QLineEdit*>("panYSensitivity");
	panYSensitivity->setText(QString::number(0.1));
	panYSensitivity->editingFinished();
	QLineEdit* xRotateSensitivity = pSettingsWindow->findChild<QLineEdit*>("xRotateSensitivity");
	xRotateSensitivity->setText(QString::number(0.1));
	xRotateSensitivity->editingFinished();
	QLineEdit* yRotateSensitivity = pSettingsWindow->findChild<QLineEdit*>("yRotateSensitivity");
	yRotateSensitivity->setText(QString::number(0.1));
	yRotateSensitivity->editingFinished();
	QLineEdit* movementSensitivity = pSettingsWindow->findChild<QLineEdit*>("movementSensitivity");
	movementSensitivity->setText(QString::number(0.1));
	movementSensitivity->editingFinished();
	QLineEdit* zoomSensitivity = pSettingsWindow->findChild<QLineEdit*>("zoomSensitivity");
	zoomSensitivity->setText(QString::number(0.1));
	zoomSensitivity->editingFinished();
	QLineEdit* fieldOfView = pSettingsWindow->findChild<QLineEdit*>("fieldOfView");
	fieldOfView->setText(QString::number(0.1));
	fieldOfView->editingFinished();
	QLineEdit* nearPlane = pSettingsWindow->findChild<QLineEdit*>("nearPlane");
	nearPlane->setText(QString::number(0.1));
	nearPlane->editingFinished();
	QLineEdit* farPlane = pSettingsWindow->findChild<QLineEdit*>("farPlane");
	farPlane->setText(QString::number(0.1));
	farPlane->editingFinished();

	QVERIFY(pGraphicsWindow->panXSensitivity == 0.1f);
	QVERIFY(pGraphicsWindow->panYSensitivity == 0.1f);
	QVERIFY(pGraphicsWindow->xRotateSensitivity == 0.1f);
	QVERIFY(pGraphicsWindow->yRotateSensitivity == 0.1f);
	QVERIFY(pGraphicsWindow->movementSensitivity == 0.1f);
	QVERIFY(pGraphicsWindow->zoomSensitivity == 0.1f);
	QVERIFY(pGraphicsWindow->fieldOfView == 0.1f);
	QVERIFY(pGraphicsWindow->nearPlane == 0.1f);
	QVERIFY(pGraphicsWindow->farPlane == 0.1f);

	resetMouse->released();

	// Set & check all of the keybind values
	KeyBindEdit* increaseSpeed = pSettingsWindow->findChild<KeyBindEdit*>("increaseSpeed");
	QTest::keyClick(increaseSpeed, Qt::Key::Key_Escape);
	QTest::keyClick(increaseSpeed, Qt::Key::Key_Y);
	increaseSpeed->editingFinished();
	KeyBindEdit* decreaseSpeed = pSettingsWindow->findChild<KeyBindEdit*>("decreaseSpeed");
	QTest::keyClick(decreaseSpeed, Qt::Key::Key_Escape);
	QTest::keyClick(decreaseSpeed, Qt::Key::Key_P);
	decreaseSpeed->editingFinished();
	KeyBindEdit* elevateForwards = pSettingsWindow->findChild<KeyBindEdit*>("elevateForwards");
	QTest::keyClick(elevateForwards, Qt::Key::Key_Escape);
	QTest::keyClick(elevateForwards, Qt::Key::Key_I);
	elevateForwards->editingFinished();
	KeyBindEdit* elevateBackwards = pSettingsWindow->findChild<KeyBindEdit*>("elevateBackwards");
	QTest::keyClick(elevateBackwards, Qt::Key::Key_Escape);
	QTest::keyClick(elevateBackwards, Qt::Key::Key_K);
	elevateBackwards->editingFinished();
	KeyBindEdit* strafeLeft = pSettingsWindow->findChild<KeyBindEdit*>("strafeLeft");
	QTest::keyClick(strafeLeft, Qt::Key::Key_Escape);
	QTest::keyClick(strafeLeft, Qt::Key::Key_J);
	strafeLeft->editingFinished();
	KeyBindEdit* strafeRight = pSettingsWindow->findChild<KeyBindEdit*>("strafeRight");
	QTest::keyClick(strafeRight, Qt::Key::Key_Escape);
	QTest::keyClick(strafeRight, Qt::Key::Key_L);
	strafeRight->editingFinished();
	KeyBindEdit* scaleUp = pSettingsWindow->findChild<KeyBindEdit*>("scaleUp");
	QTest::keyClick(scaleUp, Qt::Key::Key_Escape);
	QTest::keyClick(scaleUp, Qt::Key::Key_O);
	scaleUp->editingFinished();
	KeyBindEdit* scaleDown = pSettingsWindow->findChild<KeyBindEdit*>("scaleDown");
	QTest::keyClick(scaleDown, Qt::Key::Key_Escape);
	QTest::keyClick(scaleDown, Qt::Key::Key_U);
	scaleDown->editingFinished();
	KeyBindEdit* pitchUp = pSettingsWindow->findChild<KeyBindEdit*>("pitchUp");
	QTest::keyClick(pitchUp, Qt::Key::Key_Escape);
	QTest::keyClick(pitchUp, Qt::Key::Key_W);
	pitchUp->editingFinished();
	KeyBindEdit* pitchDown = pSettingsWindow->findChild<KeyBindEdit*>("pitchDown");
	QTest::keyClick(pitchDown, Qt::Key::Key_Escape);
	QTest::keyClick(pitchDown, Qt::Key::Key_S);
	pitchDown->editingFinished();
	KeyBindEdit* spinRight = pSettingsWindow->findChild<KeyBindEdit*>("spinRight");
	QTest::keyClick(spinRight, Qt::Key::Key_Escape);
	QTest::keyClick(spinRight, Qt::Key::Key_D);
	spinRight->editingFinished();
	KeyBindEdit* spinLeft = pSettingsWindow->findChild<KeyBindEdit*>("spinLeft");
	QTest::keyClick(spinLeft, Qt::Key::Key_Escape);
	QTest::keyClick(spinLeft, Qt::Key::Key_A);
	spinLeft->editingFinished();

	QVERIFY(pSettingsWindow->getSettings()->value("ViewerGraphicsWindow/increase_speed").toString() == QString(Qt::Key::Key_Y));
	QVERIFY(pSettingsWindow->getSettings()->value("ViewerGraphicsWindow/decrease_speed").toString() == QString(Qt::Key::Key_P));
	QVERIFY(pSettingsWindow->getSettings()->value("ViewerGraphicsWindow/elevate_forwards").toString() == QString(Qt::Key::Key_I));
	QVERIFY(pSettingsWindow->getSettings()->value("ViewerGraphicsWindow/elevate_backwards").toString() == QString(Qt::Key::Key_K));
	QVERIFY(pSettingsWindow->getSettings()->value("ViewerGraphicsWindow/strafe_left").toString() == QString(Qt::Key::Key_J));
	QVERIFY(pSettingsWindow->getSettings()->value("ViewerGraphicsWindow/strafe_right").toString() == QString(Qt::Key::Key_L));
	QVERIFY(pSettingsWindow->getSettings()->value("ViewerGraphicsWindow/scale_up").toString() == QString(Qt::Key::Key_O));
	QVERIFY(pSettingsWindow->getSettings()->value("ViewerGraphicsWindow/scale_down").toString() == QString(Qt::Key::Key_U));
	QVERIFY(pSettingsWindow->getSettings()->value("ViewerGraphicsWindow/pitch_up").toString() == QString(Qt::Key::Key_W));
	QVERIFY(pSettingsWindow->getSettings()->value("ViewerGraphicsWindow/pitch_down").toString() == QString(Qt::Key::Key_S));
	QVERIFY(pSettingsWindow->getSettings()->value("ViewerGraphicsWindow/spin_right").toString() == QString(Qt::Key::Key_D));
	QVERIFY(pSettingsWindow->getSettings()->value("ViewerGraphicsWindow/spin_left").toString() == QString(Qt::Key::Key_A));

	resetKeybinds->released();
}



QTEST_MAIN(ModelViewerTest)
#include "test.moc"