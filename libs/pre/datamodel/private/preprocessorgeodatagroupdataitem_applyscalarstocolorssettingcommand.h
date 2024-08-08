#ifndef PREPROCESSORGEODATAGROUPDATAITEM_APPLYSCALARSTOCOLORSSETTINGCOMMAND_H
#define PREPROCESSORGEODATAGROUPDATAITEM_APPLYSCALARSTOCOLORSSETTINGCOMMAND_H

#include "../preprocessorgeodatagroupdataitem.h"

#include <QUndoCommand>

class PreProcessorGeoDataGroupDataItem::ApplyScalarsToColorsSettingCommand : public QUndoCommand
{
public:
	ApplyScalarsToColorsSettingCommand(PreProcessorGeoDataGroupDataItem* item);

	void redo() override;
	void undo() override;

	int id() const override;
	bool mergeWith(const QUndoCommand* other) override;

private:
	PreProcessorGeoDataGroupDataItem* m_item;
};

#endif // PREPROCESSORGEODATAGROUPDATAITEM_APPLYSCALARSTOCOLORSSETTINGCOMMAND_H
