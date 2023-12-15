#include "postcrosssectioninternalwindow.h"
#include "postcrosssectionwindow.h"

#include <crosssection/public/abstractcrosssectionwindow_controller.h>
#include <guicore/postcontainer/postsolutioninfo.h>

#include <QDockWidget>

PostCrosssectionWindow::PostCrosssectionWindow(QWidget* parent, int index, PostProcessorWindowProjectDataItem* pdi) :
	PostProcessorWindow(parent, index, pdi),
	m_internalWindow {new PostCrosssectionInternalWindow(this)}
{
	setWindowTitle(tr("Post Crosssection: %1").arg(m_index));
	m_icon = QIcon(":/libs/crosssection/images/iconRiverCrosssection.svg");

	setCentralWidget(m_internalWindow);

	addToolBar(m_internalWindow->viewToolBar());
	addToolBar(m_internalWindow->displayToolBar());

	auto dockWidget = new QDockWidget(tr("Controller"), this);
	dockWidget->setFeatures(QDockWidget::DockWidgetMovable);
	dockWidget->setAllowedAreas(Qt::BottomDockWidgetArea | Qt::TopDockWidgetArea);
	dockWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
	dockWidget->setWidget(m_internalWindow->controller());
	m_internalWindow->controller()->setParent(dockWidget);

	addDockWidget(Qt::BottomDockWidgetArea, dockWidget);

	connect(postSolutionInfo(), &PostSolutionInfo::updated, m_internalWindow, &PostCrosssectionInternalWindow::handleSettingChange);
	connect(postSolutionInfo(), &PostSolutionInfo::currentStepUpdated, m_internalWindow, &PostCrosssectionInternalWindow::handleSettingChange);
}

PostCrosssectionInternalWindow* PostCrosssectionWindow::internalWindow() const
{
	return m_internalWindow;
}

QPixmap PostCrosssectionWindow::snapshot() const
{
	return m_internalWindow->snapshot();
}

QWidget* PostCrosssectionWindow::snapshotArea() const
{
	return m_internalWindow->graphicsView();
}

void PostCrosssectionWindow::cameraFit()
{
	m_internalWindow->cameraFit();
}

void PostCrosssectionWindow::setupDefaultGeometry(int index)
{
	QWidget* parent = parentWidget();
	if (! parent->isMaximized()) {
		parent->move(index * 30, index * 30);
		parent->resize(700, 500);
	}

	m_internalWindow->update();
	m_internalWindow->cameraFit();
}
