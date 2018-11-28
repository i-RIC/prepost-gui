#ifndef GEODATAPOLYLINE_EDITCOORDINATESCOMMAND_H
#define GEODATAPOLYLINE_EDITCOORDINATESCOMMAND_H

#include "../geodatapolyline.h"

#include <QUndoCommand>

class GeoDataPolyLine::EditCoordinatesCommand : public QUndoCommand
{
public:
	EditCoordinatesCommand(bool apply, const std::vector<QPointF>& coords, GeoDataPolyLine* pol);

	void redo() override;
	void undo() override;

private:
	void applyCoords(const std::vector<QPointF>& coords);

	bool m_apply;

	std::vector<QPointF> m_newCoords;
	std::vector<QPointF> m_oldCoords;

	GeoDataPolyLineAbstractPolyLine* m_selectedPolyLine;
	GeoDataPolyLine* m_polyline;
};

#endif // GEODATAPOLYLINE_EDITCOORDINATESCOMMAND_H
