#ifndef PREPROCESSORGRIDCROSSSECTIONWINDOW2_EDITTABLECONTROLLER_DELEGATE_H
#define PREPROCESSORGRIDCROSSSECTIONWINDOW2_EDITTABLECONTROLLER_DELEGATE_H

#include "preprocessorgridcrosssectionwindow2_edittablecontroller.h"

class PreProcessorGridCrosssectionWindow2::EditTableController::Delegate : public QItemDelegate
{
public:
	Delegate(EditTableController* controller);

	void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
	QWidget* createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;

private:
	EditTableController* m_controller;
};

#endif // PREPROCESSORGRIDCROSSSECTIONWINDOW2_EDITTABLECONTROLLER_DELEGATE_H
