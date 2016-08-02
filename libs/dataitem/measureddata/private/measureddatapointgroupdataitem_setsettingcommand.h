#ifndef MEASUREDDATAPOINTGROUPDATAITEM_SETSETTINGCOMMAND_H
#define MEASUREDDATAPOINTGROUPDATAITEM_SETSETTINGCOMMAND_H

#include "../measureddatapointgroupdataitem.h"

#include <guicore/scalarstocolors/lookuptablecontainer.h>

#include <QUndoCommand>

class MeasuredDataPointGroupDataItem::SetSettingCommand : public QUndoCommand
{
public:
	SetSettingCommand(const MeasuredDataPointSetting& setting, const LookupTableContainer& lut, const QString& title, MeasuredDataPointGroupDataItem* item);

	void redo() override;
	void undo() override;

private:
	void applySetting(const MeasuredDataPointSetting& setting, LookupTableContainer& lut, const QString& title);

	MeasuredDataPointSetting m_newSetting;
	LookupTableContainer m_newLookupTable;
	QString m_newScalarBarTitle;

	MeasuredDataPointSetting m_oldSetting;
	LookupTableContainer m_oldLookupTable;
	QString m_oldScalarBarTitle;

	MeasuredDataPointGroupDataItem* m_item;
};

#endif // MEASUREDDATAPOINTGROUPDATAITEM_SETSETTINGCOMMAND_H
