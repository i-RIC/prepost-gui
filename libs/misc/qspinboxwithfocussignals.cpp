#include "qspinboxwithfocussignals.h"

#include <misc/keyboardsupport.h>

QSpinBoxWithFocusSignals::QSpinBoxWithFocusSignals(QWidget* parent) :
	QSpinBox(parent)
{}

void QSpinBoxWithFocusSignals::stepBy(int steps)
{
	QSpinBox::stepBy(steps);
	emit stepChanged(steps);
}

void QSpinBoxWithFocusSignals::keyPressEvent(QKeyEvent *event)
{
	QSpinBox::keyPressEvent(event);

	if (iRIC::isEnterKey(event->key())) {
		emit enterPressed();
	}
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
