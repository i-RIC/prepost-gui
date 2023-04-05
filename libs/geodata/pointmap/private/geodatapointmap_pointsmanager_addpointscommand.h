#ifndef GEODATAPOINTMAP_POINTSMANAGER_ADDPOINTSCOMMAND_H
#define GEODATAPOINTMAP_POINTSMANAGER_ADDPOINTSCOMMAND_H

#include "geodatapointmap_pointsmanager.h"

#include <QUndoCommand>

class GeoDataPointmap::PointsManager::AddPointsCommand : public QUndoCommand
{
public:
	AddPointsCommand(const std::vector<QPointF>& points, const std::vector<double>& values, PointsManager* manager);

	void redo() override;
	void undo() override;

private:
	void apply(vtkPoints* points, vtkDoubleArray* vals);

	vtkSmartPointer<vtkPoints> m_newPoints;
	vtkSmartPointer<vtkDoubleArray> m_newValues;

	vtkSmartPointer<vtkPoints> m_oldPoints;
	vtkSmartPointer<vtkDoubleArray> m_oldValues;

	PointsManager* m_manager;
};

#endif // GEODATAPOINTMAP_POINTSMANAGER_ADDPOINTSCOMMAND_H
