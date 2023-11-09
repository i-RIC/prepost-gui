#ifndef BACKGROUNDCOLOREDITI_SETBACKGROUNDCOLORCOMMAND_H
#define BACKGROUNDCOLOREDITI_SETBACKGROUNDCOLORCOMMAND_H

#include "../backgroundcolorediti.h"

#include <QColor>
#include <QUndoCommand>

class BackgroundColorEditI::SetBackgroundColorCommand : public QUndoCommand
{
public:
	SetBackgroundColorCommand(const QColor color, BackgroundColorEditI* target);

	void redo() override;
	void undo() override;

private:
	QColor m_newColor;
	QColor m_oldColor;

	BackgroundColorEditI* m_target;
};

#endif // BACKGROUNDCOLOREDITI_SETBACKGROUNDCOLORCOMMAND_H
