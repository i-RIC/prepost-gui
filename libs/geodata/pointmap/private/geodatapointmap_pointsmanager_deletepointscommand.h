#ifndef GEODATAPOINTMAP_POINTSMANAGER_DELETEPOINTSCOMMAND_H
#define GEODATAPOINTMAP_POINTSMANAGER_DELETEPOINTSCOMMAND_H

#include "geodatapointmap_pointsmanager.h"

#include <vtkDoubleArray.h>
#include <vtkSmartPointer.h>
#include <vtkPoints.h>

#include <QUndoCommand>

#include <unordered_set>

class GeoDataPointmap::PointsManager::DeletePointsCommand : public QUndoCommand
{
public:
	DeletePointsCommand(std::unordered_set<vtkIdType> deletedPoints, PointsManager* manager);

	void redo() override;
	void undo() override;

private:
	vtkSmartPointer<vtkPoints> m_newPoints;
	vtkSmartPointer<vtkDoubleArray> m_newValues;

	vtkSmartPointer<vtkPoints> m_oldPoints;
	vtkSmartPointer<vtkDoubleArray> m_oldValues;

	std::unordered_set<vtkIdType> m_deletedPoints;
	PointsManager* m_manager;
};

#endif // GEODATAPOINTMAP_POINTSMANAGER_DELETEPOINTSCOMMAND_H
