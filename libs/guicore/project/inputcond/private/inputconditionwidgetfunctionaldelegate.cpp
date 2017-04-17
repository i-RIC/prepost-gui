#include "inputconditionwidgetfunctionaldelegate.h"
#include "inputconditionwidgetfunctionaldialog.h"

#include <guibase/widget/realnumbereditwidget.h>

#include <QModelIndex>
#include <QSpinBox>

namespace {

template <typename T, typename V>
void setupMinMax(T* widget, bool minIsSet, double min, bool maxIsSet, double max)
{
	if (minIsSet) {
		widget->setMinimum(static_cast<V> (min));
	}
	if (maxIsSet) {
		widget->setMaximum(static_cast<V> (max));
	}
}

} // namespace


void InputConditionWidgetFunctionalDelegate::setDialog(InputConditionWidgetFunctionalDialog* dialog)
{
	m_dialog = dialog;
}

QWidget* InputConditionWidgetFunctionalDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& /*option*/, const QModelIndex& index) const
{
	auto dlg = m_dialog;

	QVariant dat = index.model()->data(index, Qt::DisplayRole);
	if (dat.type() == QVariant::Int) {
		// Integer. Edit with QSpinBox
		QSpinBox* spinBox = new QSpinBox(parent);

		if (index.column() == 0) {
			setupMinMax<QSpinBox, int>(spinBox, dlg->m_paramMinIsSet, dlg->m_paramMin, dlg->m_paramMaxIsSet, dlg->m_paramMax);
		} else {
			int vId = index.column() - 1;
			setupMinMax<QSpinBox, int>(spinBox, dlg->m_valueMinIsSet[vId], dlg->m_valueMin[vId], dlg->m_valueMaxIsSet[vId], dlg->m_valueMax[vId]);
		}

		return spinBox;
	} else {
		// Float. Edit with RealEdit
		RealNumberEditWidget* realEdit = new RealNumberEditWidget(parent);

		if (index.column() == 0) {
			setupMinMax<RealNumberEditWidget, double>(realEdit, dlg->m_paramMinIsSet, dlg->m_paramMin, dlg->m_paramMaxIsSet, dlg->m_paramMax);
		} else {
			int vId = index.column() - 1;
			setupMinMax<RealNumberEditWidget, double>(realEdit, dlg->m_valueMinIsSet[vId], dlg->m_valueMin[vId], dlg->m_valueMaxIsSet[vId], dlg->m_valueMax[vId]);
		}

		return realEdit;
	}
}

void InputConditionWidgetFunctionalDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const
{
	QVariant dat = index.model()->data(index, Qt::DisplayRole);
	if (dat.type() == QVariant::Int) {
		// Integer. Edit with QSpinBox
		QSpinBox* spinBox = static_cast<QSpinBox*>(editor);
		spinBox->setValue(dat.toInt());
	} else {
		// Float. Edit with RealEdit
		RealNumberEditWidget* realEdit = static_cast<RealNumberEditWidget*>(editor);
		realEdit->setValue(dat.toDouble());
	}
}
void InputConditionWidgetFunctionalDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const
{
	QVariant dat = index.model()->data(index, Qt::DisplayRole);
	if (dat.type() == QVariant::Int) {
		// Integer. editor = QSpinBox
		QSpinBox* spinBox = static_cast<QSpinBox*>(editor);
		model->setData(index, spinBox->value(), Qt::EditRole);
	} else {
		// Float. Edit with RealEdit
		RealNumberEditWidget* realEdit = static_cast<RealNumberEditWidget*>(editor);
		model->setData(index, realEdit->value(), Qt::EditRole);
	}
}

void InputConditionWidgetFunctionalDelegate::updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& /*index*/) const
{
	editor->setGeometry(option.rect);
}
