#ifndef GEODATAPOLYDATAGROUP_EDITNAMEANDVALUECOMMAND_H
#define GEODATAPOLYDATAGROUP_EDITNAMEANDVALUECOMMAND_H

#include "../geodatapolydatagroup.h"

#include <QString>
#include <QUndoCommand>
#include <QVariant>

#include <vector>

class GeoDataPolyDataGroup::EditNameAndValueCommand : public QUndoCommand
{
public:
	EditNameAndValueCommand(const std::vector<QString>& names, const std::vector<QVariant>& values, const std::vector<GeoDataPolyDataGroupPolyData*> data, GeoDataPolyDataGroup* group);

	void redo() override;
	void undo() override;

private:
	void apply(const std::vector<QString>& names, const std::vector<QVariant>& values);

	std::vector<QString> m_newNames;
	std::vector<QVariant> m_newValues;

	std::vector<QString> m_oldNames;
	std::vector<QVariant> m_oldValues;

	std::vector<GeoDataPolyDataGroupPolyData*> m_data;
	GeoDataPolyDataGroup* m_group;
};

#endif // GEODATAPOLYDATAGROUP_EDITNAMEANDVALUECOMMAND_H
