#include "iricmimainwindow_modelstabledelegate.h"

#include <QPainter>
#include <QPushButton>

namespace {

void paintPushButton(QPainter* painter, const QStyleOptionViewItem& option)
{
	QPushButton button("Open");
	button.resize(option.rect.size());

	QPixmap pixmap(option.rect.size());
	button.render(&pixmap);
	painter->drawPixmap(option.rect, pixmap);
}

} // namespace

iRICMIMainWindow::ModelsTableDelegate::ModelsTableDelegate(QObject* parent) :
	QItemDelegate {parent}
{}

void iRICMIMainWindow::ModelsTableDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	if (index.column() == 3) {
		paintPushButton(painter, option);
	} else {
		QItemDelegate::paint(painter, option, index);
	}
}
