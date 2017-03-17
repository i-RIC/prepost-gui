#include "coordinateeditwidget.h"
#include <cmath>
#include <QDoubleValidator>
#include <QMessageBox>

CoordinateEditWidget::CoordinateEditWidget(QWidget* parent) :
	QLineEdit(parent)
{
	m_eventCheck = true;
	setText("0");
	setValidator(new QDoubleValidator(this));
	connect(this, SIGNAL(textChanged(QString)), this, SLOT(handleTextChange()));
}

void CoordinateEditWidget::setEventCheck(bool check)
{
	m_eventCheck = check;
}

void CoordinateEditWidget::setValue(double newvalue)
{
	if (m_doubleValue == newvalue){return;}
	m_doubleValue = newvalue;
	QString val1;
	val1.setNum(newvalue, 'f', 6);
	setText(val1);
}

double CoordinateEditWidget::value() const
{
	return m_doubleValue;
}

void CoordinateEditWidget::closeEvent(QCloseEvent* e)
{
	if (updateValue()){
		QLineEdit::closeEvent(e);
	}
}

void CoordinateEditWidget::focusOutEvent(QFocusEvent* e)
{
	if (updateValue()){
		QLineEdit::focusOutEvent(e);
	}else{
		if (m_eventCheck){
			QMessageBox::warning(this, tr("Error"), tr("It is not a real value"));
			setFocus(Qt::OtherFocusReason);
		}
	}
}

bool CoordinateEditWidget::updateValue()
{
	QString txt = text();
	if (txt.isEmpty()){
		m_doubleValue = 0;
		emit valueChanged(m_doubleValue);
		return true;
	}
	bool ok;
	double tmpval = txt.toDouble(&ok);
	if (! ok){
		// Invalid!
		return false;
	}
	m_doubleValue = tmpval;
	emit valueChanged(m_doubleValue);
	return true;
}

void CoordinateEditWidget::handleTextChange()
{
	updateValue();
}
