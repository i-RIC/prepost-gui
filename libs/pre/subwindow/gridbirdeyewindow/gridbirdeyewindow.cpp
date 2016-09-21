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
#include <QUndoCommand>

#include <vtkRenderWindow.h>
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
}

void GridBirdEyeWindow::init(PreProcessorGridDataItem* item)
{
	setWindowTitle(tr("Grid Bird's-Eye View Window"));

	m_icon = QIcon(":/libs/pre/images/iconBirdEyeWindow.png");
	// set default central widget.
	m_graphicsView = new GridBirdEyeWindowGraphicsView(this);

	// set default background color to be black.
	m_graphicsView->mainRenderer()->SetBackground(1, 1, 1);

	m_graphicsView->setActiveDataItem(0);
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
	cameraFit();
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

class GridBirdEyeWindowEditBackgroundColorCommand : public QUndoCommand
{
public:
	GridBirdEyeWindowEditBackgroundColorCommand(double oldc[3], double newc[3], GridBirdEyeWindow* w)
		: QUndoCommand(QObject::tr("Edit Background Color")) {
		for (int i = 0; i < 3; ++i) {
			m_oldColor[i] = oldc[i];
			m_newColor[i] = newc[i];
		}
		m_window = w;
	}
	void undo() {
		m_window->m_dataModel->graphicsView()->mainRenderer()->SetBackground(m_oldColor);
		m_window->m_dataModel->graphicsView()->GetRenderWindow()->Render();
	}
	void redo() {
		m_window->m_dataModel->graphicsView()->mainRenderer()->SetBackground(m_newColor);
		m_window->m_dataModel->graphicsView()->GetRenderWindow()->Render();
	}
private:
	double m_oldColor[3];
	double m_newColor[3];
	GridBirdEyeWindow* m_window;
};

void GridBirdEyeWindow::editBackgroundColor()
{
	double vtkOldColor[3];

	m_dataModel->graphicsView()->mainRenderer()->GetBackground(vtkOldColor);
	QColor oldcolor;
	iRIC::VTKColorToQColor(vtkOldColor, oldcolor);
	QColor newcolor = QColorDialog::getColor(oldcolor, this, tr("Background Color"));
	if (! newcolor.isValid()) {return;}
	double vtkNewColor[3];
	iRIC::QColorToVTKColor(newcolor, vtkNewColor);
	iRICUndoStack::instance().push(new GridBirdEyeWindowEditBackgroundColorCommand(vtkOldColor, vtkNewColor, this));
}

void GridBirdEyeWindow::displaySetting()
{
	m_dataModel->displaySetting();
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
