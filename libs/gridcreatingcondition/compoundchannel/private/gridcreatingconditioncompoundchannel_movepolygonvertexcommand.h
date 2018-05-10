#ifndef GRIDCREATINGCONDITIONCOMPOUNDCHANNEL_MOVEPOLYGONVERTEXCOMMAND_H
#define GRIDCREATINGCONDITIONCOMPOUNDCHANNEL_MOVEPOLYGONVERTEXCOMMAND_H

#include "../gridcreatingconditioncompoundchannel.h"

#include <QUndoCommand>

class GridCreatingConditionCompoundChannel::MovePolygonVertexCommand : public QUndoCommand
{
public:
	MovePolygonVertexCommand(bool keyDown, const QPoint& from, const QPoint& to, vtkIdType vertexId, GridCreatingConditionCompoundChannel* cond);

	void redo() override;
	void undo() override;
	int id() const override;
	bool mergeWith(const QUndoCommand* other) override;

private:
	void applyOffset(const QPointF& offset);

	bool m_keyDown;
	vtkIdType m_vertexId;
	GridCreatingConditionCompoundChannelAbstractPolygon* m_targetPolygon;
	GridCreatingConditionCompoundChannel* m_condition;
	QPointF m_offset;
};

#endif // GRIDCREATINGCONDITIONCOMPOUNDCHANNEL_MOVEPOLYGONVERTEXCOMMAND_H
