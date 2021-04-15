#ifndef POST3DWINDOWCELLCONTOURGROUPDATAITEM_SETSETTINGCOMMAND_H
#define POST3DWINDOWCELLCONTOURGROUPDATAITEM_SETSETTINGCOMMAND_H

#include "../post3dwindowcellcontourgroupdataitem.h"

#include <guicore/scalarstocolors/lookuptablecontainer.h>

#include <QUndoCommand>

class Post3dWindowCellContourGroupDataItem::SetSettingCommand : public QUndoCommand
{
public:
	SetSettingCommand(const ScalarSettingContainer& scalarSetting, const std::vector<Post3dCellRangeSettingContainer>& rangeSettings, const LookupTableContainer& lookupTable, QString& scalarBarTitle, Post3dWindowCellContourGroupDataItem* item);
	~SetSettingCommand();

	void redo() override;
	void undo() override;

private:
	void applySetting(const ScalarSettingContainer& scalarSetting, const std::vector<Post3dCellRangeSettingContainer> rangeSettings, const LookupTableContainer& lookupTable, const QString& scalarBarTitle);

	ScalarSettingContainer m_newScalarSetting;
	std::vector<Post3dCellRangeSettingContainer> m_newRangeSettings;
	LookupTableContainer m_newLookupTable;
	QString m_newScalarBarTitle;

	ScalarSettingContainer m_oldScalarSetting;
	std::vector<Post3dCellRangeSettingContainer> m_oldRangeSettings;
	LookupTableContainer m_oldLookupTable;
	QString m_oldScalarBarTitle;

	Post3dWindowCellContourGroupDataItem* m_item;
};

#endif // POST3DWINDOWCELLCONTOURGROUPDATAITEM_SETSETTINGCOMMAND_H
