#ifndef GEODATAPOINT_SETVERTEXCOMMAND_H
#define GEODATAPOINT_SETVERTEXCOMMAND_H

#include "../geodatapoint.h"

#include <QUndoCommand>

class GeoDataPoint::SetVertexCommand : public QUndoCommand
{
public:
	SetVertexCommand(bool keyDown, const QPointF& p, GeoDataPoint* point);

	void redo() override;
	void undo() override;

	int id() const override;
	bool mergeWith(const QUndoCommand* other) override;

private:
	QPointF m_position;

	bool m_keyDown;
	GeoDataPoint* m_point;
};


#endif // GEODATAPOINT_SETVERTEXCOMMAND_H
