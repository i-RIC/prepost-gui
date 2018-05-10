#ifndef GRIDCREATINGCONDITIONCOMPOUNDCHANNEL_MOVEPOLYLINECOMMAND_H
#define GRIDCREATINGCONDITIONCOMPOUNDCHANNEL_MOVEPOLYLINECOMMAND_H

#include "../gridcreatingconditioncompoundchannel.h"

#include <QUndoCommand>

class GridCreatingConditionCompoundChannel::MovePolyLineCommand : public QUndoCommand
{
public:
	MovePolyLineCommand(bool keyDown, const QPoint& from, const QPoint& to, GridCreatingConditionCompoundChannel* cond);

	void redo() override;
	void undo() override;
	int id() const override;
	bool mergeWith(const QUndoCommand* other) override;

private:
	void applyOffset(const QPointF& offset);

	bool m_keyDown;
	GridCreatingConditionCompoundChannelAbstractLine* m_targetLine;
	GridCreatingConditionCompoundChannel* m_condition;
	QPointF m_offset;
};

#endif // GRIDCREATINGCONDITIONCOMPOUNDCHANNEL_MOVEPOLYLINECOMMAND_H
