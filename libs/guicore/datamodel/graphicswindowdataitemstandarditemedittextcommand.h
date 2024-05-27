#ifndef GRAPHICSWINDOWDATAITEMSTANDARDITEMEDITTEXTCOMMAND_H
#define GRAPHICSWINDOWDATAITEMSTANDARDITEMEDITTEXTCOMMAND_H

#include "../guicore_global.h"

#include <QUndoCommand>

class GraphicsWindowDataItem;

class GUICOREDLL_EXPORT GraphicsWindowDataItemStandardItemEditTextCommand : public QUndoCommand
{
public:
	GraphicsWindowDataItemStandardItemEditTextCommand(const QString& text, GraphicsWindowDataItem* item);

	void redo() override;
	void undo() override;

private:
	void applyText(const QString& text);

	QString m_newText;
	QString m_oldText;

	GraphicsWindowDataItem* m_item;
};

#endif // GRAPHICSWINDOWDATAITEMSTANDARDITEMEDITTEXTCOMMAND_H
