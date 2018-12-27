#ifndef GEODATARIVERSURVEYCROSSSECTIONWINDOW_WSETABLEDELEGATE_H
#define GEODATARIVERSURVEYCROSSSECTIONWINDOW_WSETABLEDELEGATE_H

#include "../geodatariversurveycrosssectionwindow.h"

#include <QItemDelegate>

class GeoDataRiverSurveyCrosssectionWindow::WseTableDelegate : public QItemDelegate
{
public:
	WseTableDelegate(QObject* parent = nullptr);

	void setPoint(GeoDataRiverPathPoint* point);

	QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
	void setEditorData(QWidget* editor, const QModelIndex& index) const override;
	void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const override;
	void updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
	void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;

private:
	GeoDataRiverPathPoint* m_point;
};

#endif // GEODATARIVERSURVEYCROSSSECTIONWINDOW_WSETABLEDELEGATE_H
