#ifndef POST3DWINDOWCELLCONTOURGROUPTOPDATAITEM_CREATECOMMAND_H
#define POST3DWINDOWCELLCONTOURGROUPTOPDATAITEM_CREATECOMMAND_H

#include "../post3dwindowcellcontourgrouptopdataitem.h"
#include "../../post3dcellrangesettingcontainer.h"

#include <guibase/scalarsettingcontainer.h>
#include <guicore/scalarstocolors/lookuptablecontainer.h>

#include <QUndoCommand>

class Post3dWindowCellContourGroupDataItem;

class Post3dWindowCellContourGroupTopDataItem::CreateCommand : public QUndoCommand
{
public:
	CreateCommand(const ScalarSettingContainer& scalarSetting, const std::vector<Post3dCellRangeSettingContainer>& rangeSettings, const LookupTableContainer& lookupTable, QString& scalarBarTitle, Post3dWindowCellContourGroupTopDataItem* item);
	~CreateCommand();

	void redo() override;
	void undo() override;

private:
	void updateScalarBarData(const LookupTableContainer& lookupTable, const QString& scalarBarTitle);

	ScalarSettingContainer m_scalarSetting;
	std::vector<Post3dCellRangeSettingContainer> m_rangeSettings;

	LookupTableContainer m_newLookupTable;
	QString m_newScalarBarTitle;

	LookupTableContainer m_oldLookupTable;
	QString m_oldScalarBarTitle;

	Post3dWindowCellContourGroupDataItem* m_newItem;
	Post3dWindowCellContourGroupTopDataItem* m_item;
};

#endif // POST3DWINDOWCELLCONTOURGROUPTOPDATAITEM_CREATECOMMAND_H
