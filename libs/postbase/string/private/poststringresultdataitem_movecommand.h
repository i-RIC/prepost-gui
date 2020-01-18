#ifndef POSTSTRINGRESULTDATAITEM_MOVECOMMAND_H
#define POSTSTRINGRESULTDATAITEM_MOVECOMMAND_H

#include "../poststringresultdataitem.h"

#include <QUndoCommand>

class PostStringResultDataItem::MoveCommand : public QUndoCommand
{
public:
	MoveCommand(bool keyDown, const QPoint& from, const QPoint& to, PostStringResultDataItem* item);

	void redo() override;
	void undo() override;

	int id() const override;
	bool mergeWith(const QUndoCommand *other) override;

private:
	bool m_keyDown;
	int m_dx;
	int m_dy;
	PostStringResultDataItem* m_item;
};

#endif // POSTSTRINGRESULTDATAITEM_MOVECOMMAND_H
