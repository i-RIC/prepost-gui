#ifndef COORDINATEEDITWIDGET_H
#define COORDINATEEDITWIDGET_H

#include "../guibase_global.h"

#include <QLineEdit>

class GUIBASEDLL_EXPORT CoordinateEditWidget : public QLineEdit
{
	Q_OBJECT

public:
	CoordinateEditWidget(QWidget* parent = nullptr);

	double value() const;
	void setValue(double newvalue);

	void setEventCheck(bool check = true);

signals:
	void valueChanged(double value);

private slots:
	void handleTextChange();

private:
	void closeEvent(QCloseEvent* e);
	void focusOutEvent(QFocusEvent* e);
	bool updateValue();

	double m_doubleValue;
	bool m_eventCheck;
};

#endif // COORDINATEEDITWIDGET_H
