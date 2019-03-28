#ifndef POST2DWINDOWPOLYDATAGROUPDATAITEM_SETSETTINGCOMMAND_H
#define POST2DWINDOWPOLYDATAGROUPDATAITEM_SETSETTINGCOMMAND_H

#include "../post2dwindowpolydatagroupdataitem.h"

#include <guicore/scalarstocolors/lookuptablecontainer.h>

#include <QUndoCommand>

class Post2dWindowPolyDataGroupDataItem::SetSettingCommand : public QUndoCommand
{
public:
	SetSettingCommand(const ScalarSettingContainer& setting, const LookupTableContainer& ltc, int lineWidth, const QColor& color, const QString& colorbarTitle, Post2dWindowPolyDataGroupDataItem* item);

	void redo() override;
	void undo() override;

private:
	void applySetting(const ScalarSettingContainer& setting, const LookupTableContainer& lut, int lineWidth, const QColor& color, const QString& title);

	ScalarSettingContainer m_newSetting;
	LookupTableContainer m_newLookupTable;
	int m_newLineWidth;
	QColor m_newColor;
	QString m_newScalarBarTitle;

	ScalarSettingContainer m_oldSetting;
	LookupTableContainer m_oldLookupTable;
	int m_oldLineWidth;
	QColor m_oldColor;
	QString m_oldScalarBarTitle;

	Post2dWindowPolyDataGroupDataItem* m_item;
};

#endif // POST2DWINDOWPOLYDATAGROUPDATAITEM_SETSETTINGCOMMAND_H
