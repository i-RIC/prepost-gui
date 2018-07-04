#ifndef GRIDCREATINGCONDITIONCOMPOUNDCHANNEL_EDITPOLYLINECOORDINATESCOMMAND_H
#define GRIDCREATINGCONDITIONCOMPOUNDCHANNEL_EDITPOLYLINECOORDINATESCOMMAND_H

#include "../gridcreatingconditioncompoundchannel.h"

#include <QUndoCommand>

class GridCreatingConditionCompoundChannel::EditPolyLineCoordinatesCommand : public QUndoCommand
{
public:
	EditPolyLineCoordinatesCommand(bool apply, const std::vector<QPointF>& coords, GridCreatingConditionCompoundChannelAbstractLine* l, GridCreatingConditionCompoundChannel* cond);

	void redo() override;
	void undo() override;

private:
	bool m_apply;
	std::vector<QPointF> m_newCoords;
	std::vector<QPointF> m_oldCoords;

	GridCreatingConditionCompoundChannelAbstractLine* m_line;
	GridCreatingConditionCompoundChannel* m_condition;
};

#endif // GRIDCREATINGCONDITIONCOMPOUNDCHANNEL_EDITPOLYLINECOORDINATESCOMMAND_H
