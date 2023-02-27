#ifndef MERGESUPPORTEDLISTCOMMAND_H
#define MERGESUPPORTEDLISTCOMMAND_H

#include "misc_global.h"

#include <QUndoCommand>

#include <vector>

class MISCDLL_EXPORT MergeSupportedListCommand : public QUndoCommand
{
public:
	MergeSupportedListCommand(int id, bool allowMerge);
	~MergeSupportedListCommand();

	void addCommand(QUndoCommand* command);

	void redo() override;
	void undo() override;

	int id() const override;
	bool mergeWith(const QUndoCommand* other) override;

private:
	int m_id;
	bool m_allowMerge;

	std::vector<QUndoCommand*> m_commands;
};

#endif // MERGESUPPORTEDLISTCOMMAND_H
