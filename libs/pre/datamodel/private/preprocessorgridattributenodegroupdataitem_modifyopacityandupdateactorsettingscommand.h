#ifndef PREPROCESSORGRIDATTRIBUTENODEGROUPDATAITEM_MODIFYOPACITYANDUPDATEACTORSETTINGSCOMMAND_H
#define PREPROCESSORGRIDATTRIBUTENODEGROUPDATAITEM_MODIFYOPACITYANDUPDATEACTORSETTINGSCOMMAND_H

#include "../preprocessorgridattributenodegroupdataitem.h"

#include <QUndoCommand>

#include <memory>

class PreProcessorGridAttributeNodeGroupDataItem::ModifyOpacityAndUpdateActorSettingsCommand : public QUndoCommand
{
public:
	ModifyOpacityAndUpdateActorSettingsCommand(int opacityPercent, QUndoCommand* child, PreProcessorGridAttributeNodeGroupDataItem* item);

	void redo() override;
	void undo() override;

private:
	int m_newOpacityPercent;
	int m_oldOpacityPercent;

	std::unique_ptr<QUndoCommand> m_command;
	PreProcessorGridAttributeNodeGroupDataItem* m_item;
};

#endif // PREPROCESSORGRIDATTRIBUTENODEGROUPDATAITEM_MODIFYOPACITYANDUPDATEACTORSETTINGSCOMMAND_H
