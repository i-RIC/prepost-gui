#ifndef ABSTRACTCROSSSECTIONWINDOW_DISPLAYSETTINGTABLECONTROLLER_DELEGATE_H
#define ABSTRACTCROSSSECTIONWINDOW_DISPLAYSETTINGTABLECONTROLLER_DELEGATE_H

#include "abstractcrosssectionwindow_displaysettingtablecontroller.h"

#include <QItemDelegate>

class AbstractCrosssectionWindow::DisplaySettingTableController::Delegate : public QItemDelegate
{
public:
	Delegate(QObject* parent);

	void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
};

#endif // ABSTRACTCROSSSECTIONWINDOW_DISPLAYSETTINGTABLECONTROLLER_DELEGATE_H
