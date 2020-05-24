#ifndef GEODATAPOINT_EDITNAMEANDVALUECOMMAND_H
#define GEODATAPOINT_EDITNAMEANDVALUECOMMAND_H

#include "../geodatapoint.h"

#include <QUndoCommand>

class GeoDataPoint::EditNameAndValueCommand : public QUndoCommand
{
public:
	EditNameAndValueCommand(const QString& newName, const QVariant newValue, GeoDataPoint* point);

	void redo() override;
	void undo() override;

private:
	QString m_newName;
	QVariant m_newValue;

	QString m_oldName;
	QVariant m_oldValue;

	bool m_oldMapped;
	GeoDataPoint* m_point;
};

#endif // GEODATAPOINT_EDITNAMEANDVALUECOMMAND_H
