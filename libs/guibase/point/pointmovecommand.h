#ifndef POINTMOVECOMMAND_H
#define POINTMOVECOMMAND_H

#include "../guibase_global.h"

#include <QObject>
#include <QPointF>
#include <QUndoCommand>

class PointController;

class GUIBASEDLL_EXPORT PointMoveCommand : public QObject, public QUndoCommand
{
public:
	PointMoveCommand(bool keyDown, const QPointF& p, PointController* point);
	PointMoveCommand(const QString& title, bool keyDown, const QPointF& p, PointController* point);

	void redo() override;
	void undo() override;

	int id() const override;
	bool mergeWith(const QUndoCommand *other) override;

private:
	bool m_keyDown;
	QPointF m_newPoint;
	QPointF m_oldPoint;
	PointController* m_point;
};

#endif // POINTMOVECOMMAND_H
