#ifndef DISTANCEMEASUREDATAITEM_SETSETTINGCOMMAND_H
#define DISTANCEMEASUREDATAITEM_SETSETTINGCOMMAND_H

#include "../distancemeasuredataitem.h"
#include "../distancemeasuresetting.h"

#include <QUndoCommand>

class DistanceMeasureDataItem::SetSettingCommand : public QUndoCommand
{
public:
	SetSettingCommand(const QString& name, const DistanceMeasureSetting& setting, DistanceMeasureDataItem* item);

	void redo() override;
	void undo() override;

private:
	QString m_newName;
	DistanceMeasureSetting m_newSetting;

	QString m_oldName;
	DistanceMeasureSetting m_oldSetting;

	DistanceMeasureDataItem* m_item;
};

#endif // DISTANCEMEASUREDATAITEM_SETSETTINGCOMMAND_H
