#ifndef REALNUMBEREDITWIDGET_H
#define REALNUMBEREDITWIDGET_H

#include "guibase_global.h"

#include <QLineEdit>

/**
 * @brief Widget to edit real number
 *
 * It looks like just an line edit, but when user tries to
 * input string that can not be parses as real number, warning message
 * will be shown.
 */
class GUIBASEDLL_EXPORT RealNumberEditWidget : public QLineEdit
{
	Q_OBJECT

public:
	RealNumberEditWidget(QWidget* parent = nullptr);

	double value() const;
	void setValue(double newvalue);

signals:
	void valueChanged(double value);

private slots:
	void handleTextChange();

private:
	void closeEvent(QCloseEvent* e) override;
	void focusOutEvent(QFocusEvent* e) override;
	bool updateValue();

	double m_doubleValue;
};

#endif // REALNUMBEREDITWIDGET_H
