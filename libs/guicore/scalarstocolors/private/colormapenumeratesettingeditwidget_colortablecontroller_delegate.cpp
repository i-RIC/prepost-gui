#include "colormapenumeratesettingeditwidget_colortablecontroller_delegate.h"

#include <guibase/widget/centeredcheckbox.h>

namespace {
	void paintColor(QPainter* painter, const QColor& col, const QStyleOptionViewItem& option)
	{
		QBrush brush(col);
		painter->fillRect(option.rect, brush);
	}

	void paintCheckBox(QPainter* painter, bool value, const QStyleOptionViewItem& option)
	{
		CenteredCheckBox checkbox;
		checkbox.setChecked(value);
		checkbox.resize(option.rect.size());

		QPixmap pixmap(option.rect.size());
		checkbox.render(&pixmap);
		painter->drawPixmap(option.rect, pixmap);
	}

	QVariant variantData(const QModelIndex& index)
	{
		auto m = index.model();
		return m->data(index, Qt::EditRole);
	}

} // namespace


ColorMapEnumerateSettingEditWidget::ColorTableController::Delegate::Delegate(ColorTableController* controller, QObject* parent) :
	QItemDelegate {parent},
	m_controller {controller}
{}

void ColorMapEnumerateSettingEditWidget::ColorTableController::Delegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	auto s = setting();

	if (index.column() == 0) {
		QItemDelegate::paint(painter, option, index);
	} else if (index.column() == 1) {
		paintColor(painter, variantData(index).value<QColor>(), option);
	} else if (index.column() == 2) {
		paintCheckBox(painter, variantData(index).value<bool>(), option);
	}
}

ColorMapEnumerateSettingContainer* ColorMapEnumerateSettingEditWidget::ColorTableController::Delegate::setting() const
{
	return &m_controller->m_widget->m_concreteSetting;
}
