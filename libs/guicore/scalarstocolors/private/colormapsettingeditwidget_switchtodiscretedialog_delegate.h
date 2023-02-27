#ifndef COLORMAPSETTINGEDITWIDGET_SWITCHTODISCRETEDIALOG_DELEGATE_H
#define COLORMAPSETTINGEDITWIDGET_SWITCHTODISCRETEDIALOG_DELEGATE_H

#include "colormapsettingeditwidget_switchtodiscretedialog.h"

#include <QItemDelegate>

class ColorMapSettingEditWidget::SwitchToDiscreteDialog::Delegate : public QItemDelegate
{
public:
	Delegate(QObject* parent = nullptr);

	void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
};

#endif // COLORMAPSETTINGEDITWIDGET_SWITCHTODISCRETEDIALOG_DELEGATE_H
