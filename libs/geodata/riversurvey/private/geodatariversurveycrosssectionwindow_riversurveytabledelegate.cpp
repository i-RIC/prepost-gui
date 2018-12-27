#include "geodatariversurveycrosssectionwindow_riversurveytabledelegate.h"

#include <guibase/widget/centeredcheckbox.h>

#include <QPainter>

GeoDataRiverSurveyCrosssectionWindow::RiverSurveyTableDelegate::RiverSurveyTableDelegate(QObject* parent) :
	QItemDelegate(parent)
{}

void GeoDataRiverSurveyCrosssectionWindow::RiverSurveyTableDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	if (index.column() == 0) {
		QVariant dat = index.model()->data(index, Qt::DisplayRole);
		CenteredCheckBox checkbox(nullptr);
		checkbox.setChecked(dat.toBool());
		checkbox.resize(option.rect.size());
		QPixmap pixmap(option.rect.size());
		checkbox.render(&pixmap);
		painter->drawPixmap(option.rect, pixmap);
	} else if (index.column() == 2) {
		QVariant dat = index.model()->data(index, Qt::DisplayRole);
		QColor col = dat.value<QColor>();
		QBrush brush(col);
		painter->fillRect(option.rect, brush);
	} else {
		QItemDelegate::paint(painter, option, index);
	}
}
