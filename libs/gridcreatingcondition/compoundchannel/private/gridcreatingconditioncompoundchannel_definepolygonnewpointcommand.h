#ifndef GRIDCREATINGCONDITIONCOMPOUNDCHANNEL_DEFINEPOLYGONNEWPOINTCOMMAND_H
#define GRIDCREATINGCONDITIONCOMPOUNDCHANNEL_DEFINEPOLYGONNEWPOINTCOMMAND_H

#include "../gridcreatingconditioncompoundchannel.h"

#include <QUndoCommand>

class GridCreatingConditionCompoundChannel::DefinePolygonNewPointCommand : public QUndoCommand
{
public:
	DefinePolygonNewPointCommand(bool keyDown, const QPoint& point, GridCreatingConditionCompoundChannel* cond);

	void redo() override;
	void undo() override;
	int id() const override;
	bool mergeWith(const QUndoCommand* other) override;

private:
	bool m_keyDown;
	GridCreatingConditionCompoundChannelAbstractPolygon* m_targetPolygon;
	GridCreatingConditionCompoundChannel* m_condition;
	QPointF m_newPoint;
};

#endif // GRIDCREATINGCONDITIONCOMPOUNDCHANNEL_DEFINEPOLYGONNEWPOINTCOMMAND_H
