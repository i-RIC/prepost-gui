#ifndef GEODATAPOLYLINE_MOVEPOLYLINECOMMAND_H
#define GEODATAPOLYLINE_MOVEPOLYLINECOMMAND_H

#include "../geodatapolyline.h"

#include <QUndoCommand>
#include <QPointF>

class GeoDataPolyLine::MovePolyLineCommand : public QUndoCommand
{
public:
	MovePolyLineCommand(bool keyDown, const QPoint& from, const QPoint& to, GeoDataPolyLine* pol);

	void redo() override;
	void undo() override;

	int id() const override;
	bool mergeWith(const QUndoCommand* other) override;

private:
	static void movePolyLine(GeoDataPolyLineAbstractPolyLine* polyline, const QPointF& offset);

	bool m_keyDown;
	GeoDataPolyLine* m_polyline;
	QPointF m_offset;
	bool m_oldMapped;
};

#endif // GEODATAPOLYLINE_MOVEPOLYLINECOMMAND_H
