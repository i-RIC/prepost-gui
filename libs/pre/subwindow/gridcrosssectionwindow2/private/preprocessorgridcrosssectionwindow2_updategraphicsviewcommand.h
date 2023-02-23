#ifndef PREPROCESSORGRIDCROSSSECTIONWINDOW2_UPDATEGRAPHICSVIEWCOMMAND_H
#define PREPROCESSORGRIDCROSSSECTIONWINDOW2_UPDATEGRAPHICSVIEWCOMMAND_H

#include "../preprocessorgridcrosssectionwindow2.h"

#include <QUndoCommand>

#include <memory>

class PreProcessorGridCrosssectionWindow2::UpdateGraphicsViewCommand : public QUndoCommand
{
public:
	UpdateGraphicsViewCommand(QUndoCommand* command, PreProcessorGridCrosssectionWindow2* window);

	void redo() override;
	void undo() override;

	int id() const override;
	bool mergeWith(const QUndoCommand* other) override;

private:
	std::unique_ptr<QUndoCommand> m_command;
	PreProcessorGridCrosssectionWindow2* m_window;
};

#endif // PREPROCESSORGRIDCROSSSECTIONWINDOW2_UPDATEGRAPHICSVIEWCOMMAND_H
