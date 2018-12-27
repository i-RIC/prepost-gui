#ifndef GEODATARIVERSURVEYCROSSSECTIONWINDOW_DATATABLEDELEGATE_H
#define GEODATARIVERSURVEYCROSSSECTIONWINDOW_DATATABLEDELEGATE_H

#include "../geodatariversurveycrosssectionwindow.h"

#include <QStyledItemDelegate>

class CenteredCheckBox;

class GeoDataRiverSurveyCrosssectionWindow::DataTableDelegate : public QStyledItemDelegate
{
	Q_OBJECT

public:
	DataTableDelegate();
	void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
	void setCrosssection(GeoDataRiverCrosssection* c) {m_crosssection = c;}
	QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
	void setEditorData(QWidget* editor, const QModelIndex& index) const override;
	void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const override;
	void updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index) const override;

private slots:
	void handleCheckboxToggle(bool toggled);

private:
	GeoDataRiverCrosssection* m_crosssection;
	mutable CenteredCheckBox* m_checkBox;
	mutable QAbstractItemModel* m_model;
	mutable QModelIndex m_index;
};

#endif // GEODATARIVERSURVEYCROSSSECTIONWINDOW_DATATABLEDELEGATE_H
