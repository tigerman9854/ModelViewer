#include <QtTest/QtTest>
#include "ModelViewer.h"

class ModelViewerTest : public QObject {
	Q_OBJECT

private slots:
	void initTestCase();
	void cleanupTestCase();

	// Add tests here
	void testShow();

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



QTEST_MAIN(ModelViewerTest)
#include "test.moc"