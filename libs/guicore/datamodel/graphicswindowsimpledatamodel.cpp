#include "graphicswindowsimpledatamodel.h"
#include "vtkgraphicsview.h"

#include <QMainWindow>

GraphicsWindowSimpleDataModel::GraphicsWindowSimpleDataModel(QMainWindow* w, ProjectDataItem* parent)
	: ProjectDataItem(parent)
{
	m_mainWindow = w;
	m_graphicsView = dynamic_cast<VTKGraphicsView*>(w->centralWidget());
}

GraphicsWindowSimpleDataModel::~GraphicsWindowSimpleDataModel()
{
}

void GraphicsWindowSimpleDataModel::zoomIn()
{
	m_graphicsView->scale(1.2);
	viewOperationEndedGlobal();
	m_graphicsView->render();
}

void GraphicsWindowSimpleDataModel::zoomOut()
{
	m_graphicsView->scale(1.0 / 1.2);
	viewOperationEndedGlobal();
	m_graphicsView->render();
}

void GraphicsWindowSimpleDataModel::fit()
{
	m_graphicsView->fitInView();
	viewOperationEndedGlobal();
	m_graphicsView->render();
}

void GraphicsWindowSimpleDataModel::moveLeft()
{
	moveCenter(moveWidth(), 0);
}

void GraphicsWindowSimpleDataModel::moveRight()
{
	moveCenter(- moveWidth(), 0);
}

void GraphicsWindowSimpleDataModel::moveUp()
{
	moveCenter(0, - moveWidth());
}

void GraphicsWindowSimpleDataModel::moveDown()
{
	moveCenter(0, moveWidth());
}

qreal GraphicsWindowSimpleDataModel::moveWidth()
{
	return MOVEWIDTH;
}

void GraphicsWindowSimpleDataModel::moveCenter(int x, int y)
{
	m_graphicsView->translate(x, y);
	viewOperationEndedGlobal();
	m_graphicsView->render();
}

void GraphicsWindowSimpleDataModel::viewOperationEndedGlobal()
{

}
