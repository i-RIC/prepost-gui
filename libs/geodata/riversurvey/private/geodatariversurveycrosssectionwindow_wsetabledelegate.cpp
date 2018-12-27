#include "geodatariversurveycrosssectionwindow_wsetabledelegate.h"
#include "../geodatariversurvey.h"

#include <guibase/widget/centeredcheckbox.h>
#include <guibase/widget/realnumbereditwidget.h>

#include <QLineEdit>
#include <QModelIndex>
#include <QPainter>

GeoDataRiverSurveyCrosssectionWindow::WseTableDelegate::WseTableDelegate(QObject* parent) :
	QItemDelegate(parent),
	m_point {nullptr}
{}

void GeoDataRiverSurveyCrosssectionWindow::WseTableDelegate::setPoint(GeoDataRiverPathPoint* point)
{
	m_point = point;
}

QWidget* GeoDataRiverSurveyCrosssectionWindow::WseTableDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	if (index.column() == 1) {
		QLineEdit* edit = new QLineEdit(parent);
		edit->setMaxLength(GeoDataRiverSurvey::WSE_NAME_MAXLENGTH);
		return edit;
	} else {
		return new RealNumberEditWidget(parent);
	}
}

void GeoDataRiverSurveyCrosssectionWindow::WseTableDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const
{
	if (index.column() == 1) {
		QVariant dat = index.model()->data(index, Qt::DisplayRole);
		auto edit = dynamic_cast<QLineEdit*>(editor);
		edit->setText(dat.toString());
	} else {
		QVariant dat = index.model()->data(index, Qt::DisplayRole);
		auto realEdit = dynamic_cast<RealNumberEditWidget*>(editor);
		realEdit->setValue(dat.toDouble());
	}
}

void GeoDataRiverSurveyCrosssectionWindow::WseTableDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const
{
	if (index.column() == 1) {
		QLineEdit* edit = dynamic_cast<QLineEdit*>(editor);
		model->setData(index, edit->text());
	} else {
		RealNumberEditWidget* realEdit = dynamic_cast<RealNumberEditWidget*>(editor);
		model->setData(index, realEdit->value());
	}
}

void GeoDataRiverSurveyCrosssectionWindow::WseTableDelegate::updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	editor->setGeometry(option.rect);
}

void GeoDataRiverSurveyCrosssectionWindow::WseTableDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	if (index.column() == 0) {
		QVariant dat = index.model()->data(index, Qt::DisplayRole);
		CenteredCheckBox checkbox(nullptr);
		checkbox.setChecked(dat.toBool());
		checkbox.resize(option.rect.size());
		QPixmap pixmap(option.rect.size());
		checkbox.render(&pixmap);
		painter->drawPixmap(option.rect, pixmap);
	} else if (index.column() == 3) {
		QVariant dat = index.model()->data(index, Qt::DisplayRole);
		QColor col = dat.value<QColor>();
		QBrush brush(col);
		painter->fillRect(option.rect, brush);
	} else {
		QItemDelegate::paint(painter, option, index);
	}
}
