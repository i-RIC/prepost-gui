#include "gridcreatingconditiontriangle.h"
#include "gridcreatingconditiontriangleremeshdialog.h"
#include "gridcreatingconditiontriangleremeshpolygon.h"

#include <guicore/pre/base/preprocessorwindowi.h>
#include <misc/iricundostack.h>
#include <misc/mathsupport.h>

#include <QUndoCommand>

#include <vtkProperty.h>

GridCreatingConditionTriangleRemeshPolygon::GridCreatingConditionTriangleRemeshPolygon(GridCreatingConditionTriangle* parent)
	: GridCreatingConditionTriangleAbstractPolygon(parent)
{
	m_isCellSizeSet = false;
	m_cellSize = 0;

	m_paintActor->GetProperty()->SetColor(1, 0, 0);
	m_paintActor->GetProperty()->SetOpacity(0.1);
	m_edgeActor->GetProperty()->SetColor(1, 0, 0);
	m_vertexActor->GetProperty()->SetColor(1, 0, 0);
}

void GridCreatingConditionTriangleRemeshPolygon::finishDefinition()
{
	if (m_isCellSizeSet == false) {
		QRectF rect = polygon().boundingRect();
		m_cellSize = iRIC::roundedValue(rect.width() * rect.height() / 100, 1);
		editGridSize(true);
		m_isCellSizeSet = true;
	}
}

class GridCreatingConditionTriangleRemeshPolygonEditGridSizeCommand : public QUndoCommand
{
public:
	GridCreatingConditionTriangleRemeshPolygonEditGridSizeCommand(double area, GridCreatingConditionTriangleRemeshPolygon* pol)
		: QUndoCommand(GridCreatingConditionTriangleRemeshPolygon::tr("Edit refinement maximum area")) {
		m_newSize = area;
		m_oldSet = pol->m_isCellSizeSet;
		m_oldSize = pol->m_cellSize;

		m_polygon = pol;
	}
	void undo() {
		m_polygon->m_isCellSizeSet = m_oldSet;
		m_polygon->m_cellSize = m_oldSize;
	}
	void redo() {
		m_polygon->m_isCellSizeSet = true;
		m_polygon->m_cellSize = m_newSize;
	}
private:
	bool m_oldSet;
	double m_oldSize;
	double m_newSize;
	GridCreatingConditionTriangleRemeshPolygon* m_polygon;
};

void GridCreatingConditionTriangleRemeshPolygon::editGridSize(bool required)
{
	GridCreatingConditionTriangleRemeshDialog dialog(m_parent->preProcessorWindow());
	dialog.setArea(m_cellSize);
	if (required) {
		dialog.setRequired();
	}
	if (QDialog::Rejected == dialog.exec()) {
		return;
	}
	iRICUndoStack::instance().push(new GridCreatingConditionTriangleRemeshPolygonEditGridSizeCommand(dialog.area(), this));
}
