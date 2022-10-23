#ifndef MEASUREDDATAPOINTGROUPDATAITEM_UPDATESETTINGCOMMAND_H
#define MEASUREDDATAPOINTGROUPDATAITEM_UPDATESETTINGCOMMAND_H

#include "../measureddatapointgroupdataitem.h"

#include <QUndoCommand>

class MeasuredDataPointSetting;

class MeasuredDataPointGroupDataItem::UpdateSettingCommand : public QUndoCommand
{
public:
	UpdateSettingCommand(const MeasuredDataPointSetting& s, ColorMapSettingContainer* cs, MeasuredDataPointGroupDataItem* item);

	void redo() override;
	void undo() override;

private:
	QUndoCommand* m_settingCommand;
	QUndoCommand* m_colormapCommand;

	MeasuredDataPointGroupDataItem* m_item;
};

#endif // MEASUREDDATAPOINTGROUPDATAITEM_UPDATESETTINGCOMMAND_H
