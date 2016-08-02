#ifndef DISTANCEMEASUREDATAITEM_MOVEVERTEXCOMMAND_H
#define DISTANCEMEASUREDATAITEM_MOVEVERTEXCOMMAND_H

#include "../distancemeasuredataitem.h"

#include <QUndoCommand>

class DistanceMeasureDataItem::MoveVertexCommand : public QUndoCommand
{
public:
	MoveVertexCommand(int point, const QVector2D& v, bool finish, DistanceMeasureDataItem* item);

	void redo() override;
	void undo() override;

	int id() const override;
	bool mergeWith(const QUndoCommand* other) override;

private:
	int m_point;
	QVector2D m_newPoint;
	QVector2D m_oldPoint;
	bool m_finish;
	DistanceMeasureDataItem* m_item;
};

#endif // DISTANCEMEASUREDATAITEM_MOVEVERTEXCOMMAND_H
