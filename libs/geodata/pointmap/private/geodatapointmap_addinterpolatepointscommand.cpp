#include "geodatapointmap_addinterpolatepointscommand.h"

#include "../geodatapointmapinterpolatepoints.h"

#include <vtkDoubleArray.h>
#include <vtkIntArray.h>
#include <vtkPointData.h>

GeoDataPointmap::AddInterpolatePointsCommand::AddInterpolatePointsCommand(GeoDataPointmap* p, GeoDataPointmapInterpolatePoints* ip) :
	QUndoCommand {QObject::tr("Add Interpolation Pts")}
{
	this->m_pmap = p;
	this->m_iPoints = ip;
	newpoints = vtkSmartPointer<vtkPoints>::New();
	newvalues = vtkSmartPointer<vtkDoubleArray>::New();
	oldpoints = vtkSmartPointer<vtkPoints>::New();
	oldvalues = vtkSmartPointer<vtkDoubleArray>::New();
	size_t numInterpPts = m_iPoints->xinterp.size();

	for (vtkIdType i = 0; i < this->m_pmap->vtkGrid()->GetPoints()->GetNumberOfPoints(); i++) {
		double p[3];
		double value;
		this->m_pmap->vtkGrid()->GetPoints()->GetPoint(i, p);
		newpoints->InsertNextPoint(p);
		oldpoints->InsertNextPoint(p);
		value = this->m_pmap->vtkGrid()->GetPointData()->GetArray(VALUES)->GetTuple1(i);
		newvalues->InsertNextValue(value);
		oldvalues->InsertNextValue(value);
	}

	// don't use first and last points
	for (size_t i = 1; i < numInterpPts-1; i++) {
		double p[3];
		double value;
		p[0] = m_iPoints->xinterp[i];
		p[1] = m_iPoints->yinterp[i];
		p[2] = 0;
		value = m_iPoints->zinterp[i];
		newpoints->InsertNextPoint(p);
		newvalues->InsertNextValue(value);
	}
}

void GeoDataPointmap::AddInterpolatePointsCommand::redo()
{
	m_pmap->setPoints(newpoints, newvalues);
	this->m_pmap->m_needRemeshing = true;
	this->m_pmap->setMapped(false);
}

void GeoDataPointmap::AddInterpolatePointsCommand::undo()
{
	m_pmap->setPoints(oldpoints, oldvalues);
	this->m_pmap->m_needRemeshing = true;
	this->m_pmap->setMapped(false);
}
