#ifndef GRIDCREATINGCONDITIONCENTERANDWIDTH_IMPL_H
#define GRIDCREATINGCONDITIONCENTERANDWIDTH_IMPL_H

#include "gridcreatingconditioncenterandwidth_setting.h"
#include "../gridcreatingconditioncenterandwidth.h"

#include <guibase/polyline/polylinecontroller.h>
#include <guibase/vtktool/vtklabel2dactor.h>

#include <QCursor>
#include <QPixmap>

class QAction;

class vtkActor;
class vtkDataSetMapper;
class vtkCardinalSpline;
class vtkParametricSpline;

class GridCreatingConditionCenterAndWidth::Impl
{
public:
	enum class MouseEventMode
	{
		Normal,
		BeforeDefining,
		Defining,
		Translate,
		TranslatePrepare,
		MoveVertex,
		MoveVertexPrepare,
		AddVertex,
		AddVertexPrepare,
		AddVertexNotPossible,
		RemoveVertexPrepare,
		RemoveVertexNotPossible,

		EditVerticesDialog
	};

	Impl(GridCreatingConditionCenterAndWidth* parent);
	~Impl();

	void updateMouseEventMode(const QPoint& pos, PreProcessorGraphicsViewI* view);
	void updateMouseCursor(PreProcessorGraphicsViewI* v);
	void updateActionStatus();

	Setting m_setting;

	int m_selectedVertexId;
	int m_selectedEdgeId;

	int m_oldIMax;
	int m_oldJMax;
	double m_oldWidth;

	PolyLineController m_polyLineController;
	vtkLabel2DActor m_upstreamActor;
	vtkLabel2DActor m_downstreamActor;

	vtkActor* m_previewActor;
	vtkDataSetMapper* m_previewMapper;
	v4InputGrid* m_previewGrid;

	vtkCardinalSpline* m_xSpline;
	vtkCardinalSpline* m_ySpline;
	vtkCardinalSpline* m_zSpline;
	vtkParametricSpline* m_spline;
	vtkPoints* m_splinePoints;

	MouseEventMode m_mouseEventMode;

	bool m_isAccepted;
	bool m_isGridCreated;

	QPoint m_dragStartPoint;
	QPoint m_previousPoint;

	QPixmap m_addPixmap;
	QPixmap m_removePixmap;
	QCursor m_addCursor;
	QCursor m_removeCursor;

	QMenu* m_rightClickingMenu;
	QAction* m_addVertexAction;
	QAction* m_removeVertexAction;
	QAction* m_coordEditAction;
	QAction* m_reverseCenterLineAction;
	QAction* m_importCenterLineAction;
	QAction* m_exportCenterLineAction;

private:
	GridCreatingConditionCenterAndWidth* m_parent;
};

#endif // GRIDCREATINGCONDITIONCENTERANDWIDTH_IMPL_H
