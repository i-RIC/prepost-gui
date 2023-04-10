#ifndef GEODATAPOINTMAP_POINTSMANAGER_EDITSINGLEPOINTCOMMAND_H
#define GEODATAPOINTMAP_POINTSMANAGER_EDITSINGLEPOINTCOMMAND_H

#include "geodatapointmap_pointsmanager.h"

#include <QUndoCommand>

class GeoDataPointmap::PointsManager::EditSinglePointCommand : public QUndoCommand
{
public:
	EditSinglePointCommand(bool apply, double x, double y, double val, vtkIdType index, PointsManager* manager);

	void redo() override;
	void undo() override;

	int id() const override;
	bool mergeWith(const QUndoCommand* other) override;

private:
	void apply(const QPointF& p, double v);

	bool m_apply;

	QPointF m_newPoint;
	double m_newValue;

	QPointF m_oldPoint;
	double m_oldValue;

	vtkIdType m_index;
	PointsManager* m_manager;
};

#endif // GEODATAPOINTMAP_POINTSMANAGER_EDITSINGLEPOINTCOMMAND_H
