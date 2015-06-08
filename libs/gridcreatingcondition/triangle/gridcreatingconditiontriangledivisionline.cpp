#include "gridcreatingconditiontriangledivisionline.h"

#include <vtkProperty.h>

GridCreatingConditionTriangleDivisionLine::GridCreatingConditionTriangleDivisionLine(GridCreatingConditionTriangle* parent)
	: GridCreatingConditionTriangleAbstractLine(parent)
{
	m_edgeActor->GetProperty()->SetColor(0, 0, 0);
	m_vertexActor->GetProperty()->SetColor(0, 0, 0);
}
