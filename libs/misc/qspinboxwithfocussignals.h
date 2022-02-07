#ifndef QSPINBOXWITHFOCUSSIGNALS_H
#define QSPINBOXWITHFOCUSSIGNALS_H

#include "misc_global.h"

#include <QSpinBox>

class MISCDLL_EXPORT QSpinBoxWithFocusSignals : public QSpinBox
{
	Q_OBJECT

public:
	QSpinBoxWithFocusSignals(QWidget* parent = nullptr);

signals:
	void focusInOccured();
	void focusOutOccured();

protected:
	void focusInEvent(QFocusEvent* event) override;
	void focusOutEvent(QFocusEvent* event) override;
};

#endif // QSPINBOXWITHFOCUSSIGNALS_H
