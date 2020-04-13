#ifndef GEODATAPOLYGON_EDITNAMEANDVALUECOMMAND_H
#define GEODATAPOLYGON_EDITNAMEANDVALUECOMMAND_H

#include "../geodatapolygon.h"

#include <QUndoCommand>

class GeoDataPolygon::EditNameAndValueCommand : public QUndoCommand
{
public:
	EditNameAndValueCommand(const QString& newName, const QVariant newValue, GeoDataPolygon* polylgon);

	void redo() override;
	void undo() override;

private:
	QString m_newName;
	QVariant m_newValue;

	QString m_oldName;
	QVariant m_oldValue;

	bool m_oldMapped;
	GeoDataPolygon* m_polygon;
};

#endif // GEODATAPOLYGON_EDITNAMEANDVALUECOMMAND_H
