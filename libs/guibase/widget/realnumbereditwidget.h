#ifndef REALNUMBEREDITWIDGET_H
#define REALNUMBEREDITWIDGET_H

#include "../guibase_global.h"

#include <QLineEdit>

class QEvent;

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
	~RealNumberEditWidget();

	double value() const;
	void setValue(double newvalue);

	double minimum() const;
	bool minimumIsSet() const;
	void setMinimum(double min);
	void clearMinimum();

	double maximum() const;
	bool maximumIsSet() const;
	void setMaximum(double max);
	void clearMaximum();

	void setAcceptEvents(bool accept);
	void setInformChangeOnFocusOutOnly(bool onFocusOutOnly);

signals:
	void valueChanged(double value);

private slots:
	void handleTextEdited();

private:
	bool updateValue(bool inhibitMessage = false);

	void closeEvent(QCloseEvent* e) override;
	void focusOutEvent(QFocusEvent* e) override;

	class Impl;
	Impl* impl;
};

#ifdef _DEBUG
	#include "private/realnumbereditwidget_impl.h"
#endif // _DEBUG

#endif // REALNUMBEREDITWIDGET_H
