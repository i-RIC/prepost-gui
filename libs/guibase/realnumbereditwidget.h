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
	/**
	 * @brief Set check event flag
	 *
	 * Whe the flag is off, even when user input invalid value,
	 * no warning will be shown.
	 */
	void setEventCheck(bool check = true) {
		m_eventCheck = check;
	}
	/// Set the value
	void setValue(double newvalue);
	/// The value
	double value() const {
		return m_doubleValue;
	}

signals:
	void valueChanged(double value);

private slots:
	void handleTextChange();

protected:
	void closeEvent(QCloseEvent* e) override;
	void focusOutEvent(QFocusEvent* e) override;
	bool updateValue();

	double m_doubleValue;
	bool m_eventCheck;
};

#endif // REALNUMBEREDITWIDGET_H
