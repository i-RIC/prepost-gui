#ifndef GEODATAPOLYGON_EDITVALUECOMMAND_H
#define GEODATAPOLYGON_EDITVALUECOMMAND_H

#include "../geodatapolygon.h"

#include <QUndoCommand>

class GeoDataPolygon::EditValueCommand : public QUndoCommand
{
public:
	EditValueCommand(const QVariant& newvalue, GeoDataPolygon* polygon);

	void redo() override;
	void undo() override;

private:
	QVariant m_newValue;
	QVariant m_oldValue;
	bool m_oldMapped;
	GeoDataPolygon* m_polygon;
};

#endif // GEODATAPOLYGON_EDITVALUECOMMAND_H
