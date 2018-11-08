#ifndef GRIDCREATINGCONDITIONCOMPOUNDCHANNEL_ADDPOLYLINEVERTEXCOMMAND_H
#define GRIDCREATINGCONDITIONCOMPOUNDCHANNEL_ADDPOLYLINEVERTEXCOMMAND_H

#include "../gridcreatingconditioncompoundchannel.h"

#include <QUndoCommand>

class GridCreatingConditionCompoundChannel::AddPolyLineVertexCommand : public QUndoCommand
{
public:
	AddPolyLineVertexCommand(bool keyDown, vtkIdType edgeId, QPoint point, GridCreatingConditionCompoundChannel* cond);

	void redo() override;
	void undo() override;
	int id() const override;
	bool mergeWith(const QUndoCommand* other) override;

private:
	bool m_keyDown;
	vtkIdType m_vertexId;
	QPointF m_vertexPosition;
	GridCreatingConditionCompoundChannelAbstractLine* m_targetLine;
	GridCreatingConditionCompoundChannel* m_condition;
};


#endif // GRIDCREATINGCONDITIONCOMPOUNDCHANNEL_ADDPOLYLINEVERTEXCOMMAND_H
