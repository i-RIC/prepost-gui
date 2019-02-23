#ifndef GRIDCREATINGCONDITIONLAPLACE_EDGEEDITCOORDINATESCOMMAND_H
#define GRIDCREATINGCONDITIONLAPLACE_EDGEEDITCOORDINATESCOMMAND_H

#include "../gridcreatingconditionlaplace.h"

#include <QPointF>
#include <QUndoCommand>

class GridCreatingConditionLaplace::EdgeEditCoordinatesCommand : public QUndoCommand
{
public:
	EdgeEditCoordinatesCommand(bool streamWise, int i, int j, const std::vector<QPointF>& coords, GridCreatingConditionLaplace::Impl* impl);

	void redo() override;
	void undo() override;

private:
	void apply(const std::vector<QPointF>& points);

	bool m_streamWise;
	int m_i;
	int m_j;

	std::vector<QPointF> m_newCoords;
	std::vector<QPointF> m_oldCoords;

	GridCreatingConditionLaplace::Impl* m_impl;
};

#endif // GRIDCREATINGCONDITIONLAPLACE_EDGEEDITCOORDINATESCOMMAND_H
