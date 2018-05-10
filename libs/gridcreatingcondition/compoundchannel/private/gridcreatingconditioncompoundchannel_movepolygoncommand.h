#ifndef GRIDCREATINGCONDITIONCOMPOUNDCHANNEL_MOVEPOLYGONCOMMAND_H
#define GRIDCREATINGCONDITIONCOMPOUNDCHANNEL_MOVEPOLYGONCOMMAND_H

#include "../gridcreatingconditioncompoundchannel.h"

#include <QUndoCommand>

class GridCreatingConditionCompoundChannel::MovePolygonCommand : public QUndoCommand
{
public:
	MovePolygonCommand(bool keyDown, const QPoint& from, const QPoint& to, GridCreatingConditionCompoundChannel* cond);

	void redo() override;
	void undo() override;
	int id() const override;
	bool mergeWith(const QUndoCommand* other) override;

private:
	void applyOffset(const QPointF& offset);

	bool m_keyDown;
	GridCreatingConditionCompoundChannelAbstractPolygon* m_targetPolygon;
	GridCreatingConditionCompoundChannel* m_condition;
	QPointF m_offset;
};

#endif // GRIDCREATINGCONDITIONCOMPOUNDCHANNEL_MOVEPOLYGONCOMMAND_H
