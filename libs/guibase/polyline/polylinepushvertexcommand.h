#ifndef POLYLINEPUSHVERTEXCOMMAND_H
#define POLYLINEPUSHVERTEXCOMMAND_H

#include "../guibase_global.h"

#include <QObject>
#include <QPointF>
#include <QUndoCommand>

class PolyLineController;

class GUIBASEDLL_EXPORT PolyLinePushVertexCommand : public QObject, public QUndoCommand
{
	Q_OBJECT

public:
	PolyLinePushVertexCommand(bool keyDown, const QPointF& point, PolyLineController* line);
	PolyLinePushVertexCommand(const QString& title, bool keyDown, const QPointF& point, PolyLineController* line);

	void redo() override;
	void undo() override;

	int id() const override;
	bool mergeWith(const QUndoCommand* other) override;

private:
	bool m_keyDown;
	QPointF m_point;
	PolyLineController* m_polyLine;
};

#endif // POLYLINEPUSHVERTEXCOMMAND_H
