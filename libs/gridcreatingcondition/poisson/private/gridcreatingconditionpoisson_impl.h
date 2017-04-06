#ifndef GRIDCREATINGCONDITIONPOISSON_IMPL_H
#define GRIDCREATINGCONDITIONPOISSON_IMPL_H

#include "../gridcreatingconditionpoisson.h"

#include <guibase/polyline/polylinecontroller.h>
#include <guibase/vtktool/vtklabel2dactor.h>

#include <QPixmap>
#include <QCursor>

class GeoDataRiverSurvey;
class Grid;

class QAction;

class GridCreatingConditionPoisson::Impl
{
public:
	enum class MouseEventMode {
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
	};

	Impl(GridCreatingConditionPoisson* parent);
	~Impl();

	void finishDefiningLine();
	void updateLabels();
	void updateMouseEventMode(const QPoint& mousePosition);
	void updateMouseCursor(PreProcessorGraphicsViewInterface* v);
	void updateActionStatus();

	void copyCenterLine(GeoDataRiverSurvey* data, int num);
	Grid* createGrid();

	PolyLineController m_centerLineController;
	PolyLineController m_leftBankLineController;
	PolyLineController m_rightBankLineController;

	PolyLineController m_upstreamLineController;
	PolyLineController m_downstreamLineController;

	vtkLabel2DActor m_upstreamActor;
	vtkLabel2DActor m_downstreamActor;
	vtkLabel2DActor m_leftBankActor;
	vtkLabel2DActor m_rightBankActor;

	PolyLineController* m_activeLine;

	MouseEventMode m_mouseEventMode;
	int m_selectedVertexId;
	int m_selectedEdgeId;

	int m_iDiv;
	int m_jDiv;

	double m_previousLeftBankDistance;
	double m_previousRightBankDistance;

	QPoint m_previousPos;
	QPoint m_pressPos;

	QAction* m_buildBankLinesAction;
	QAction* m_addVertexAction;
	QAction* m_removeVertexAction;
	QAction* m_coordEditAction;

	QAction* m_importCenterLineAction;
	QAction* m_exportCenterLineAction;
	QAction* m_importLeftBankLineAction;
	QAction* m_exportLeftBankLineAction;
	QAction* m_importRightBankLineAction;
	QAction* m_exportRightBankLineAction;

	QPixmap m_addCursorPixmap;
	QPixmap m_removeCursorPixmap;

	QCursor m_addCursor;
	QCursor m_removeCursor;

	QMenu* m_rightClickingMenu;

private:
	GridCreatingConditionPoisson*	m_parent;
};

#endif // GRIDCREATINGCONDITIONPOISSON_IMPL_H
