#ifndef ABSTRACTCROSSSECTIONWINDOW_EDITTABLECONTROLLER_DELEGATE_H
#define ABSTRACTCROSSSECTIONWINDOW_EDITTABLECONTROLLER_DELEGATE_H

#include "abstractcrosssectionwindow_edittablecontroller.h"

class AbstractCrosssectionWindow::EditTableController::Delegate : public QItemDelegate
{
public:
	Delegate(EditTableController* controller);

	void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
	QWidget* createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;

private:
	EditTableController* m_controller;
};

#endif // ABSTRACTCROSSSECTIONWINDOW_EDITTABLECONTROLLER_DELEGATE_H
