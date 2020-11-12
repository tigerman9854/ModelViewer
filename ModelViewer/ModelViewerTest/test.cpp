#include <QtTest/QtTest>
#include <QMenu>
#include <QMenuBar>
#include <QSignalSpy>

#include "ModelViewer.h"
#include "ModelLoader.h"
#include "ViewerGraphicsWindow.h"
#include "GraphicsWindowDelegate.h"

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


void ModelViewerTest::integration()// README: The addition of my settings menu borked this test
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
	QTest::mouseMove(m_pWindow->GetGraphicsWindow(), QPoint(3, 3));
	QTest::mouseRelease(m_pWindow->GetGraphicsWindow(), Qt::LeftButton, { 0 }, QPoint(3, 3));

	QVERIFY(m_pWindow->GetGraphicsWindow()->GetModelMatrix() != resetMatrix);
	QTest::qWait(100);


	// Pan the camera
	m_pWindow->GetGraphicsWindow()->resetView();

	QTest::mousePress(m_pWindow->GetGraphicsWindow(), Qt::RightButton, { 0 }, QPoint(0, 0));
	QTest::mouseMove(m_pWindow->GetGraphicsWindow(), QPoint(150, 150));
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
	QTest::mouseMove(m_pWindow->GetGraphicsWindow(), QPoint(3, 3));
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
	QTest::mousePress(m_pWindow->GetGraphicsWindow(), Qt::LeftButton);
	QTest::mouseMove(m_pWindow->GetGraphicsWindow(), QPoint(3, 3));
	QTest::mouseRelease(m_pWindow->GetGraphicsWindow(), Qt::LeftButton, { 0 }, QPoint(3, 3));

	QVERIFY(m_pWindow->GetGraphicsWindow()->GetModelMatrix() != resetMatrix);
	m_pWindow->hide();
}


void ModelViewerTest::panWithMouse()
{
	ResetViewAndShow();

	// Test Pan
	QTest::mousePress(m_pWindow->GetGraphicsWindow(), Qt::RightButton);
	QTest::mouseMove(m_pWindow->GetGraphicsWindow(), QPoint(10, 10));
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

	ViewerGraphicsWindow* pGraphicsWindow = m_pWindow->GetGraphicsWindow();

	// Load a model
	bool success = m_pWindow->GetGraphicsWindow()->loadModel("../Data/Models/cubeColor.ply");
	QVERIFY(success);

	// Click on the graphics window to set focus
	QTest::mouseClick(pGraphicsWindow, Qt::LeftButton);

	auto testKey = [=](int key) {
		ResetViewAndShow();

		QTest::keyPress(pGraphicsWindow, key);
		QTest::qWait(100);
		QTest::keyRelease(pGraphicsWindow, key);

		pGraphicsWindow->requestUpdate();
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
}



QTEST_MAIN(ModelViewerTest)
#include "test.moc"