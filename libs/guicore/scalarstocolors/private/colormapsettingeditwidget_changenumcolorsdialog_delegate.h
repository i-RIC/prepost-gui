#ifndef COLORMAPSETTINGEDITWIDGET_CHANGENUMCOLORSDIALOG_DELEGATE_H
#define COLORMAPSETTINGEDITWIDGET_CHANGENUMCOLORSDIALOG_DELEGATE_H

#include "colormapsettingeditwidget_changenumcolorsdialog.h"

#include <QItemDelegate>

class ColorMapSettingEditWidget::ChangeNumColorsDialog::Delegate : public QItemDelegate
{
public:
	Delegate(QObject* parent = nullptr);

	void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
};

#endif // COLORMAPSETTINGEDITWIDGET_CHANGENUMCOLORSDIALOG_DELEGATE_H
