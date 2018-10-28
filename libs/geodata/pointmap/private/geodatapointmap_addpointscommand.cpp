#include "geodatapointmap_addpointscommand.h"
#include "../geodatapointmapaddpointdialog.h"

#include <vtkDoubleArray.h>
#include <vtkIntArray.h>
#include <vtkPointData.h>

GeoDataPointmap::AddPointsCommand::AddPointsCommand(GeoDataPointmap* p, GeoDataPointmapAddPointDialog* ip) :
	QUndoCommand {QObject::tr("Add Points")}
{
	this->m_pmap = p;
	this->m_iPoints = ip;
	newpoints = vtkSmartPointer<vtkPoints>::New();
	newvalues = vtkSmartPointer<vtkDoubleArray>::New();
	oldpoints = vtkSmartPointer<vtkPoints>::New();
	oldvalues = vtkSmartPointer<vtkDoubleArray>::New();
	size_t numInterpPts = m_iPoints->xpoint.size();

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

	// don't use first point it's already a known
	for (size_t i = 1; i < numInterpPts; i++) {
		double p[3];
		double value;
		p[0] = m_iPoints->xpoint[i];
		p[1] = m_iPoints->ypoint[i];
		p[2] = 0;
		value = m_iPoints->zpoint[i];
		newpoints->InsertNextPoint(p);
		newvalues->InsertNextValue(value);
	}
}

void GeoDataPointmap::AddPointsCommand::redo()
{
	m_pmap->setPoints(newpoints, newvalues);
	this->m_pmap->setMapped(false);
	this->m_pmap->m_needRemeshing = true;
}

void GeoDataPointmap::AddPointsCommand::undo()
{
	m_pmap->setPoints(oldpoints, oldvalues);
	this->m_pmap->m_needRemeshing = true;
	this->m_pmap->setMapped(false);
}
