#ifndef GEODATAPOLYGON_EDITCOORDINATESCOMMAND_H
#define GEODATAPOLYGON_EDITCOORDINATESCOMMAND_H

#include "../geodatapolygon.h"

#include <QUndoCommand>

class GeoDataPolygon::EditCoordinatesCommand : public QUndoCommand
{
public:
	EditCoordinatesCommand(bool apply, const std::vector<QPointF>& coords, GeoDataPolygon* pol);

	void redo() override;
	void undo() override;

private:
	void applyCoords(const std::vector<QPointF>& coords);

	bool m_apply;

	std::vector<QPointF> m_newCoords;
	std::vector<QPointF> m_oldCoords;

	GeoDataPolygonAbstractPolygon* m_selectedPolygon;
	GeoDataPolygon* m_polygon;
};

#endif // GEODATAPOLYGON_EDITCOORDINATESCOMMAND_H
