#include "geodatapointmap_addpointscommand.h"

#include <vtkDoubleArray.h>
#include <vtkIntArray.h>
#include <vtkPointData.h>

GeoDataPointmap::AddPointsCommand::AddPointsCommand(const std::vector<QPointF>& points, const std::vector<double>& values, GeoDataPointmap* pm) :
	QUndoCommand {GeoDataPointmap::tr("Add Points")},
	m_newPoints {vtkSmartPointer<vtkPoints>::New()},
	m_newValues {vtkSmartPointer<vtkDoubleArray>::New()},
	m_oldPoints {pm->vtkGrid()->GetPoints()},
	m_oldValues {vtkDoubleArray::SafeDownCast(pm->vtkGrid()->GetPointData()->GetArray(VALUES))},
	m_pointmap {pm}
{
	m_newPoints->DeepCopy(m_oldPoints.Get());
	m_newValues->DeepCopy(m_oldValues.Get());

	for (int i = 0; i < points.size(); ++i) {
		const auto& p = points.at(i);
		double v = values.at(i);
		m_newPoints->InsertNextPoint(p.x(), p.y(), 0);
		m_newValues->InsertNextValue(v);
	}
}

void GeoDataPointmap::AddPointsCommand::redo()
{
	apply(m_newPoints, m_newValues);
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
