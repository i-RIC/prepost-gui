#ifndef DISTANCEMEASUREDATAITEM_DEFINECOMMAND_H
#define DISTANCEMEASUREDATAITEM_DEFINECOMMAND_H

#include "../distancemeasuredataitem.h"

#include <QUndoCommand>

class DistanceMeasureDataItem::DefineCommand : public QUndoCommand
{
public:
	DefineCommand(const QPointF& v1, const QPointF& v2, bool finish, DistanceMeasureDataItem* item);

	void redo() override;
	void undo() override;

	int id() const override;
	bool mergeWith(const QUndoCommand* other) override;

private:
	void apply(const QPointF& p1, const QPointF& p2, DistanceMeasureDataItem::MouseEventMode mode, bool defined);

	QPointF m_newPoint1;
	QPointF m_newPoint2;

	QPointF m_oldPoint1;
	QPointF m_oldPoint2;

	bool m_finish;
	DistanceMeasureDataItem* m_item;
};

#endif // DISTANCEMEASUREDATAITEM_DEFINECOMMAND_H
