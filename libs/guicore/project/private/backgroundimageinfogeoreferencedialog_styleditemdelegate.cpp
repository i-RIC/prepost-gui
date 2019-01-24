#include "backgroundimageinfogeoreferencedialog_styleditemdelegate.h"

#include <QDoubleSpinBox>
#include <QPainter>

namespace {
const static int DEFAULT_DECIMALS = 5;
}
BackgroundImageInfoGeoreferenceDialog::StyledItemDelegate::StyledItemDelegate(QObject* parent) :
	QStyledItemDelegate(parent),
	m_decimals {DEFAULT_DECIMALS}
{
}

QString BackgroundImageInfoGeoreferenceDialog::StyledItemDelegate::displayText(const QVariant& value, const QLocale& locale) const
{
	return locale.toString(value.toDouble(), 'f', m_decimals);
}

void BackgroundImageInfoGeoreferenceDialog::StyledItemDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const
{
  QDoubleSpinBox* e = qobject_cast<QDoubleSpinBox*>(editor);
	if (e != nullptr) {
		e->setDecimals(m_decimals);
	}

	QStyledItemDelegate::setEditorData(editor, index);
}
