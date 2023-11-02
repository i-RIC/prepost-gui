#include "../../datamodel/preprocessorgriddataitem.h"
#include "preprocessorgridcrosssectioninternalwindow.h"
#include "preprocessorgridcrosssectionwindow.h"

#include <crosssection/public/abstractcrosssectionwindow_controller.h>

#include <QDockWidget>
#include <QToolBar>

PreProcessorGridCrosssectionWindow::PreProcessorGridCrosssectionWindow(PreProcessorGridDataItem* item, QWidget* parent) :
	QMainWindowWithSnapshot(parent),
	m_internalWindow {new PreProcessorGridCrosssectionInternalWindow(item, this)},
	m_item {item}
{
	setWindowTitle(tr("Grid Crosssection Window"));
	setWindowIcon(QIcon(":/libs/crosssection/images/iconRiverCrosssection.svg"));

	m_internalWindow->setupDisplaySettings();
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
}

PreProcessorGridCrosssectionWindow::~PreProcessorGridCrosssectionWindow()
{
	m_item->removeCrossSectionWindow(this);
}

PreProcessorGridCrosssectionInternalWindow* PreProcessorGridCrosssectionWindow::internalWindow() const
{
	return m_internalWindow;
}

QPixmap PreProcessorGridCrosssectionWindow::snapshot() const
{
	return m_internalWindow->snapshot();
}

QWidget* PreProcessorGridCrosssectionWindow::snapshotArea() const
{
	return m_internalWindow->graphicsView();
}

void PreProcessorGridCrosssectionWindow::cameraFit()
{
	m_internalWindow->cameraFit();
}
