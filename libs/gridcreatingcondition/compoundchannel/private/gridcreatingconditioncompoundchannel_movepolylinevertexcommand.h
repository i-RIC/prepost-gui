#ifndef GRIDCREATINGCONDITIONCOMPOUNDCHANNEL_MOVEPOLYLINEVERTEXCOMMAND_H
#define GRIDCREATINGCONDITIONCOMPOUNDCHANNEL_MOVEPOLYLINEVERTEXCOMMAND_H

#include "../gridcreatingconditioncompoundchannel.h"

#include <QUndoCommand>

class GridCreatingConditionCompoundChannel::MovePolyLineVertexCommand : public QUndoCommand
{
public:
	MovePolyLineVertexCommand(bool keyDown, const QPoint& from, const QPoint& to, vtkIdType vertexId, GridCreatingConditionCompoundChannel* cond);

	void redo() override;
	void undo() override;
	int id() const override;
	bool mergeWith(const QUndoCommand* other) override;

private:
	void applyOffset(const QPointF& offset);

	bool m_keyDown;
	vtkIdType m_vertexId;
	QPointF m_offset;
	GridCreatingConditionCompoundChannelAbstractLine* m_targetLine;
	GridCreatingConditionCompoundChannel* m_condition;
};

#endif // GRIDCREATINGCONDITIONCOMPOUNDCHANNEL_MOVEPOLYLINEVERTEXCOMMAND_H
