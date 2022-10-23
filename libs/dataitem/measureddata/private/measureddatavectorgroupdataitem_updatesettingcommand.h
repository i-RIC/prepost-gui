#ifndef MEASUREDDATAVECTORGROUPDATAITEM_UPDATESETTINGCOMMAND_H
#define MEASUREDDATAVECTORGROUPDATAITEM_UPDATESETTINGCOMMAND_H

#include "../measureddatavectorgroupdataitem.h"
#include "../measureddatavectorsetting.h"

#include <QUndoCommand>

class MeasuredDataVectorGroupDataItem::UpdateSettingCommand : public QUndoCommand
{
public:
	UpdateSettingCommand(QUndoCommand* arrowsCommand, QUndoCommand* filterCommand, MeasuredDataVectorGroupDataItem* item);

	void redo() override;
	void undo() override;

	QUndoCommand* m_arrowsCommand;
	QUndoCommand* m_filterCommand;

	MeasuredDataVectorGroupDataItem* m_item;
};

#endif // MEASUREDDATAVECTORGROUPDATAITEM_UPDATESETTINGCOMMAND_H
