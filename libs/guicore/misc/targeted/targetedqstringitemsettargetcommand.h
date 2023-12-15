#ifndef TARGETEDQSTRINGITEMSETTARGETCOMMAND_H
#define TARGETEDQSTRINGITEMSETTARGETCOMMAND_H

#include <QUndoCommand>

class TargetedQStringItemI;

class TargetedQStringItemSetTargetCommand : public QUndoCommand
{
public:
	TargetedQStringItemSetTargetCommand(const QString& newTarget, TargetedQStringItemI* item, const QString& commandName);
	~TargetedQStringItemSetTargetCommand();

	void redo() override;
	void undo() override;

private:
	QString m_newTarget;
	QString m_oldTarget;

	TargetedQStringItemI* m_item;
};

#endif // TARGETEDQSTRINGITEMSETTARGETCOMMAND_H
