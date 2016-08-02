#ifndef DISTANCEMEASUREDATAITEM_REDEFINECOMMAND_H
#define DISTANCEMEASUREDATAITEM_REDEFINECOMMAND_H

#include "../distancemeasuredataitem.h"

#include <QUndoCommand>

class DistanceMeasureDataItem::RedefineCommand : public QUndoCommand
{
public:
	RedefineCommand(DistanceMeasureDataItem* item);

	void redo() override;
	void undo() override;

private:
	DistanceMeasureDataItem* m_item;
};

#endif // DISTANCEMEASUREDATAITEM_REDEFINECOMMAND_H
