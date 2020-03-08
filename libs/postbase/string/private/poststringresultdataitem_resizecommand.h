#ifndef POSTSTRINGRESULTDATAITEM_RESIZECOMMAND_H
#define POSTSTRINGRESULTDATAITEM_RESIZECOMMAND_H

#include "../poststringresultdataitem.h"

#include <QUndoCommand>

class PostStringResultDataItem::ResizeCommand : public QUndoCommand
{
public:
	ResizeCommand(bool keyDown, const QPoint& from, const QPoint& to, PostStringResultDataItem* item);

	void redo() override;
	void undo() override;

	int id() const override;
	bool mergeWith(const QUndoCommand *other) override;

private:
	bool m_keyDown;
	int m_dw;
	int m_dh;
	bool m_oldAuto;
	PostStringResultDataItem* m_item;
};

#endif // POSTSTRINGRESULTDATAITEM_RESIZECOMMAND_H
