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
{}

void GraphicsWindowSimpleDataModel::viewOperationEndedGlobal()
{}
