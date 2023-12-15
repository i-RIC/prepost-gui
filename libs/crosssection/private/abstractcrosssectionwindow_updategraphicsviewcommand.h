#ifndef ABSTRACTCROSSSECTIONWINDOW_UPDATEGRAPHICSVIEWCOMMAND_H
#define ABSTRACTCROSSSECTIONWINDOW_UPDATEGRAPHICSVIEWCOMMAND_H

#include "../abstractcrosssectionwindow.h"

#include <QUndoCommand>

#include <memory>

class AbstractCrosssectionWindow::UpdateGraphicsViewCommand : public QUndoCommand
{
public:
	UpdateGraphicsViewCommand(QUndoCommand* command, AbstractCrosssectionWindow* window);

	void redo() override;
	void undo() override;

	int id() const override;
	bool mergeWith(const QUndoCommand* other) override;

private:
	std::unique_ptr<QUndoCommand> m_command;
	AbstractCrosssectionWindow* m_window;
};

#endif // ABSTRACTCROSSSECTIONWINDOW_UPDATEGRAPHICSVIEWCOMMAND_H
