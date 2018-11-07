#ifndef GEODATARIVERSURVEYCROSSSECTIONWINDOW_RIVERSURVEYTABLEDELEGATE_H
#define GEODATARIVERSURVEYCROSSSECTIONWINDOW_RIVERSURVEYTABLEDELEGATE_H

#include "../geodatariversurveycrosssectionwindow.h"

#include <QItemDelegate>

class GeoDataRiverSurveyCrosssectionWindow::RiverSurveyTableDelegate : public QItemDelegate
{
public:
	RiverSurveyTableDelegate(QObject* parent = nullptr);

	void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
};

#endif // GEODATARIVERSURVEYCROSSSECTIONWINDOW_RIVERSURVEYTABLEDELEGATE_H
