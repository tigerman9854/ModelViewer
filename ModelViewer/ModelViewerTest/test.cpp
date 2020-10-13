#include <QtTest/QtTest>
#include "ModelViewer.h"
#include "ModelLoader.h"
#include "ViewerGraphicsWindow.h"

class ModelViewerTest : public QObject {
	Q_OBJECT

private slots:
	void initTestCase();
	void cleanupTestCase();

	// Add tests here
	void testShow();
	void loadModel();
	void displayModel();

private:
	ModelViewer* m_pWindow;
};



void ModelViewerTest::initTestCase() 
{
	m_pWindow = new ModelViewer();
}

void ModelViewerTest::cleanupTestCase()
{
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



QTEST_MAIN(ModelViewerTest)
#include "test.moc"