#ifndef GEODATAPOLYGON_MOVEPOLYGONCOMMAND_H
#define GEODATAPOLYGON_MOVEPOLYGONCOMMAND_H

#include "../geodatapolygon.h"

#include <QUndoCommand>
#include <QPointF>

class GeoDataPolygon::MovePolygonCommand : public QUndoCommand
{
public:
	MovePolygonCommand(bool keyDown, const QPoint& from, const QPoint& to, GeoDataPolygon* pol);

	void redo() override;
	void undo() override;

	int id() const override;
	bool mergeWith(const QUndoCommand* other) override;

private:
	static void movePolygon(GeoDataPolygonAbstractPolygon* polygon, const QPointF& offset);

	bool m_keyDown;
	GeoDataPolygon* m_polygon;
	QPointF m_offset;
	bool m_oldMapped;
};

#endif // GEODATAPOLYGON_MOVEPOLYGONCOMMAND_H
