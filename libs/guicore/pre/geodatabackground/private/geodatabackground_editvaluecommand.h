#ifndef GEODATABACKGROUND_EDITVALUECOMMAND_H
#define GEODATABACKGROUND_EDITVALUECOMMAND_H

#include "../geodatabackground.h"

#include <QUndoCommand>

class GeoDataBackground::EditValueCommand : public QUndoCommand
{
public:
	EditValueCommand(GeoDataBackground::Type type, const QVariant& val, GeoDataBackground* b);

	void redo() override;
	void undo() override;

private:
	void applyValues(GeoDataBackground::Type t, const QVariant& val, bool mapped);

	GeoDataBackground::Type m_newType;
	QVariant m_newValue;

	GeoDataBackground::Type m_oldType;
	QVariant m_oldValue;
	bool m_oldMapped;

	GeoDataBackground* m_background;
};

#endif // GEODATABACKGROUND_EDITVALUECOMMAND_H
