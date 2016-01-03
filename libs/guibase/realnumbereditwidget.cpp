#include "realnumbereditwidget.h"

#include <QDoubleValidator>
#include <QMessageBox>

#include <cmath>

RealNumberEditWidget::RealNumberEditWidget(QWidget* parent) :
	QLineEdit(parent)
{
	setValidator(new QDoubleValidator(this));
	connect(this, SIGNAL(textChanged(QString)), this, SLOT(handleTextChange()));
}

double RealNumberEditWidget::value() const
{
	return m_doubleValue;
}

void RealNumberEditWidget::setValue(double newvalue)
{
	if (m_doubleValue == newvalue) {return;}

	m_doubleValue = newvalue;
	QString txt("%1");
	setText(txt.arg(m_doubleValue));
}

void RealNumberEditWidget::closeEvent(QCloseEvent* e)
{
	if (updateValue()) {
		QLineEdit::closeEvent(e);
	}
}

void RealNumberEditWidget::focusOutEvent(QFocusEvent* e)
{
	if (updateValue()) {
		QLineEdit::focusOutEvent(e);
	} else {
		QMessageBox::warning(this, tr("Error"), tr("It is not a real value"));
		setFocus(Qt::OtherFocusReason);
	}
}

bool RealNumberEditWidget::updateValue()
{
	QString txt = text();
	if (txt.isEmpty()) {
		m_doubleValue = 0;
		emit valueChanged(m_doubleValue);
		return true;
	}

	bool ok;
	double tmpval = txt.toDouble(&ok);
	if (! ok) {
		return false; // invalid value
	}

	m_doubleValue = tmpval;
	emit valueChanged(m_doubleValue);
	return true;
}

void RealNumberEditWidget::handleTextChange()
{
	updateValue();
}
