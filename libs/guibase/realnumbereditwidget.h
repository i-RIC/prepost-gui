#ifndef REALNUMBEREDITWIDGET_H
#define REALNUMBEREDITWIDGET_H

#include "guibase_global.h"

#include <QLineEdit>

class GUIBASEDLL_EXPORT RealNumberEditWidget : public QLineEdit
{
	Q_OBJECT

public:
	RealNumberEditWidget(QWidget* parent = nullptr);
	void setEventCheck(bool check = true) {
		m_eventCheck = check;
	}
	void setValue(double newvalue);
	double value() {
		return m_doubleValue;
	}

signals:
	void valueChanged(double value);

private slots:
	void handleTextChange();

protected:
	void closeEvent(QCloseEvent* e);
	void focusOutEvent(QFocusEvent* e);
	bool updateValue();
	double m_doubleValue;
	bool m_eventCheck;
};

#endif // REALNUMBEREDITWIDGET_H
