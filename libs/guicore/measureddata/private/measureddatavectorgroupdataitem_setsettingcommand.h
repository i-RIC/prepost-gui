#ifndef MEASUREDDATAVECTORGROUPDATAITEM_SETSETTINGCOMMAND_H
#define MEASUREDDATAVECTORGROUPDATAITEM_SETSETTINGCOMMAND_H

#include "../measureddatavectorgroupdataitem.h"

#include <QUndoCommand>

class MeasuredDataVectorGroupDataItem::SetSettingCommand : public QUndoCommand
{
public:
	SetSettingCommand(MeasuredDataVectorSetting& setting, MeasuredDataVectorGroupDataItem* item);

	void redo() override;
	void undo() override;

private:
	void applySetting(const MeasuredDataVectorSetting& s);

	MeasuredDataVectorSetting m_newSetting;
	MeasuredDataVectorSetting m_oldSetting;

	MeasuredDataVectorGroupDataItem* m_item;
};

#endif // MEASUREDDATAVECTORGROUPDATAITEM_SETSETTINGCOMMAND_H
