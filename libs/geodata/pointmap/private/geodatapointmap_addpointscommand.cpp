#include "geodatapointmap_addpointscommand.h"
#include "../geodatapointmapaddpointdialog.h"

#include <vtkDoubleArray.h>
#include <vtkIntArray.h>
#include <vtkPointData.h>

GeoDataPointmap::AddPointsCommand::AddPointsCommand(const std::vector<QPointF>& points, double value, GeoDataPointmap* pm) :
	QUndoCommand {GeoDataPointmap::tr("Add Points")},
	m_newPoints {vtkSmartPointer<vtkPoints>::New()},
	m_newValues {vtkSmartPointer<vtkDoubleArray>::New()},
	m_oldPoints {pm->vtkGrid()->GetPoints()},
	m_oldValues {pm->vtkGrid()->GetPointData()->GetArray(VALUES)},
	m_pointmap {pm}
{
	m_newPoints->DeepCopy(m_oldPoints.Get());
	m_newValues->DeepCopy(m_oldValues.Get());

	for (const QPointF& p : points) {
		m_newPoints->InsertNextPoint(p.x(), p.y(), 0);
		m_newValues->InsertNextValue(value);
	}
}

void GeoDataPointmap::AddPointsCommand::redo()
{
	apply(m_newPoint, m_newValues);
}

void GeoDataPointmap::AddPointsCommand::undo()
{
	apply(m_oldPoints, m_oldValues);
}

void GeoDataPointmap::AddPointsCommand::apply(vtkPoints* points, vtkDoubleArray* vals)
{
	m_pointmap->setPoints(points, vals);
	m_pointmap->m_needRemeshing = true;
	m_pointmap->setMapped(false);
}
