#ifndef GEODATAPOLYDATAGROUP_SORTEDITTARGETDATACOMMAND_H
#define GEODATAPOLYDATAGROUP_SORTEDITTARGETDATACOMMAND_H

#include "../geodatapolydatagroup.h"

#include <QUndoCommand>

class GeoDataPolyDataGroup::SortEditTargetDataCommand : public QUndoCommand
{
public:
	SortEditTargetDataCommand(int newIndex, GeoDataPolyDataGroup* group);

	void redo() override;
	void undo() override;

private:
	int m_newIndex;
	int m_oldIndex;

	GeoDataPolyDataGroup* m_group;
};

#endif // GEODATAPOLYDATAGROUP_SORTEDITTARGETDATACOMMAND_H
