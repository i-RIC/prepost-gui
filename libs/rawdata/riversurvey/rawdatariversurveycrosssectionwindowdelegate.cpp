#include "rawdatarivercrosssection.h"
#include "rawdatariversurveycrosssectionwindowdelegate.h"

#include <guibase/centeredcheckbox.h>
#include <guibase/realnumbereditwidget.h>

#include <QPainter>

RawDataRiverSurveyCrosssectionWindowDelegate::RawDataRiverSurveyCrosssectionWindowDelegate()
{
	m_editor = nullptr;
	m_crosssection = nullptr;
	m_checkBox = nullptr;
	m_model = nullptr;
}

void RawDataRiverSurveyCrosssectionWindowDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	if (m_crosssection == nullptr) {return;}
	QVariant dat = index.model()->data(index, Qt::EditRole);
	if (index.column() == 0) {
		CenteredCheckBox checkbox(nullptr);
		checkbox.setChecked(dat.toBool());
		if (m_crosssection->fixedPointLSet() &&
				(index.row() == m_crosssection->fixedPointLIndex() || index.row() == 0)) {
			checkbox.setDisabled(true);
		}
		if (m_crosssection->fixedPointRSet() &&
				(index.row() == m_crosssection->fixedPointRIndex() || index.row() == m_crosssection->numOfAltitudes() - 1)) {
			checkbox.setDisabled(true);
		}
		checkbox.resize(option.rect.size());

		QPixmap pixmap(option.rect.size());
		checkbox.render(&pixmap);
		painter->drawPixmap(option.rect, pixmap);
	} else {
		QStyledItemDelegate::paint(painter, option, index);
	}
}

QWidget* RawDataRiverSurveyCrosssectionWindowDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	if (m_crosssection == nullptr) {return nullptr;}
	QVariant dat = index.model()->data(index, Qt::DisplayRole);
	if (dat.type() == QVariant::Double) {
		// Float. Edit with RealEdit
		RealNumberEditWidget* w = new RealNumberEditWidget(parent);
		m_editor = w;
		connect(m_editor, SIGNAL(destroyed(QObject*)), this, SLOT(handleEditorDestroy(QObject*)));
		return w;
	} else if (dat.type() == QVariant::Bool) {
		// Bool. Edit with Checkbox.
		m_checkBox = new CenteredCheckBox(parent);
		m_model = const_cast<QAbstractItemModel*>(index.model());
		m_index = index;
		connect(m_checkBox, SIGNAL(toggled(bool)), this, SLOT(handleCheckboxToggle(bool)));
		return m_checkBox;
	} else {
		QWidget* w = QStyledItemDelegate::createEditor(parent, option, index);
		m_editor = w;
		connect(m_editor, SIGNAL(destroyed(QObject*)), this, SLOT(handleEditorDestroy(QObject*)));
		return w;
	}
}

void RawDataRiverSurveyCrosssectionWindowDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const
{
	if (m_crosssection == nullptr) {return;}
	QVariant dat = index.model()->data(index, Qt::DisplayRole);
	if (dat.type() == QVariant::Double) {
		// Float. Edit with RealEdit
		RealNumberEditWidget* realEdit = dynamic_cast<RealNumberEditWidget*>(editor);
		realEdit->setValue(dat.toDouble());
	} else if (dat.type() == QVariant::Bool) {
		CenteredCheckBox* checkBox = dynamic_cast<CenteredCheckBox*>(editor);
		checkBox->blockSignals(true);
		checkBox->setChecked(dat.toBool());
		if (m_crosssection->fixedPointLSet() &&
				(index.row() == m_crosssection->fixedPointLIndex() || index.row() == 0)) {
			checkBox->setDisabled(true);
		}
		if (m_crosssection->fixedPointRSet() &&
				(index.row() == m_crosssection->fixedPointRIndex() || index.row() == m_crosssection->numOfAltitudes() - 1)) {
			checkBox->setDisabled(true);
		}
		checkBox->blockSignals(false);
	} else {
		QStyledItemDelegate::setEditorData(editor, index);
	}
}

void RawDataRiverSurveyCrosssectionWindowDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const
{
	QVariant dat = index.model()->data(index, Qt::DisplayRole);
	if (dat.type() == QVariant::Double) {
		// Float. Edit with RealEdit
		RealNumberEditWidget* realEdit = dynamic_cast<RealNumberEditWidget*>(editor);
		model->setData(index, realEdit->value());
	} else if (dat.type() == QVariant::Bool) {
		CenteredCheckBox* checkBox = dynamic_cast<CenteredCheckBox*>(editor);
		model->setData(index, checkBox->isChecked());
	} else {
		QStyledItemDelegate::setModelData(editor, model, index);
	}
}

void RawDataRiverSurveyCrosssectionWindowDelegate::updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& /*index*/) const
{
	editor->setGeometry(option.rect);
}

void RawDataRiverSurveyCrosssectionWindowDelegate::handleCheckboxToggle(bool toggled)
{
	m_model->setData(m_index, toggled);
}

void RawDataRiverSurveyCrosssectionWindowDelegate::handleEditorDestroy(QObject* e)
{
	if (m_editor == e) {
		m_editor = nullptr;
	}
}
