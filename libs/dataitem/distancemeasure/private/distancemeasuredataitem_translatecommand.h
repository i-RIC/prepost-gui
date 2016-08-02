#ifndef DISTANCEMEASUREDATAITEM_TRANSLATECOMMAND_H
#define DISTANCEMEASUREDATAITEM_TRANSLATECOMMAND_H

#include "../distancemeasuredataitem.h"

#include <QUndoCommand>
#include <QVector2D>

class DistanceMeasureDataItem::TranslateCommand : public QUndoCommand
{
public:
	TranslateCommand(const QVector2D& p1, const QVector2D& p2, bool finish, DistanceMeasureDataItem* item);

	void redo() override;
	void undo() override;

	int id() const override;
	bool mergeWith(const QUndoCommand *other) override;

private:
	void applyPoints(const QVector2D& p1, const QVector2D& p2);

	QVector2D m_newPoint1;
	QVector2D m_newPoint2;

	QVector2D m_oldPoint1;
	QVector2D m_oldPoint2;

	bool m_finish;

	DistanceMeasureDataItem* m_item;
};

#endif // DISTANCEMEASUREDATAITEM_TRANSLATECOMMAND_H
