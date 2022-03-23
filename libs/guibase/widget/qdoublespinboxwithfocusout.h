#ifndef QDOUBLESPINBOXWITHFOCUSOUT_H
#define QDOUBLESPINBOXWITHFOCUSOUT_H

#include "../guibase_global.h"

#include <QDoubleSpinBox>

class GUIBASEDLL_EXPORT QDoubleSpinBoxWithFocusOut : public QDoubleSpinBox
{
	Q_OBJECT

public:
	QDoubleSpinBoxWithFocusOut(QWidget *parent = 0);

protected:
	void focusOutEvent(QFocusEvent* event) override;

signals:
	void focusOut();
};

#endif // QDOUBLESPINBOXWITHFOCUSOUT_H
