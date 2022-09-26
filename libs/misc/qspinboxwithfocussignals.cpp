#include "qspinboxwithfocussignals.h"

QSpinBoxWithFocusSignals::QSpinBoxWithFocusSignals(QWidget* parent) :
	QSpinBox(parent)
{}

void QSpinBoxWithFocusSignals::stepBy(int steps)
{
	emit stepChanged(steps);
	QSpinBox::stepBy(steps);
}

void QSpinBoxWithFocusSignals::focusInEvent(QFocusEvent* event)
{
	QSpinBox::focusInEvent(event);

	emit focusInOccured();
}

void QSpinBoxWithFocusSignals::focusOutEvent(QFocusEvent* event)
{
	QSpinBox::focusOutEvent(event);

	emit focusOutOccured();
}
