#ifndef DISTANCEMEASUREDATAITEM_DEFINECOMMAND_H
#define DISTANCEMEASUREDATAITEM_DEFINECOMMAND_H

#include "../distancemeasuredataitem.h"

#include <QUndoCommand>

class DistanceMeasureDataItem::DefineCommand : public QUndoCommand
{
public:
	DefineCommand(const QVector2D& v1, const QVector2D& v2, bool finish, DistanceMeasureDataItem* item);

	void redo() override;
	void undo() override;

	int id() const override;
	bool mergeWith(const QUndoCommand* other) override;

private:
	QVector2D m_newPoint1;
	QVector2D m_newPoint2;

	QVector2D m_oldPoint1;
	QVector2D m_oldPoint2;

	bool m_finish;
	DistanceMeasureDataItem* m_item;
};

#endif // DISTANCEMEASUREDATAITEM_DEFINECOMMAND_H
