#ifndef MEASUREDDATAPOINTGROUPDATAITEM_UPDATESETTINGCOMMAND_H
#define MEASUREDDATAPOINTGROUPDATAITEM_UPDATESETTINGCOMMAND_H

#include "../measureddatapointgroupdataitem.h"

#include <QUndoCommand>

class MeasuredDataPointSetting;

class MeasuredDataPointGroupDataItem::UpdateSettingCommand : public QUndoCommand
{
public:
	UpdateSettingCommand(bool allowMerge, const Setting& s, ColorMapSettingContainer* cs, MeasuredDataPointGroupDataItem* item);

	void redo() override;
	void undo() override;

	int id() const override;
	bool mergeWith(const QUndoCommand* other) override;

private:
	QUndoCommand* m_settingCommand;
	QUndoCommand* m_colormapCommand;

	bool m_allowMerge;
	MeasuredDataPointGroupDataItem* m_item;
};

#endif // MEASUREDDATAPOINTGROUPDATAITEM_UPDATESETTINGCOMMAND_H
