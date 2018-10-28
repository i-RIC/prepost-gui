#include "geodatapointmap_deletepointscommand.h"

#include <vtkDoubleArray.h>
#include <vtkIntArray.h>
#include <vtkPointData.h>

GeoDataPointmap::DeletePointsCommand::DeletePointsCommand(const QString& title, QVector<vtkIdType> deletedPoints, GeoDataPointmap* p) :
	QUndoCommand {title},
	m_newPoints {vtkSmartPointer<vtkPoints>::New()},
	m_oldPoints {p->vtkGrid()->GetPoints()},
	m_oldValues {p->vtkGrid()->GetPointData()->GetArray(VALUES)},
	m_deletedPoints (deletedPoints),
	m_pointMap {p}
{
	m_newPoints->SetDataTypeToDouble();
	QString name = m_oldValues->GetClassName();
	if (name == "vtkIntArray") {
		m_newValues = vtkSmartPointer<vtkIntArray>::New();
	} else if (name == "vtkDoubleArray") {
		m_newValues = vtkSmartPointer<vtkDoubleArray>::New();
	}
	// build m_newPoints, m_newValues
	int pos = 0;
	for (vtkIdType i = 0; i < m_oldPoints->GetNumberOfPoints(); ++i) {
		if (pos < m_deletedPoints.count() && m_deletedPoints[pos] == i) {
			// this point is deleted, so skip.
			++ pos;
		} else {
			m_newPoints->InsertNextPoint(m_oldPoints->GetPoint(i));
			m_newValues->InsertNextTuple1(m_oldValues->GetTuple1(i));
		}
	}
}

void GeoDataPointmap::DeletePointsCommand::redo()
{
	m_pointMap->setPoints(m_newPoints, m_newValues);
	m_pointMap->updateBreakLinesOnDelete(m_deletedPoints);
	m_pointMap->m_needRemeshing = true;
	m_pointMap->setMapped(false);
}

void GeoDataPointmap::DeletePointsCommand::undo()
{
	m_pointMap->setPoints(m_oldPoints, m_oldValues);
	m_pointMap->updateBreakLinesOnInsert(m_deletedPoints);
	m_pointMap->m_needRemeshing = true;
	m_pointMap->setMapped(false);
}
