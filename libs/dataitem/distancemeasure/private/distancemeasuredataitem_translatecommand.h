#ifndef DISTANCEMEASUREDATAITEM_TRANSLATECOMMAND_H
#define DISTANCEMEASUREDATAITEM_TRANSLATECOMMAND_H

#include "../distancemeasuredataitem.h"

#include <QUndoCommand>

class DistanceMeasureDataItem::TranslateCommand : public QUndoCommand
{
public:
	TranslateCommand(const QPointF& p1, const QPointF& p2, bool finish, DistanceMeasureDataItem* item);

	void redo() override;
	void undo() override;

	int id() const override;
	bool mergeWith(const QUndoCommand *other) override;

private:
	void applyPoints(const QPointF& p1, const QPointF& p2);

	QPointF m_newPoint1;
	QPointF m_newPoint2;

	QPointF m_oldPoint1;
	QPointF m_oldPoint2;

	bool m_finish;

	DistanceMeasureDataItem* m_item;
};

#endif // DISTANCEMEASUREDATAITEM_TRANSLATECOMMAND_H
