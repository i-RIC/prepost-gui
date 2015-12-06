#ifndef TARGETEDITEMSETTARGETCOMMAND_H
#define TARGETEDITEMSETTARGETCOMMAND_H

#include <QUndoCommand>

class TargetedItemI;

class TargetedItemSetTargetCommand : public QUndoCommand
{
public:
	TargetedItemSetTargetCommand(const std::string& newTarget, TargetedItemI* item, const QString& commandName);
	~TargetedItemSetTargetCommand();

	void redo() override;
	void undo() override;

private:
	std::string m_newTarget;
	std::string m_oldTarget;

	TargetedItemI* m_item;
};

#endif // TARGETEDITEMSETTARGETCOMMAND_H
