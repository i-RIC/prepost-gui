#include "realnumbereditwidget.h"
#include <cmath>
#include <QDoubleValidator>
#include <QMessageBox>

RealNumberEditWidget::RealNumberEditWidget(QWidget* parent)
	: QLineEdit(parent){
	m_eventCheck = true;
	setValidator(new QDoubleValidator(this));
	connect(this, SIGNAL(textChanged(QString)), this, SLOT(handleTextChange()));
}

void RealNumberEditWidget::setValue(double newvalue)
{
	if (m_doubleValue == newvalue){return;}
	m_doubleValue = newvalue;
	QString txt("%1");
	setText(txt.arg(m_doubleValue));
}

void RealNumberEditWidget::closeEvent(QCloseEvent* e)
{
	if (updateValue()){
		QLineEdit::closeEvent(e);
	}
}

void RealNumberEditWidget::focusOutEvent(QFocusEvent* e)
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

bool RealNumberEditWidget::updateValue()
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

void RealNumberEditWidget::handleTextChange()
{
	updateValue();
}
