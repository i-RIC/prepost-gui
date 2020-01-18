#include "graphicswindowsimpledatamodel.h"
#include "vtkgraphicsview.h"

#include <QMainWindow>

GraphicsWindowSimpleDataModel::GraphicsWindowSimpleDataModel(QMainWindow* w, ProjectDataItem* parent) :
	ProjectDataItem(parent),
	m_mainWindow {w},
	m_graphicsView {dynamic_cast<VTKGraphicsView*>(w->centralWidget())}
{}

GraphicsWindowSimpleDataModel::~GraphicsWindowSimpleDataModel()
{}

VTKGraphicsView* GraphicsWindowSimpleDataModel::graphicsView() const
{
	return m_graphicsView;
}

QMainWindow* GraphicsWindowSimpleDataModel::mainWindow() const
{
	return m_mainWindow;
}

void GraphicsWindowSimpleDataModel::viewOperationEndedGlobal()
{}

void GraphicsWindowSimpleDataModel::handleResize()
{}
