#include "preprocessorgridcrosssectionwindow2_displaysettingtablecontroller_delegate.h"

#include <guibase/widget/centeredcheckbox.h>

#include <QPushButton>

namespace {
	void paintCheckBox(QPainter* painter, bool value, const QStyleOptionViewItem& option)
	{
		CenteredCheckBox checkbox;
		checkbox.setChecked(value);
		checkbox.resize(option.rect.size());

		QPixmap pixmap(option.rect.size());
		checkbox.render(&pixmap);
		painter->drawPixmap(option.rect, pixmap);
	}

	void paintSettingButton(QPainter* painter, const QStyleOptionViewItem& option)
	{
		QPushButton button(PreProcessorGridCrosssectionWindow2::tr("Setting"));
		button.resize(option.rect.size());

		QPixmap pixmap(option.rect.size());
		button.render(&pixmap);
		painter->drawPixmap(option.rect, pixmap);
	}

	QVariant variantData(const QModelIndex& index)
	{
		auto m = index.model();
		return m->data(index, Qt::EditRole);
	}
} // namespace


PreProcessorGridCrosssectionWindow2::DisplaySettingTableController::Delegate::Delegate(QObject* parent) :
	QItemDelegate {parent}
{}

void PreProcessorGridCrosssectionWindow2::DisplaySettingTableController::Delegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	auto col = index.column();
	if (col == 0) {
		paintCheckBox(painter, variantData(index).value<bool>(), option);
	} else if (col == 1) {
		QItemDelegate::paint(painter, option, index);
	} else if (col == 2) {
		paintSettingButton(painter, option);
	}
}
