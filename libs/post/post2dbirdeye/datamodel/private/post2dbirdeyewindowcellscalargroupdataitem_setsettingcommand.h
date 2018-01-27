#ifndef POST2DBIRDEYEWINDOWCELLSCALARGROUPDATAITEM_SETSETTINGCOMMAND_H
#define POST2DBIRDEYEWINDOWCELLSCALARGROUPDATAITEM_SETSETTINGCOMMAND_H

#include "../post2dbirdeyewindowcellscalargroupdataitem.h"

#include <guicore/scalarstocolors/lookuptablecontainer.h>

#include <QString>
#include <QUndoCommand>

class Post2dBirdEyeWindowCellScalarGroupTopDataItem;

class Post2dBirdEyeWindowCellScalarGroupDataItem::SetSettingCommand : public QUndoCommand
{
public:
	SetSettingCommand(const Post2dWindowContourSetting& s, const LookupTableContainer& ltc, const QString& colorbarTitle, Post2dBirdEyeWindowCellScalarGroupDataItem* item, QUndoCommand* parent = nullptr);

	void redo() override;
	void undo() override;

private:
	void applySettings(const QString& target, const LookupTableContainer& c, const QString& title);

	Post2dWindowContourSetting m_newSetting;
	LookupTableContainer m_newLookupTable;
	QString m_newScalarBarTitle;

	Post2dWindowContourSetting m_oldSetting;
	LookupTableContainer m_oldLookupTable;
	QString m_oldScalarBarTitle;

	Post2dBirdEyeWindowCellScalarGroupDataItem* m_item;
	Post2dBirdEyeWindowCellScalarGroupTopDataItem* m_topItem;
};

#endif // POST2DBIRDEYEWINDOWCELLSCALARGROUPDATAITEM_SETSETTINGCOMMAND_H
