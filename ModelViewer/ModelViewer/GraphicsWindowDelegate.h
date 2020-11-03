#pragma once
#include <QWidget>
#include <QLayout>
#include <QLabel>

// Forward Declares
class ViewerGraphicsWindow;


class GraphicsWindowDelegate : public QWidget
{
	Q_OBJECT

public:
	GraphicsWindowDelegate(ViewerGraphicsWindow* graphicsWindow, QWidget* parent = nullptr);

	enum class Status {
		k_empty,
		k_model,
		k_error,
		k_loading,
	};

	void SetStatus(Status s);
	Status GetStatus();

private slots:
	void OnViewerInitialized();
	void OnBeginModelLoading(QString filepath);
	void OnEndModelLoading(bool success, QString filepath);
	void OnError(QString message);
	void OnModelUnloaded();

private:
	void DoOnStatusChanged();

	QLayout* m_pMainLayout = nullptr;
	QWidget* m_pCurrentWidget = nullptr;

	// Widgets to change between to show 
	QWidget* m_pEmptyWidget = nullptr;
	QWidget* m_pModelWidget = nullptr;
	QWidget* m_pErrorWidget = nullptr;
	QWidget* m_pLoadingWidget = nullptr;

	QLabel* m_pEmptyText = nullptr;
	QLabel* m_pErrorText = nullptr;
	QLabel* m_pLoadingText = nullptr;

	Status m_status;

	ViewerGraphicsWindow* m_pGraphicsWindow = nullptr;
};