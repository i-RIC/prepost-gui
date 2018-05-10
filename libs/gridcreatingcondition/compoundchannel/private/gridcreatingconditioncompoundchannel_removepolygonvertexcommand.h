#ifndef GRIDCREATINGCONDITIONCOMPOUNDCHANNEL_REMOVEPOLYGONVERTEXCOMMAND_H
#define GRIDCREATINGCONDITIONCOMPOUNDCHANNEL_REMOVEPOLYGONVERTEXCOMMAND_H

#include "../gridcreatingconditioncompoundchannel.h"

#include <QUndoCommand>

class GridCreatingConditionCompoundChannel::RemovePolygonVertexCommand : public QUndoCommand
{
public:
	RemovePolygonVertexCommand(vtkIdType vertexId, GridCreatingConditionCompoundChannel* cond);

	void redo() override;
	void undo() override;

private:
	vtkIdType m_vertexId;
	QPointF m_vertexPosition;
	GridCreatingConditionCompoundChannelAbstractPolygon* m_targetPolygon;
	GridCreatingConditionCompoundChannel* m_condition;
};

#endif // GRIDCREATINGCONDITIONCOMPOUNDCHANNEL_REMOVEPOLYGONVERTEXCOMMAND_H
