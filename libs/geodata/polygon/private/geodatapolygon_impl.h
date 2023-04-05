#ifndef GEODATAPOLYGON_IMPL_H
#define GEODATAPOLYGON_IMPL_H

#include "../geodatapolygon.h"

#include <misc/zdepthrange.h>

class GeoDataPolygonTriangleThread;

class vtkActor;
class vtkDoubleArray;

class GeoDataPolygon::Impl
{
public:
	Impl(GeoDataPolygon* parent);
	~Impl();

	GeoDataPolygon* m_parent;

	GeoDataPolygonRegionPolygon* m_regionPolygon;
	QList<GeoDataPolygonHolePolygon*> m_holePolygons;
	GeoDataPolygonAbstractPolygon* m_selectedPolygon;

	ZDepthRange m_depthRange;

	GeoDataPolygonTriangleThread* m_triangleThread;

	bool m_shapeUpdating;
	bool m_bcSettingMode;

	SelectMode m_selectMode;
	MouseEventMode m_mouseEventMode;

	vtkPolyData* m_polyData;
	vtkActor* m_actor;
	vtkDoubleArray* m_scalarValues;

	QPoint m_dragStartPoint;
	QPoint m_currentPoint;

	QMenu* m_rightClickingMenu;

	QAction* m_holeModeAction;
	QAction* m_deleteAction;
	QAction* m_copyAction;
	QAction* m_addVertexAction;
	QAction* m_removeVertexAction;
	QAction* m_coordEditAction;

	QAction* m_customHoleModeAction;
	QAction* m_customDeleteAction;
	QAction* m_customAddVertexAction;
	QAction* m_customRemoveVertexAction;
	QAction* m_customCoordEditAction;

	QPixmap m_addPixmap;
	QPixmap m_removePixmap;
	QPixmap m_movePointPixmap;
	QCursor m_addCursor;
	QCursor m_removeCursor;
	QCursor m_movePointCursor;
};

#endif // GEODATAPOLYGON_IMPL_H
