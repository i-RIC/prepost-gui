#ifndef GRAPHICSWINDOWDRAWCOMMANDS_H
#define GRAPHICSWINDOWDRAWCOMMANDS_H

#include "../guicore_global.h"
#include <QUndoCommand>

class GraphicsWindowDataItem;

class GUICOREDLL_EXPORT GraphicsWindowDrawOnRedo : public QUndoCommand
{
public:
	GraphicsWindowDrawOnRedo(GraphicsWindowDataItem* item);
	void undo()
	{}
	void redo() override;

private:
	GraphicsWindowDataItem* m_item;
};

class GUICOREDLL_EXPORT GraphicsWindowDrawOnUndo : public QUndoCommand
{
public:
	GraphicsWindowDrawOnUndo(GraphicsWindowDataItem* item);
	void undo() override;
	void redo()
	{}

private:
	GraphicsWindowDataItem* m_item;
};

#endif // GRAPHICSWINDOWDRAWCOMMANDS_H
