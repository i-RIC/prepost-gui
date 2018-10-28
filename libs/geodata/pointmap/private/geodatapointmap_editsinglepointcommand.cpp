#include "geodatapointmap_editsinglepointcommand.h"

#include <vtkPointData.h>

GeoDataPointmap::EditSinglePointCommand::EditSinglePointCommand(double x, double y, double value, vtkIdType editedPoint, GeoDataPointmap* p) :
	QUndoCommand {GeoDataPointmap::tr("Edit Points")},
	m_newPoint {x, y},
	m_newValue {value},
	m_editedPoint {editedPoint},
	m_pointMap {p}
{
	double point[3];
	p->vtkGrid()->GetPoint(editedPoint, point);
	m_oldPoint = QPointF(point[0], point[1]);
	vtkDataArray* vals = p->vtkGrid()->GetPointData()->GetArray(VALUES);
	m_oldValue = vals->GetTuple1(editedPoint);
}

void GeoDataPointmap::EditSinglePointCommand::redo()
{
	applyPoint(m_newPoint, m_newValue);
}

void GeoDataPointmap::EditSinglePointCommand::undo()
{
	applyPoint(m_oldPoint, m_oldValue);
}

void GeoDataPointmap::EditSinglePointCommand::applyPoint(const QPointF& p, double v)
{
	m_pointMap->vtkGrid()->GetPoints()->SetPoint(m_editedPoint, p.x(), p.y(), 0);
	m_pointMap->vtkGrid()->GetPoints()->Modified();
	m_pointMap->vtkGrid()->GetPointData()->GetArray(VALUES)->SetTuple1(m_editedPoint, v);
	m_pointMap->vtkGrid()->GetPointData()->GetArray(VALUES)->Modified();
	m_pointMap->m_needRemeshing = true;
	m_pointMap->setMapped(false);
}
