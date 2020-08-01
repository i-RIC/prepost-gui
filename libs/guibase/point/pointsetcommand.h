#ifndef POINTSETCOMMAND_H
#define POINTSETCOMMAND_H

#include "../guibase_global.h"

#include <QObject>
#include <QPointF>
#include <QUndoCommand>

class GUIBASEDLL_EXPORT PointSetCommand : public QObject, public QUndoCommand
{
public:
	PointSetCommand(bool keyDown, const QPointF& p, PointController* point);
	PointSetCommand(const QString& title, bool keyDown, const QPointF& p, PointController* point);

	void redo() override;
	void undo() override;

	int id() const override;
	bool mergeWith(const QUndoCommand *other) override;

private:
	bool m_keyDown;
	QPointF m_newPoint;
	PointController* m_point;
};

#endif // POINTSETCOMMAND_H
