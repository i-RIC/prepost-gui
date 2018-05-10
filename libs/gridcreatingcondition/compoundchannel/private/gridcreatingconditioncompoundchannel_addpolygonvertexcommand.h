#ifndef GRIDCREATINGCONDITIONCOMPOUNDCHANNEL_ADDPOLYGONVERTEXCOMMAND_H
#define GRIDCREATINGCONDITIONCOMPOUNDCHANNEL_ADDPOLYGONVERTEXCOMMAND_H

#include "../gridcreatingconditioncompoundchannel.h"

#include <QUndoCommand>

class GridCreatingConditionCompoundChannel::AddPolygonVertexCommand : public QUndoCommand
{
public:
	AddPolygonVertexCommand(bool keyDown, vtkIdType edgeId, QPoint point, GridCreatingConditionCompoundChannel* cond);

	void redo() override;
	void undo() override;
	int id() const override;
	bool mergeWith(const QUndoCommand* other) override;

private:
	bool m_keyDown;
	vtkIdType m_vertexId;
	QPointF m_vertexPosition;
	GridCreatingConditionCompoundChannelAbstractPolygon* m_targetPolygon;
	GridCreatingConditionCompoundChannel* m_condition;
};

#endif // GRIDCREATINGCONDITIONCOMPOUNDCHANNEL_ADDPOLYGONVERTEXCOMMAND_H
