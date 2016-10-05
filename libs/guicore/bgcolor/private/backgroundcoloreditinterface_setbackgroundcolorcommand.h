#ifndef SETBACKGROUNDCOLORCOMMAND_H
#define SETBACKGROUNDCOLORCOMMAND_H

#include "../backgroundcoloreditinterface.h"

#include <QColor>
#include <QUndoCommand>

class BackgroundColorEditInterface::SetBackgroundColorCommand : public QUndoCommand
{
public:
	SetBackgroundColorCommand(const QColor color, BackgroundColorEditInterface* target);

	void redo() override;
	void undo() override;

private:
	QColor m_newColor;
	QColor m_oldColor;

	BackgroundColorEditInterface* m_target;
};

#endif // SETBACKGROUNDCOLORCOMMAND_H
