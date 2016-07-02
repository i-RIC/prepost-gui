#ifndef GEODATAPOLYGON_ADDHOLEPOLYGONCOMMAND_H
#define GEODATAPOLYGON_ADDHOLEPOLYGONCOMMAND_H

#include "../geodatapolygon.h"

#include <QUndoCommand>

class GeoDataPolygon::AddHolePolygonCommand : public QUndoCommand
{
public:
	AddHolePolygonCommand(GeoDataPolygonHolePolygon* newPoly, GeoDataPolygon* pol);
	~AddHolePolygonCommand();

	void redo() override;
	void undo() override;

private:
	bool m_undoed;
	GeoDataPolygon* m_polygon;
	GeoDataPolygonHolePolygon* m_targetPolygon;
	bool m_oldMapped;
};

#endif // GEODATAPOLYGON_ADDHOLEPOLYGONCOMMAND_H
