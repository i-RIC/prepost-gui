#ifndef POINTCLEARCOMMAND_H
#define POINTCLEARCOMMAND_H

#include "../guibase_global.h"

#include <QObject>
#include <QPointF>
#include <QUndoCommand>

class PointController;

class GUIBASEDLL_EXPORT PointClearCommand : public QObject, public QUndoCommand
{
public:
	PointClearCommand(PointController* point);
	PointClearCommand(const QString& title, PointController* point);

	void redo() override;
	void undo() override;

private:
	QPointF m_oldPoint;
	PointController* m_point;
};

#endif // POINTCLEARCOMMAND_H
