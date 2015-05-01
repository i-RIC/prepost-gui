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

GridBirdEyeWindow::GridBirdEyeWindow(QWidget* parent, PreProcessorGridDataItem* item)
	: QMainWindow(parent)
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

vtkRenderWindow* GridBirdEyeWindow::getVtkRenderWindow()
{
	return m_graphicsView->mainRenderer()->GetRenderWindow();
}

void GridBirdEyeWindow::updateGrid()
{
	m_dataModel->updateGrid();
	m_dataModel->fit();
}

QList<QMenu*> GridBirdEyeWindow::getAdditionalMenus()
{
	QList<QMenu*> menus;
	menus.append(m_actionManager->drawMenu());
	return menus;
}

void GridBirdEyeWindow::cameraFit()
{
	m_graphicsView->fitInView();
}

void GridBirdEyeWindow::cameraZoomIn()
{
	m_dataModel->zoomIn();
}

void GridBirdEyeWindow::cameraZoomOut()
{
	m_dataModel->zoomOut();
}

void GridBirdEyeWindow::cameraMoveLeft()
{
	m_dataModel->moveLeft();
}

void GridBirdEyeWindow::cameraMoveRight()
{
	m_dataModel->moveRight();
}

void GridBirdEyeWindow::cameraMoveUp()
{
	m_dataModel->moveUp();
}

void GridBirdEyeWindow::cameraMoveDown()
{
	m_dataModel->moveDown();
}

void GridBirdEyeWindow::cameraXYPlane()
{
	m_dataModel->toXYPlane();
}

void GridBirdEyeWindow::cameraYZPlane()
{
	m_dataModel->toYZPlane();
}

void GridBirdEyeWindow::cameraZXPlane()
{
	m_dataModel->toZXPlane();
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
