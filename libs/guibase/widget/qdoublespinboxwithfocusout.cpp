#include "qdoublespinboxwithfocusout.h"

QDoubleSpinBoxWithFocusOut::QDoubleSpinBoxWithFocusOut(QWidget *parent):
	QDoubleSpinBox {parent}
{}

void QDoubleSpinBoxWithFocusOut::focusOutEvent(QFocusEvent* event)
{
	emit focusOut();
	QDoubleSpinBox::focusOutEvent(event);
}
