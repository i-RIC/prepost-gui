#include "colormapsettingeditwidget_switchtodiscretedialog_delegate.h"

namespace {
	void paintColor(QPainter* painter, const QColor& col, const QStyleOptionViewItem& option)
	{
		QBrush brush(col);
		painter->fillRect(option.rect, brush);
	}

	QVariant variantData(const QModelIndex& index)
	{
		auto m = index.model();
		return m->data(index, Qt::EditRole);
	}

} // namespace

ColorMapSettingEditWidget::SwitchToDiscreteDialog::Delegate::Delegate(QObject* parent) :
	QItemDelegate {parent}
{}

void ColorMapSettingEditWidget::SwitchToDiscreteDialog::Delegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	if (index.column() == 2) {
		paintColor(painter, variantData(index).value<QColor>(), option);
	} else {
		QItemDelegate::paint(painter, option, index);
	}
}
