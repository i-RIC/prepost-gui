#ifndef GEODATAPOINT_MOVEVERTEXCOMMAND_H
#define GEODATAPOINT_MOVEVERTEXCOMMAND_H

#include "../geodatapoint.h"

#include <QUndoCommand>

class GeoDataPoint::MoveVertexCommand : public QUndoCommand
{
public:
	MoveVertexCommand(bool keyDown, const QPointF& p, GeoDataPoint* point);

	void redo() override;
	void undo() override;

	int id() const override;
	bool mergeWith(const QUndoCommand* other) override;

private:
	void apply(const QPointF& point);

	QPointF m_newPoint;
	QPointF m_oldPoint;

	bool m_keyDown;
	GeoDataPoint* m_point;
};

#endif // GEODATAPOINT_MOVEVERTEXCOMMAND_H
