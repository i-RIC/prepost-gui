#ifndef PREPROCESSORGRIDATTRIBUTECELLGROUPDATAITEM_MODIFYOPACITYANDUPDATEACTORSETTINGSCOMMAND_H
#define PREPROCESSORGRIDATTRIBUTECELLGROUPDATAITEM_MODIFYOPACITYANDUPDATEACTORSETTINGSCOMMAND_H

#include "../preprocessorgridattributecellgroupdataitem.h"

#include <QUndoCommand>

#include <memory>

class PreProcessorGridAttributeCellGroupDataItem::ModifyOpacityAndUpdateActorSettingsCommand : public QUndoCommand
{
public:
	ModifyOpacityAndUpdateActorSettingsCommand(int opacityPercent, QUndoCommand* child, PreProcessorGridAttributeCellGroupDataItem* item);

	void redo() override;
	void undo() override;

private:
	int m_newOpacityPercent;
	int m_oldOpacityPercent;

	std::unique_ptr<QUndoCommand> m_command;
	PreProcessorGridAttributeCellGroupDataItem* m_item;
};

#endif // PREPROCESSORGRIDATTRIBUTECELLGROUPDATAITEM_MODIFYOPACITYANDUPDATEACTORSETTINGSCOMMAND_H
