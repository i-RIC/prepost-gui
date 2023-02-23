#ifndef PREPROCESSORGRIDCROSSSECTIONWINDOW2_DISPLAYSETTINGTABLECONTROLLER_DELEGATE_H
#define PREPROCESSORGRIDCROSSSECTIONWINDOW2_DISPLAYSETTINGTABLECONTROLLER_DELEGATE_H

#include "preprocessorgridcrosssectionwindow2_displaysettingtablecontroller.h"

#include <QItemDelegate>

class PreProcessorGridCrosssectionWindow2::DisplaySettingTableController::Delegate : public QItemDelegate
{
public:
	Delegate(QObject* parent);

	void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
};

#endif // PREPROCESSORGRIDCROSSSECTIONWINDOW2_DISPLAYSETTINGTABLECONTROLLER_DELEGATE_H
