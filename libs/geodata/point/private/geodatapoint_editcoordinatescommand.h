#ifndef GEODATAPOINT_EDITCOORDINATESCOMMAND_H
#define GEODATAPOINT_EDITCOORDINATESCOMMAND_H

#include "../geodatapoint.h"

#include <QUndoCommand>

class GeoDataPoint::EditCoordinatesCommand : public QUndoCommand
{
public:
	EditCoordinatesCommand(bool apply, const QPointF& p, GeoDataPoint* point);

	void redo() override;
	void undo() override;

private:
	void applyCoords(const QPointF& point);

	bool m_apply;

	QPointF m_newPoint;
	QPointF m_oldPoint;

	GeoDataPoint* m_point;
};

#endif // GEODATAPOINT_EDITCOORDINATESCOMMAND_H
