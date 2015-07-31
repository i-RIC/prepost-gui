#include "graphics2dwindowdatamodel.h"
#include "vtk2dgraphicsview.h"

#include <misc/iricundostack.h>

#include <QUndoCommand>

#include <vtkCamera.h>
#include <vtkRenderer.h>

Graphics2DWindowDataModel::Graphics2DWindowDataModel(QMainWindow* w, ProjectDataItem* parent)
	: GraphicsWindowDataModel(w, parent)
{
	m_dataRange.setRange(0, 100);
	m_hoverRange.setRange(110, 120);
	m_labelRange.setRange(130, 140);
}
