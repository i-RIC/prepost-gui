#ifndef GEODATAPOINTMAP_INTERPOLATELINEADDPOINTCOMMAND_H
#define GEODATAPOINTMAP_INTERPOLATELINEADDPOINTCOMMAND_H

#include "../geodatapointmap.h"

#include <QPointF>
#include <QUndoCommand>

class GeoDataPointmap::InterpolateLineAddPointCommand : public QUndoCommand
{
public:
	InterpolateLineAddPointCommand(double x, double y, double val, bool newFlag, GeoDataPointmap* map);

	void redo() override;
	void undo() override;

private:
	QPointF m_newPoint;
	double m_newVal;
	bool m_newFlag;

	GeoDataPointmap* m_pointMap;
};

#endif // GEODATAPOINTMAP_INTERPOLATELINEADDPOINTCOMMAND_H
