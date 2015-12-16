#ifndef POST3DWINDOWCONTOURGROUPDATAITEM_SETSETTINGCOMMAND_H
#define POST3DWINDOWCONTOURGROUPDATAITEM_SETSETTINGCOMMAND_H

#include "../post3dwindowcontourgroupdataitem.h"

#include <QUndoCommand>

class Post3dWindowContourGroupDataItem::SetSettingCommand : public QUndoCommand
{
public:
	SetSettingCommand(const ScalarSettingContainer& scalarSetting, const LookupTableContainer& lookupTable, const QString& scalarBarTitle, Post3dWindowContourGroupDataItem* item);

	void redo() override;
	void undo() override;

private:
	void applySetting(const ScalarSettingContainer& scalarSetting, const LookupTableContainer& lut, const QString& scalarBarTitle);

	ScalarSettingContainer m_newScalarSetting;
	LookupTableContainer m_newLookupTable;
	QString m_newScalarBarTitle;

	ScalarSettingContainer m_oldScalarSetting;
	LookupTableContainer m_oldLookupTable;
	QString m_oldScalarBarTitle;

	Post3dWindowContourGroupDataItem* m_item;
};

#endif // POST3DWINDOWCONTOURGROUPDATAITEM_SETSETTINGCOMMAND_H
