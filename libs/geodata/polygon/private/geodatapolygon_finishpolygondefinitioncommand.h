#ifndef GEODATAPOLYGON_FINISHPOLYGONDEFINITIONCOMMAND_H
#define GEODATAPOLYGON_FINISHPOLYGONDEFINITIONCOMMAND_H

#include "../geodatapolygon.h"

#include <QUndoCommand>

class GeoDataPolygon::FinishPolygonDefiningCommand : public QUndoCommand
{
public:
	FinishPolygonDefiningCommand(GeoDataPolygon* polygon);

	void redo() override;
	void undo() override;

private:
	GeoDataPolygon* m_polygon;
	GeoDataPolygonAbstractPolygon* m_targetPolygon;
};

#endif // GEODATAPOLYGON_FINISHPOLYGONDEFINITIONCOMMAND_H
