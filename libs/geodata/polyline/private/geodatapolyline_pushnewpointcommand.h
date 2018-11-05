#ifndef GEODATAPOLYLINE_PUSHNEWPOINTCOMMAND_H
#define GEODATAPOLYLINE_PUSHNEWPOINTCOMMAND_H

#include "../geodatapolyline.h"
#include "geodatapolyline_modifyabstractpolylinecommand.h"

#include <QUndoCommand>
#include <QVector2D>

class GeoDataPolyLine::PushNewPointCommand : public GeoDataPolyLine::ModifyAbstractPolyLineCommand
{
public:
	PushNewPointCommand(bool keyDown, const QPoint& point, GeoDataPolyLine* pol);

	int id() const override;
	bool mergeWith(const QUndoCommand* other) override;

	void undo() override;

private:
	bool m_keyDown;
};

#endif // GEODATAPOLYLINE_PUSHNEWPOINTCOMMAND_H
