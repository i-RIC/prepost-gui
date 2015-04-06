#include "graphics3dwindowdatamodel.h"
#include "vtk3dgraphicsview.h"

void Graphics3DWindowDataModel::resetRotation()
{
	dynamic_cast<VTK3DGraphicsView*>(m_graphicsView)->resetRoll();
}

void Graphics3DWindowDataModel::rotate90()
{
	dynamic_cast<VTK3DGraphicsView*>(m_graphicsView)->rotate(90);
}

void Graphics3DWindowDataModel::toXYPlane()
{
	dynamic_cast<VTK3DGraphicsView*>(m_graphicsView)->toXYPlane();
}

void Graphics3DWindowDataModel::toYZPlane()
{
	dynamic_cast<VTK3DGraphicsView*>(m_graphicsView)->toYZPlane();
}

void Graphics3DWindowDataModel::toZXPlane()
{
	dynamic_cast<VTK3DGraphicsView*>(m_graphicsView)->toZXPlane();
}
