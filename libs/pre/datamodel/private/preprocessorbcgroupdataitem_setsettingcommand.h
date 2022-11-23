#ifndef PREPROCESSORBCGROUPDATAITEM_SETSETTINGCOMMAND_H
#define PREPROCESSORBCGROUPDATAITEM_SETSETTINGCOMMAND_H

#include "../preprocessorbcgroupdataitem.h"

#include <guibase/vtktextpropertysettingcontainer.h>

#include <QUndoCommand>

class PreProcessorBCGroupDataItem::SetSettingCommand : public QUndoCommand
{
public:
	SetSettingCommand(const vtkTextPropertySettingContainer& setting, PreProcessorBCGroupDataItem* item);

	void redo() override;
	void undo() override;

private:
	void apply(const vtkTextPropertySettingContainer& setting);

	vtkTextPropertySettingContainer m_newNameSetting;
	vtkTextPropertySettingContainer m_oldNameSetting;

	PreProcessorBCGroupDataItem* m_item;
};

#endif // PREPROCESSORBCGROUPDATAITEM_SETSETTINGCOMMAND_H
