#include "gridbirdeyewindow.h"
#include "gridbirdeyewindowactionmanager.h"
#include "gridbirdeyewindowdatamodel.h"
#include "gridbirdeyewindowgraphicsview.h"

#include <guibase/colortool.h>
#include <guibase/graphicsmisc.h>
#include <misc/iricundostack.h>

#include <QAction>
#include <QColorDialog>
#include <QLabel>
#include <QToolBar>

#include <vtkRenderer.h>

GridBirdEyeWindow::GridBirdEyeWindow(QWidget* parent, PreProcessorGridDataItem* item) :
	QMainWindow(parent)
{
	init(item);
}

GridBirdEyeWindow::~GridBirdEyeWindow()
{
	PreProcessorGridDataItem* gItem = dynamic_cast<PreProcessorGridDataItem*>(m_dataModel->parent());
	gItem->informBirdEyeWindowClose();
	delete m_dataModel;

	// Closing the window is not undoable
	iRICUndoStack::instance().clear();
}

void GridBirdEyeWindow::init(PreProcessorGridDataItem* item)
{
	setWindowTitle(tr("Grid Bird's-Eye View Window"));

	m_icon = QIcon(":/libs/pre/images/iconBirdEyeWindow.svg");
	// set default central widget.
	m_graphicsView = new GridBirdEyeWindowGraphicsView(this);
	m_graphicsView->setInteractive(true);
	setCentralWidget(m_graphicsView);
	m_dataModel = new GridBirdEyeWindowDataModel(this, item);

	m_actionManager = new GridBirdEyeWindowActionManager(this);
}

QPixmap GridBirdEyeWindow::snapshot()
{
	QImage img = m_graphicsView->getImage();
	QPixmap pixmap = QPixmap::fromImage(img);
	return pixmap;
}

vtkRenderWindow* GridBirdEyeWindow::getVtkRenderWindow() const
{
	vtkRenderer* r = const_cast<vtkRenderer*> (m_graphicsView->mainRenderer());
	return r->GetRenderWindow();
}

void GridBirdEyeWindow::updateGrid()
{
	m_dataModel->updateGrid();
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

QToolBar* GridBirdEyeWindow::getAdditionalToolBar() const
{
	return nullptr;
}

void GridBirdEyeWindow::cameraFit()
{
	m_graphicsView->cameraFit();
}

void GridBirdEyeWindow::cameraZoomIn()
{
	m_graphicsView->cameraZoomIn();
}

void GridBirdEyeWindow::cameraZoomOut()
{
	m_graphicsView->cameraZoomOut();
}

void GridBirdEyeWindow::cameraMoveLeft()
{
	m_graphicsView->cameraMoveLeft();
}

void GridBirdEyeWindow::cameraMoveRight()
{
	m_graphicsView->cameraMoveRight();
}

void GridBirdEyeWindow::cameraMoveUp()
{
	m_graphicsView->cameraMoveUp();
}

void GridBirdEyeWindow::cameraMoveDown()
{
	m_graphicsView->cameraMoveDown();
}

void GridBirdEyeWindow::cameraXYPlane()
{
	m_graphicsView->cameraToXYPlane();
}

void GridBirdEyeWindow::cameraYZPlane()
{
	m_graphicsView->cameraToYZPlane();
}

void GridBirdEyeWindow::cameraZXPlane()
{
	m_graphicsView->cameraToZXPlane();
}

void GridBirdEyeWindow::editBackgroundColor()
{
	BackgroundColorEditInterface::editBackgroundColor(this);
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
