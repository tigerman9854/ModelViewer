#include <QtTest/QtTest>
#include <QMenu>

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
	void testShow();
	void loadModel();
	void displayModel();
	void integration();

private:
	ModelViewer* m_pWindow = nullptr;
};



void ModelViewerTest::initTestCase() 
{
	// Called once before all test cases
}

void ModelViewerTest::cleanupTestCase()
{
	// Called once after all test casese
}

void ModelViewerTest::init()
{
	// Called before each test case
	m_pWindow = new ModelViewer();
}

void ModelViewerTest::cleanup()
{
	// Called after each test case
	delete m_pWindow;
}



void ModelViewerTest::testShow()
{
	QVERIFY(m_pWindow->isHidden());
	m_pWindow->show();
	QTest::qWait(1000);
	QVERIFY(m_pWindow->isHidden() == false);
	m_pWindow->hide();
}

void ModelViewerTest::loadModel()
{
	ModelLoader m;
	Model loaded = m.LoadModel("../Data/Models/cube.obj");
	QVERIFY(loaded.m_isValid);
}

void ModelViewerTest::displayModel()
{
	m_pWindow->show();
	bool success = m_pWindow->GetGraphicsWindow()->loadModel("../Data/Models/cube.obj");
	QVERIFY(success);
	success = m_pWindow->GetGraphicsWindow()->loadModel("../Data/Models/cube2.obj");
	QVERIFY(success);
	success = m_pWindow->GetGraphicsWindow()->loadModel("../Data/Models/cubeColor.ply");
	QVERIFY(success);
	QTest::qWait(1000);
	QVERIFY(success);
	m_pWindow->hide();
}

void ModelViewerTest::integration()
{
	// Show the window, make sure it appeared
	QVERIFY(m_pWindow->isHidden());
	m_pWindow->show();
	QTest::qWait(100);
	QVERIFY(m_pWindow->isHidden() == false);

	QMenu* pFileMenu = m_pWindow->findChild<QMenu*>("FileMenu");
	//auto pFileMenuActions = m_p

}


QTEST_MAIN(ModelViewerTest)
#include "test.moc"