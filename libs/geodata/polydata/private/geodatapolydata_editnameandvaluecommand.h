#ifndef GEODATAPOLYDATA_EDITNAMEANDVALUECOMMAND_H
#define GEODATAPOLYDATA_EDITNAMEANDVALUECOMMAND_H

#include "../geodatapolydata.h"

#include <QUndoCommand>

class GeoDataPolyData::EditNameAndValueCommand : public QUndoCommand
{
public:
	EditNameAndValueCommand(const QString& newName, const QVariant newValue, GeoDataPolyData* polyData);

	void redo() override;
	void undo() override;

private:
	QString m_newName;
	QVariant m_newValue;

	QString m_oldName;
	QVariant m_oldValue;

	bool m_oldMapped;
	GeoDataPolyData* m_polyData;
};

#endif // GEODATAPOLYDATA_EDITNAMEANDVALUECOMMAND_H
