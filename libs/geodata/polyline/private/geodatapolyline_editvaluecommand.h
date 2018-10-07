#ifndef GEODATAPOLYLINE_EDITVALUECOMMAND_H
#define GEODATAPOLYLINE_EDITVALUECOMMAND_H

#include "../geodatapolyline.h"

#include <QUndoCommand>

class GeoDataPolyLine::EditValueCommand : public QUndoCommand
{
public:
	EditValueCommand(const QVariant& newvalue, GeoDataPolyLine* polyline);

	void redo() override;
	void undo() override;

private:
	QVariant m_newValue;
	QVariant m_oldValue;
	bool m_oldMapped;
	GeoDataPolyLine* m_polyline;
};

#endif // GEODATAPOLYLINE_EDITVALUECOMMAND_H
