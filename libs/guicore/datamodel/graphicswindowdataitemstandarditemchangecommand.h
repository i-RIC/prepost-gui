#ifndef GRAPHICSWINDOWDATAITEMSTANDARDITEMCHANGECOMMAND_H
#define GRAPHICSWINDOWDATAITEMSTANDARDITEMCHANGECOMMAND_H

#include "../guicore_global.h"

class GraphicsWindowDataItem;

#include <QUndoCommand>
#include <QVariant>

class GraphicsWindowDataItem;

/// The class to store standard item change information.
class GUICOREDLL_EXPORT GraphicsWindowDataItemStandardItemChangeCommand : public QUndoCommand
{
public:
	GraphicsWindowDataItemStandardItemChangeCommand(GraphicsWindowDataItem* item);
	~GraphicsWindowDataItemStandardItemChangeCommand();

	void redo() override;
	void redoStandardItem() const;

	void undo() override;
	void undoStandardItem() const;

private:
	void applySetting(const QVariant& text, const QVariant& checkState) const;

	QVariant m_newDisplayText;
	QVariant m_newCheckState;

	QVariant m_oldDisplayText;
	QVariant m_oldCheckState;

	GraphicsWindowDataItem* m_item;
};

#endif // GRAPHICSWINDOWDATAITEMSTANDARDITEMCHANGECOMMAND_H
