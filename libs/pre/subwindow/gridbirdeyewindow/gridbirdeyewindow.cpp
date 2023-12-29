#include "gridbirdeyeobjectbrowser.h"
#include "gridbirdeyewindow.h"
#include "gridbirdeyewindowactionmanager.h"
#include "gridbirdeyewindowdatamodel.h"
#include "gridbirdeyewindowgraphicsview.h"
#include "gridbirdeyewindowprojectdataitem.h"
#include "datamodel/gridbirdeyewindowzonedataitem.h"

#include <guibase/colortool.h>
#include <guibase/graphicsmisc.h>
#include <misc/iricundostack.h>

#include <QAction>
#include <QColorDialog>
#include <QLabel>
#include <QToolBar>

#include <vtkRenderer.h>

GridBirdEyeWindow::GridBirdEyeWindow(QWidget* parent, PreProcessorGridDataItem* item) :
	QMainWindowWithSnapshot {parent},
	m_projectDataItem {nullptr}
{
	init(item);
}

GridBirdEyeWindow::~GridBirdEyeWindow()
{
	delete m_dataModel;
	if (m_projectDataItem != nullptr) {
		m_projectDataItem->informWindowClose();
	}

	// Closing the window is not undoable
	iRICUndoStack::instance().clear();
}

void GridBirdEyeWindow::init(PreProcessorGridDataItem* /*item*/)
{
	setWindowTitle(tr("Grid Bird's-Eye View Window"));

	m_icon = QIcon(":/libs/pre/images/iconBirdEyeWindow.svg");
	// set default central widget.
	QLabel* label = new QLabel(this);
	setCentralWidget(label);

	m_actionManager = new GridBirdEyeWindowActionManager(this);

	m_objectBrowser = new GridBirdEyeObjectBrowser(this);
	addDockWidget(Qt::LeftDockWidgetArea, m_objectBrowser);
}

ObjectBrowser* GridBirdEyeWindow::objectBrowser() const
{
	return m_objectBrowser;
}

GridBirdEyeWindowDataModel* GridBirdEyeWindow::dataModel() const
{
	return m_dataModel;
}

QPixmap GridBirdEyeWindow::snapshot() const
{
	QImage img = m_dataModel->graphicsView()->getImage();
	QPixmap pixmap = QPixmap::fromImage(img);
	pixmap.setDevicePixelRatio(devicePixelRatioF());
	return pixmap;
}

QWidget* GridBirdEyeWindow::snapshotArea() const
{
	return m_dataModel->graphicsView();
}

vtkRenderWindow* GridBirdEyeWindow::getVtkRenderWindow() const
{
	vtkRenderer* r = const_cast<vtkRenderer*> (m_dataModel->graphicsView()->mainRenderer());
	return r->GetRenderWindow();
}

void GridBirdEyeWindow::updateGrid()
{
	m_dataModel->zoneDataItem()->update();
}

const QIcon& GridBirdEyeWindow::icon() const
{
	return m_icon;
}

QList<QMenu*> GridBirdEyeWindow::getAdditionalMenus() const
{
	QList<QMenu*> menus;
	menus.append(m_actionManager->drawMenu());
	return menus;
}

const std::shared_ptr<QToolBar>& GridBirdEyeWindow::getAdditionalToolBar() const
{
	return m_dataModel->operationToolBar();
}

void GridBirdEyeWindow::cameraFit()
{
	m_dataModel->graphicsView()->cameraFit();
}

void GridBirdEyeWindow::cameraZoomIn()
{
	m_dataModel->graphicsView()->cameraZoomIn();
}

void GridBirdEyeWindow::cameraZoomOut()
{
	m_dataModel->graphicsView()->cameraZoomOut();
}

void GridBirdEyeWindow::cameraMoveLeft()
{
	m_dataModel->graphicsView()->cameraMoveLeft();
}

void GridBirdEyeWindow::cameraMoveRight()
{
	m_dataModel->graphicsView()->cameraMoveRight();
}

void GridBirdEyeWindow::cameraMoveUp()
{
	m_dataModel->graphicsView()->cameraMoveUp();
}

void GridBirdEyeWindow::cameraMoveDown()
{
	m_dataModel->graphicsView()->cameraMoveDown();
}

void GridBirdEyeWindow::cameraXYPlane()
{
	m_dataModel->graphicsView()->cameraToXYPlane();
}

void GridBirdEyeWindow::cameraYZPlane()
{
	m_dataModel->graphicsView()->cameraToYZPlane();
}

void GridBirdEyeWindow::cameraZXPlane()
{
	m_dataModel->graphicsView()->cameraToZXPlane();
}

void GridBirdEyeWindow::editBackgroundColor()
{
	BackgroundColorEditI::editBackgroundColor(this);
}

void GridBirdEyeWindow::editDisplaySetting()
{
	m_dataModel->editDisplaySetting();
}

void GridBirdEyeWindow::editZScale()
{
	m_dataModel->editZScale();
}

void GridBirdEyeWindow::cameraParallelProjection()
{
	m_dataModel->graphicsView()->cameraParallelProjection();
}

void GridBirdEyeWindow::cameraPerspectiveProjection()
{
	m_dataModel->graphicsView()->cameraPerspectiveProjection();
}

void GridBirdEyeWindow::updateProjectionMenu(QAction* parallel, QAction* perspective)
{
	m_dataModel->graphicsView()->updateProjectionMenu(parallel, perspective);
}

VTKGraphicsView* GridBirdEyeWindow::viewForBackgroundColor() const
{
	return m_dataModel->graphicsView();
}
