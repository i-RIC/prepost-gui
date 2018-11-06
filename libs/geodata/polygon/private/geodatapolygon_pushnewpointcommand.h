#ifndef GEODATAPOLYGON_PUSHNEWPOINTCOMMAND_H
#define GEODATAPOLYGON_PUSHNEWPOINTCOMMAND_H

#include "../geodatapolygon.h"
#include "geodatapolygon_modifyabstractpolygoncommand.h"

#include <QUndoCommand>
#include <QVector2D>

class GeoDataPolygon::PushNewPointCommand : public GeoDataPolygon::ModifyAbstractPolygonCommand
{
public:
	PushNewPointCommand(bool keyDown, const QPoint& point, GeoDataPolygon* pol);

	int id() const override;
	bool mergeWith(const QUndoCommand* other) override;

	void undo() override;

private:
	bool m_keyDown;
};

#endif // GEODATAPOLYGON_PUSHNEWPOINTCOMMAND_H
