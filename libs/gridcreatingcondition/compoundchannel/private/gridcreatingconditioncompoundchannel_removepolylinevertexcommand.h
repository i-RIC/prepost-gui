#ifndef GRIDCREATINGCONDITIONCOMPOUNDCHANNEL_REMOVEPOLYLINEVERTEXCOMMAND_H
#define GRIDCREATINGCONDITIONCOMPOUNDCHANNEL_REMOVEPOLYLINEVERTEXCOMMAND_H

#include "../gridcreatingconditioncompoundchannel.h"

#include <QUndoCommand>

class GridCreatingConditionCompoundChannel::RemovePolyLineVertexCommand : public QUndoCommand
{
public:
	RemovePolyLineVertexCommand(vtkIdType vertexId, GridCreatingConditionCompoundChannel* cond);

	void redo() override;
	void undo() override;

private:
	vtkIdType m_vertexId;
	QPointF m_vertexPosition;
	GridCreatingConditionCompoundChannelAbstractLine* m_targetLine;
	GridCreatingConditionCompoundChannel* m_condition;
};

#endif // GRIDCREATINGCONDITIONCOMPOUNDCHANNEL_REMOVEPOLYLINEVERTEXCOMMAND_H
