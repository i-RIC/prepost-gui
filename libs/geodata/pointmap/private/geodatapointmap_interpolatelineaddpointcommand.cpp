#include "geodatapointmap_interpolatelineaddpointcommand.h"

#include <vtkDoubleArray.h>

GeoDataPointmap::InterpolateLineAddPointCommand::InterpolateLineAddPointCommand(bool keyDown, const double x, const double y, double zpos, GeoDataPointmap* ptmap) :
	QUndoCommand {GeoDataPointmap::tr("Add New Interp Line Point")}
{
	m_keyDown = keyDown;
	m_newPoint = QPointF(x, y);
	zposition = zpos;
	m_pointMap = ptmap;
}

void GeoDataPointmap::InterpolateLineAddPointCommand::redo()
{
	vtkPolygon* pol = m_pointMap->m_vtkInterpPolygon;
	m_pointMap->m_vtkInterpValue->InsertNextValue(zposition);
	if (m_keyDown) {
		// add new point.
		pol->GetPoints()->InsertNextPoint(m_newPoint.x(), m_newPoint.y(), 0.);
		pol->GetPoints()->Modified();
	} else {
		// modify the last point.
		vtkIdType lastId = pol->GetNumberOfPoints() - 1;
		pol->GetPoints()->SetPoint(lastId, m_newPoint.x(), m_newPoint.y(), 0.);
		pol->GetPoints()->Modified();
	}
	pol->Modified();
	m_pointMap->m_needRemeshing = true;
	m_pointMap->setMapped(false);
	m_pointMap->updateInterpShapeData();
}

void GeoDataPointmap::InterpolateLineAddPointCommand::undo()
{
	vtkPolygon* pol = m_pointMap->m_vtkInterpPolygon;
	if (m_keyDown) {
		// decrease the number of points. i. e. remove the last point.
		vtkIdType numOfPoints = pol->GetPoints()->GetNumberOfPoints();
		pol->GetPoints()->SetNumberOfPoints(numOfPoints - 1);
		pol->GetPoints()->Modified();
		m_pointMap->m_vtkInterpValue->SetNumberOfValues(numOfPoints-1);
	} else {
		// this does not happen. no implementation needed.
	}
	pol->Modified();
	m_pointMap->m_needRemeshing = true;
	m_pointMap->setMapped(false);
	m_pointMap->updateInterpShapeData();
}

bool GeoDataPointmap::InterpolateLineAddPointCommand::mergeWith(const QUndoCommand* other)
{
	const InterpolateLineAddPointCommand* comm = dynamic_cast<const InterpolateLineAddPointCommand*>(other);
	if (comm == nullptr) {return false;}
	if (comm->m_keyDown) {return false;}
	if (comm->m_pointMap != m_pointMap) {return false;}
	m_newPoint = comm->m_newPoint;
	return true;
}
