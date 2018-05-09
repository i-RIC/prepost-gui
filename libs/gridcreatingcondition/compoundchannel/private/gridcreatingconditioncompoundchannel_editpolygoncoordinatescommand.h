#ifndef GRIDCREATINGCONDITIONCOMPOUNDCHANNEL_EDITPOLYGONCOORDINATESCOMMAND_H
#define GRIDCREATINGCONDITIONCOMPOUNDCHANNEL_EDITPOLYGONCOORDINATESCOMMAND_H

#include "../gridcreatingconditioncompoundchannel.h"

#include <QUndoCommand>

class GridCreatingConditionCompoundChannel::EditPolygonCoordinatesCommand : public QUndoCommand
{
public:
	EditPolygonCoordinatesCommand(bool apply, const std::vector<QPointF>& coords, GridCreatingConditionCompoundChannelAbstractPolygon* pol, GridCreatingConditionCompoundChannel* cond);

	void redo() override;
	void undo() override;

private:
	bool m_apply;
	std::vector<QPointF> m_newCoords;
	std::vector<QPointF> m_oldCoords;

	GridCreatingConditionCompoundChannelAbstractPolygon* m_polygon;
	GridCreatingConditionCompoundChannel* m_cond;
};

#endif // GRIDCREATINGCONDITIONCOMPOUNDCHANNEL_EDITPOLYGONCOORDINATESCOMMAND_H
