#ifndef GRAPH2DWINDOWDATAITEM_STANDARDITEMCHANGECOMMAND_H
#define GRAPH2DWINDOWDATAITEM_STANDARDITEMCHANGECOMMAND_H

#include "../graph2dwindowdataitem.h"

#include <QStandardItem>
#include <QUndoCommand>
#include <QVariant>

class Graph2dWindowDataItem::StandardItemChangeCommand : public QUndoCommand
{
public:
	StandardItemChangeCommand(Graph2dWindowDataItem* item);

	void redo() override;
	void undo() override;

private:
	QVariant m_oldDisplayText;
	QVariant m_oldCheckState;

	QVariant m_newDisplayText;
	QVariant m_newCheckState;

	Graph2dWindowDataItem* m_item;
};

#endif // GRAPH2DWINDOWDATAITEM_STANDARDITEMCHANGECOMMAND_H
