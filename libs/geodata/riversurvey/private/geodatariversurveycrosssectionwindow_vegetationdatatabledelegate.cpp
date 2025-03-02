#include "geodatariversurveycrosssectionwindow_vegetationdatatabledelegate.h"

#include <guibase/widget/centeredcheckbox.h>
#include <guibase/widget/realnumbereditwidget.h>

#include <QComboBox>

namespace {

void drawComboBox(const QString& caption, const QRect& rect, QPainter* painter)
{
	QComboBox comboBox;
	comboBox.addItem(caption);
	comboBox.resize(rect.size());

	QPixmap pixmap(rect.size());
	comboBox.render(&pixmap);
	painter->drawPixmap(rect, pixmap);
}

}

GeoDataRiverSurveyCrosssectionWindow::VegetationDataTableDelegate::VegetationDataTableDelegate() :
	m_jmkData {nullptr},
	m_checkBox {nullptr},
	m_model {nullptr}
{}

void GeoDataRiverSurveyCrosssectionWindow::VegetationDataTableDelegate::setData(GeoDataRiverPathPointJmkData* data)
{
	m_jmkData = data;
}

void GeoDataRiverSurveyCrosssectionWindow::VegetationDataTableDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	if (m_jmkData == nullptr) {return;}
	QVariant dat = index.model()->data(index, Qt::DisplayRole);
	auto col = index.column();
	if (col == 0 || col == 1 || col == 2 || col == 7) {
		QStyledItemDelegate::paint(painter, option, index);
	} else if (col == 3) {
		CenteredCheckBox checkbox(nullptr);
		checkbox.resize(option.rect.size());
		if (dat.toInt() == 1) {
			checkbox.setChecked(true);
		}
		QPixmap pixmap(option.rect.size());
		checkbox.render(&pixmap);
		painter->drawPixmap(option.rect, pixmap);
	} else if (col == 4) {
		if (dat.toInt() == 1) {
			drawComboBox(tr("Dense"), option.rect, painter);
		} else {
			drawComboBox(tr("Sparse"), option.rect, painter);
		}
	} else if (col == 5) {
		if (dat.toInt() == 1) {
			drawComboBox(tr("Real trees"), option.rect, painter);
		} else {
			drawComboBox(tr("Upstream trees"), option.rect, painter);
		}
	} else if (col == 6) {
		if (dat.toInt() == 1) {
			drawComboBox(tr("High trees"), option.rect, painter);
		} else {
			drawComboBox(tr("Low trees"), option.rect, painter);
		}
	}
}

QWidget* GeoDataRiverSurveyCrosssectionWindow::VegetationDataTableDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	if (m_jmkData == nullptr) {return nullptr;}

	QVariant dat = index.model()->data(index, Qt::DisplayRole);
	auto col = index.column();
	if (col == 0 || col == 1 || col == 2 || col == 7) {
		auto w = new RealNumberEditWidget(parent);
		return w;
	} else if (col == 3) {
		m_checkBox = new CenteredCheckBox(parent);
		m_model = const_cast<QAbstractItemModel*>(index.model());
		m_index = index;

		connect(m_checkBox, &CenteredCheckBox::toggled, this, &VegetationDataTableDelegate::handleCheckboxToggle);
		return m_checkBox;
	} else if (col == 4) {
		auto w = new QComboBox(parent);
		w->addItem(tr("Sparse"));
		w->addItem(tr("Dense"));
		return w;
	} else if (col == 5) {
		auto w = new QComboBox(parent);
		w->addItem(tr("Upstream trees"));
		w->addItem(tr("Real trees"));
		return w;
	} else if (col == 6) {
		auto w = new QComboBox(parent);
		w->addItem(tr("Low trees"));
		w->addItem(tr("High trees"));
		return w;
	}
}

void GeoDataRiverSurveyCrosssectionWindow::VegetationDataTableDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const
{
	if (m_jmkData == nullptr) {return;}

	QVariant dat = index.model()->data(index, Qt::DisplayRole);
	auto col = index.column();
	if (col == 0 || col == 1 || col == 2 || col == 7) {
		auto w = dynamic_cast<RealNumberEditWidget*> (editor);
		w->setValue(dat.toDouble());
	} else if (col == 3) {
		auto checkBox = dynamic_cast<CenteredCheckBox*>(editor);
		checkBox->blockSignals(true);
		checkBox->setChecked(dat.toInt() == 1);
		checkBox->blockSignals(false);
	} else if (col == 4) {
		auto comboBox = dynamic_cast<QComboBox*> (editor);
		comboBox->setCurrentIndex(dat.toInt());
	} else if (col == 5) {
		auto comboBox = dynamic_cast<QComboBox*> (editor);
		comboBox->setCurrentIndex(dat.toInt());
	} else if (col == 6) {
		auto comboBox = dynamic_cast<QComboBox*> (editor);
		comboBox->setCurrentIndex(dat.toInt());
	}
}

void GeoDataRiverSurveyCrosssectionWindow::VegetationDataTableDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const
{
	auto col = index.column();
	if (col == 0 || col == 1 || col == 2 || col == 7) {
		auto w = dynamic_cast<RealNumberEditWidget*> (editor);
		model->setData(index, w->value());
	} else if (col == 3) {
		int v = 0;
		auto checkBox = dynamic_cast<CenteredCheckBox*>(editor);
		if (checkBox->isChecked()) {
			v = 1;
		}
		model->setData(index, 1);
	} else if (col == 4) {
		auto comboBox = dynamic_cast<QComboBox*> (editor);
		model->setData(index, comboBox->currentIndex());
	} else if (col == 5) {
		auto comboBox = dynamic_cast<QComboBox*> (editor);
		model->setData(index, comboBox->currentIndex());
	} else if (col == 6) {
		auto comboBox = dynamic_cast<QComboBox*> (editor);
		model->setData(index, comboBox->currentIndex());
	}
}

void GeoDataRiverSurveyCrosssectionWindow::VegetationDataTableDelegate::updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	editor->setGeometry(option.rect);
}

void GeoDataRiverSurveyCrosssectionWindow::VegetationDataTableDelegate::handleCheckboxToggle(bool toggled)
{
	m_model->setData(m_index, toggled);
}
