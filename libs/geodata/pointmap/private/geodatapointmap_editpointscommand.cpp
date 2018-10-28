#include "geodatapointmap_editpointscommand.h"

#include <vtkDoubleArray.h>
#include <vtkIntArray.h>
#include <vtkPointData.h>

GeoDataPointmap::EditPointsCommand::EditPointsCommand(double value, QVector<vtkIdType> editedPoints, GeoDataPointmap* p) :
	QUndoCommand {GeoDataPointmap::tr("Edit Points")},
	m_oldValues {p->vtkGrid()->GetPointData()->GetArray(VALUES)},
	m_pointMap {p}
{

	QString name = m_oldValues->GetClassName();
	if (name == "vtkIntArray") {
		m_newValues = vtkSmartPointer<vtkIntArray>::New();
	} else if (name == "vtkDoubleArray") {
		m_newValues = vtkSmartPointer<vtkDoubleArray>::New();
	}
	m_newValues->SetName(VALUES);
	m_newValues->DeepCopy(m_oldValues);

	for (int i = 0; i < editedPoints.count(); ++i) {
		vtkIdType id = editedPoints[i];
		m_newValues->SetTuple1(id, value);
	}
	m_newValues->Modified();
}

void GeoDataPointmap::EditPointsCommand::redo()
{
	m_pointMap->vtkGrid()->GetPointData()->Initialize();
	m_pointMap->vtkGrid()->GetPointData()->AddArray(m_newValues);
	m_pointMap->vtkGrid()->GetPointData()->SetActiveScalars(VALUES);
	m_pointMap->m_needRemeshing = true;
	m_pointMap->setMapped(false);
}

void GeoDataPointmap::EditPointsCommand::undo()
{
	m_pointMap->vtkGrid()->GetPointData()->Initialize();
	m_pointMap->vtkGrid()->GetPointData()->AddArray(m_oldValues);
	m_pointMap->vtkGrid()->GetPointData()->SetActiveScalars(VALUES);
	m_pointMap->m_needRemeshing = true;
	m_pointMap->setMapped(false);
}
