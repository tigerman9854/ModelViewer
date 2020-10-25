#include <QtTest/QtTest>
#include <QMenu>
#include <QMenuBar>

#include "ModelViewer.h"
#include "ModelLoader.h"
#include "ViewerGraphicsWindow.h"

class ModelViewerTest : public QObject {
	Q_OBJECT

private slots:
	void initTestCase();
	void cleanupTestCase();
	void init();
	void cleanup();

	// Add tests here
	void integration();
	void testShow();
	void loadModel();
	void displayModel();
	void resetView();
	void rotateWithMouse();
	void panWithMouse();
	void checkMousePressAndRelease();
	void zoomWithMouse();

private:
	ModelViewer* m_pWindow = nullptr;
	QMatrix4x4 resetMatrix;
};



void ModelViewerTest::initTestCase() 
{
	// Called once before all test cases
	m_pWindow = new ModelViewer();
	resetMatrix.perspective(60.0f, 4.0f / 3.0f, 0.1f, 100.0f);
	resetMatrix.translate(0, 0, -4);
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
	QTest::keyClick(pLoadMenu, Qt::Key_Enter, Qt::NoModifier, 20);
	QTest::keyClick(pPrimitiveMenu, Qt::Key_Down, Qt::NoModifier, 20);
	QTest::keyClick(pPrimitiveMenu, Qt::Key_Down, Qt::NoModifier, 20);
	QTest::keyClick(pPrimitiveMenu, Qt::Key_Down, Qt::NoModifier, 20);
	QTest::keyClick(pPrimitiveMenu, Qt::Key_Down, Qt::NoModifier, 20);
	QTest::keyClick(pPrimitiveMenu, Qt::Key_Enter, Qt::NoModifier, 20);

	// Rotate the camera
	QTest::mousePress(m_pWindow->GetGraphicsWindow(), Qt::RightButton);
	QTest::mouseMove(m_pWindow->GetGraphicsWindow(), QPoint(3, 3));
	QTest::mouseRelease(m_pWindow->GetGraphicsWindow(), Qt::RightButton);

	QVERIFY(m_pWindow->GetGraphicsWindow()->sceneMatrix != resetMatrix);
	QTest::qWait(100);


	// Pan the camera
	QTest::mousePress(m_pWindow->GetGraphicsWindow(), Qt::LeftButton, { 0 }, QPoint(0, 0));
	QTest::mouseMove(m_pWindow->GetGraphicsWindow(), QPoint(150, 150));
	QTest::mouseRelease(m_pWindow->GetGraphicsWindow(), Qt::LeftButton, { 0 }, QPoint(150, 150));

	QVERIFY(m_pWindow->GetGraphicsWindow()->viewportX != 0);
	QVERIFY(m_pWindow->GetGraphicsWindow()->viewportY != 0);
	QTest::qWait(100);

	m_pWindow->hide();
	QVERIFY(m_pWindow->isHidden());
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
	m_pWindow->show();

	// Check that we start w/ an idenity matrix
	m_pWindow->GetGraphicsWindow()->resetView();
	QVERIFY(m_pWindow->GetGraphicsWindow()->sceneMatrix == resetMatrix);

	// Check that after altering it we end w/ an idenity matrix
	m_pWindow->GetGraphicsWindow()->viewportX = 10;
	m_pWindow->GetGraphicsWindow()->viewportY = 10;
	m_pWindow->GetGraphicsWindow()->sceneMatrix.translate(5, 2, 7);
	m_pWindow->GetGraphicsWindow()->resetView();
	QVERIFY(m_pWindow->GetGraphicsWindow()->sceneMatrix == resetMatrix);
	QVERIFY(m_pWindow->GetGraphicsWindow()->viewportX == 0);
	QVERIFY(m_pWindow->GetGraphicsWindow()->viewportY == 0);
	m_pWindow->hide();
}

void ModelViewerTest::rotateWithMouse()
{
	m_pWindow->show();
	m_pWindow->GetGraphicsWindow()->resetView();

	// Test rotating
	QTest::mousePress(m_pWindow->GetGraphicsWindow(), Qt::RightButton);
	QTest::mouseMove(m_pWindow->GetGraphicsWindow(), QPoint(3, 3));
	QTest::mouseRelease(m_pWindow->GetGraphicsWindow(), Qt::RightButton);

	QVERIFY(m_pWindow->GetGraphicsWindow()->sceneMatrix != resetMatrix);
	m_pWindow->hide();
}


void ModelViewerTest::panWithMouse()
{
	m_pWindow->show();
	m_pWindow->GetGraphicsWindow()->resetView();

	// Test Pan
	QTest::mousePress(m_pWindow->GetGraphicsWindow(), Qt::LeftButton);
	QTest::mouseMove(m_pWindow->GetGraphicsWindow(), QPoint(10, 10));
	QTest::mouseRelease(m_pWindow->GetGraphicsWindow(), Qt::LeftButton, { 0 }, QPoint(10, 10)); // Release the mouse at (10, 10)

	// Wait for new frame to be drawn
	QTest::qWait(100);

	QVERIFY(m_pWindow->GetGraphicsWindow()->viewportX != 0);
	QVERIFY(m_pWindow->GetGraphicsWindow()->viewportY != 0);
	m_pWindow->hide();
}

void ModelViewerTest::checkMousePressAndRelease()// FIXME: The mouse inputs are not working...
{
	m_pWindow->show();
	m_pWindow->GetGraphicsWindow()->resetView();

	// Test right click
	QTest::mousePress(m_pWindow->GetGraphicsWindow(), Qt::RightButton);
	QVERIFY(m_pWindow->GetGraphicsWindow()->rightMousePressed);
	QTest::mouseRelease(m_pWindow->GetGraphicsWindow(), Qt::RightButton);
	QVERIFY(!m_pWindow->GetGraphicsWindow()->rightMousePressed);

	// Test left click
	QTest::mousePress(m_pWindow->GetGraphicsWindow(), Qt::LeftButton);
	QVERIFY(m_pWindow->GetGraphicsWindow()->leftMousePressed);
	QTest::mouseRelease(m_pWindow->GetGraphicsWindow(), Qt::LeftButton);
	QVERIFY(!m_pWindow->GetGraphicsWindow()->leftMousePressed);
	m_pWindow->hide();
}

void ModelViewerTest::zoomWithMouse()
{
	m_pWindow->show();
	m_pWindow->GetGraphicsWindow()->resetView();

	//https://stackoverflow.com/questions/50996997/how-to-simulate-mouse-wheel-events-using-qtestlib-qt5
	ViewerGraphicsWindow* test = m_pWindow->GetGraphicsWindow();
	test->sceneMatrix.scale(.5 * test->zoomSensitivity);
	QVERIFY(test->sceneMatrix != resetMatrix);
}


QTEST_MAIN(ModelViewerTest)
#include "test.moc"