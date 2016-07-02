#ifndef GEODATAPOLYGON_PUSHNEWPOINTCOMMAND_H
#define GEODATAPOLYGON_PUSHNEWPOINTCOMMAND_H

#include "../geodatapolygon.h"

#include <QUndoCommand>
#include <QVector2D>

class GeoDataPolygon::PushNewPointCommand : public QUndoCommand
{
public:
	PushNewPointCommand(bool keyDown, const QPoint& point, GeoDataPolygon* pol);
	void redo() override;
	void undo() override;

	int id() const override;
	bool mergeWith(const QUndoCommand* other) override;

private:
	bool m_keyDown;
	GeoDataPolygon* m_polygon;
	GeoDataPolygonAbstractPolygon* m_targetPolygon;
	QVector2D m_newPoint;
	bool m_oldMapped;
};

#endif // GEODATAPOLYGON_PUSHNEWPOINTCOMMAND_H
