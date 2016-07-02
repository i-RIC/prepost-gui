#ifndef GRAPHICSWINDOWDATAITEM_MODIFYCOMMAND_H
#define GRAPHICSWINDOWDATAITEM_MODIFYCOMMAND_H

#include "../graphicswindowdataitem.h"

#include <QUndoCommand>

#include <memory>

class ProjectMainFile;

class GraphicsWindowDataItem::ModifyCommand : public QUndoCommand
{
public:
	explicit ModifyCommand(QUndoCommand *child, ProjectMainFile* main);
	~ModifyCommand();

	void redo() override;
	void undo() override;

	int id() const override;
	bool mergeWith(const QUndoCommand *other);

private:
	std::unique_ptr<QUndoCommand> m_command;
	ProjectMainFile* m_mainFile;
	bool m_wasModified;
};

#endif // GRAPHICSWINDOWDATAITEM_MODIFYCOMMAND_H
