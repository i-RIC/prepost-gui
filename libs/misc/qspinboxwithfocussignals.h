#ifndef QSPINBOXWITHFOCUSSIGNALS_H
#define QSPINBOXWITHFOCUSSIGNALS_H

#include "misc_global.h"

#include <QSpinBox>

class MISCDLL_EXPORT QSpinBoxWithFocusSignals : public QSpinBox
{
	Q_OBJECT

public:
	QSpinBoxWithFocusSignals(QWidget* parent = nullptr);
	void stepBy(int steps) override;

	void keyPressEvent(QKeyEvent *event) override;

signals:
	void enterPressed();
	void focusInOccured();
	void focusOutOccured();
	void stepChanged(int steps);

protected:
	void focusInEvent(QFocusEvent* event) override;
	void focusOutEvent(QFocusEvent* event) override;
};

#endif // QSPINBOXWITHFOCUSSIGNALS_H
