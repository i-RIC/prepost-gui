#ifndef POST2DBIRDEYEWINDOWNODESCALARGROUPDATAITEM_SETSETTINGCOMMAND_H
#define POST2DBIRDEYEWINDOWNODESCALARGROUPDATAITEM_SETSETTINGCOMMAND_H

#include "../post2dbirdeyewindownodescalargroupdataitem.h"

#include <guicore/scalarstocolors/lookuptablecontainer.h>

#include <QString>
#include <QUndoCommand>

class Post2dBirdEyeWindowNodeScalarGroupTopDataItem;

class Post2dBirdEyeWindowNodeScalarGroupDataItem::SetSettingCommand : public QUndoCommand
{
public:
	SetSettingCommand(const Post2dBirdEyeWindowContourSetting& s, const LookupTableContainer& ltc, const QString& colorbarTitle, Post2dBirdEyeWindowNodeScalarGroupDataItem* item, QUndoCommand* parent = nullptr);

	void redo() override;
	void undo() override;

private:
	void applySettings(const QString& target, const LookupTableContainer& c, const QString& title);

	Post2dBirdEyeWindowContourSetting m_newSetting;
	LookupTableContainer m_newLookupTable;
	QString m_newScalarBarTitle;

	Post2dBirdEyeWindowContourSetting m_oldSetting;
	LookupTableContainer m_oldLookupTable;
	QString m_oldScalarBarTitle;

	Post2dBirdEyeWindowNodeScalarGroupDataItem* m_item;
	Post2dBirdEyeWindowNodeScalarGroupTopDataItem* m_topItem;
};

#endif // SETSETTINGCOMMAND_H
