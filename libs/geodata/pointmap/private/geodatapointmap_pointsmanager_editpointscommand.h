#ifndef GEODATAPOINTMAP_POINTSMANAGER_EDITPOINTSCOMMAND_H
#define GEODATAPOINTMAP_POINTSMANAGER_EDITPOINTSCOMMAND_H

#include "geodatapointmap_pointsmanager.h"

#include <QUndoCommand>

class GeoDataPointmap::PointsManager::EditPointsCommand : public QUndoCommand
{
public:
	EditPointsCommand(bool apply, double val, const std::vector<vtkIdType>& indices, PointsManager* manager);

	void redo() override;
	void undo() override;

	int id() const override;
	bool mergeWith(const QUndoCommand* other) override;

private:
	bool m_apply;

	double m_newValue;
	std::vector<double> m_oldValues;

	std::vector<vtkIdType> m_indices;
	PointsManager* m_manager;
};

#endif // GEODATAPOINTMAP_POINTSMANAGER_EDITPOINTSCOMMAND_H
