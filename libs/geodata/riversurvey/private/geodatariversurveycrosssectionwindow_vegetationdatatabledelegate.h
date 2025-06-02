#ifndef GEODATARIVERSURVEYCROSSSECTIONWINDOW_VEGETATIONDATATABLEDELEGATE_H
#define GEODATARIVERSURVEYCROSSSECTIONWINDOW_VEGETATIONDATATABLEDELEGATE_H

#include "../geodatariversurveycrosssectionwindow.h"

#include <QStyledItemDelegate>

class CenteredCheckBox;
class GeoDataRiverPathPointJmkData;

class GeoDataRiverSurveyCrosssectionWindow::VegetationDataTableDelegate : public QStyledItemDelegate
{
	Q_OBJECT

public:
	VegetationDataTableDelegate();
	void setData(GeoDataRiverPathPointJmkData* data);

	void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
	QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
	void setEditorData(QWidget* editor, const QModelIndex& index) const override;
	void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const override;
	void updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index) const override;

private slots:
	void handleCheckboxToggle(bool toggled);

private:
	GeoDataRiverPathPointJmkData* m_jmkData;

	mutable CenteredCheckBox* m_checkBox;
	mutable QAbstractItemModel* m_model;
	mutable QModelIndex m_index;
};

#endif // GEODATARIVERSURVEYCROSSSECTIONWINDOW_VEGETATIONDATATABLEDELEGATE_H
