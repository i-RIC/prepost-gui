#include "realnumbereditwidget.h"
#include "private/realnumbereditwidget_impl.h"

#include <QDoubleValidator>
#include <QKeyEvent>
#include <QMessageBox>

#include <cmath>

RealNumberEditWidget::Impl::Impl() :
	m_value {0},
	m_maximumIsSet {false},
	m_maximum {0},
	m_minimumIsSet {false},
	m_minimum {0}
{}

// public interfaces

RealNumberEditWidget::RealNumberEditWidget(QWidget* parent) :
	QLineEdit(parent),
	impl {new Impl {}}
{
	setValidator(new QDoubleValidator(this));
	connect(this, SIGNAL(textEdited(QString)), this, SLOT(handleTextEdited()));
}

RealNumberEditWidget::~RealNumberEditWidget()
{
	delete impl;
}

double RealNumberEditWidget::value() const
{
	return impl->m_value;
}

void RealNumberEditWidget::setValue(double newvalue)
{
	if (! text().isEmpty() && (impl->m_value == newvalue)) {return;}

	impl->m_value = newvalue;
	if (impl->m_value == 0) {
		setText("0");
	} else {
		setText(QString::number(impl->m_value, 'g', 10));
	}
}

double RealNumberEditWidget::minimum() const
{
	return impl->m_minimum;
}

bool RealNumberEditWidget::minimumIsSet() const
{
	return impl->m_minimumIsSet;
}

void RealNumberEditWidget::setMinimum(double min)
{
	impl->m_minimum = min;
	impl->m_minimumIsSet = true;
}

void RealNumberEditWidget::clearMinimum()
{
	impl->m_minimum = 0;
	impl->m_minimumIsSet = false;
}

double RealNumberEditWidget::maximum() const
{
	return impl->m_maximum;
}

bool RealNumberEditWidget::maximumIsSet() const
{
	return impl->m_maximumIsSet;
}

void RealNumberEditWidget::setMaximum(double max)
{
	impl->m_maximum = max;
	impl->m_maximumIsSet = true;
}

void RealNumberEditWidget::clearMaximum()
{
	impl->m_maximum = 0;
	impl->m_maximumIsSet = false;
}

void RealNumberEditWidget::closeEvent(QCloseEvent* e)
{
	if (updateValue(true)) {
		QLineEdit::closeEvent(e);
	} else {
		e->ignore();
	}
}

void RealNumberEditWidget::focusOutEvent(QFocusEvent* e)
{
	if (updateValue()) {
		QLineEdit::focusOutEvent(e);
	} else {
		e->ignore();
		setFocus();
	}
}

void RealNumberEditWidget::handleTextEdited()
{
	updateValue(true);
}

bool RealNumberEditWidget::updateValue(bool inhibitMessage)
{
	QString txt = text();
	double tmpVal = 0;
	if (txt.isEmpty()) {
		tmpVal = 0;
	} else {
		bool ok;
		tmpVal = txt.toDouble(&ok);
		if (! ok) {
			if (! inhibitMessage) {
				QMessageBox::warning(this, tr("Warning"), tr("It is not a real value"));
			}
			return false;
		}
	}
	if (impl->m_minimumIsSet && tmpVal < impl->m_minimum) {
		if (! inhibitMessage) {
			QMessageBox::warning(this, tr("Warning"), tr("Minimum value is %1.").arg(impl->m_minimum));
		}
		return false;
	}
	if (impl->m_maximumIsSet && tmpVal > impl->m_maximum) {
		if (! inhibitMessage) {
			QMessageBox::warning(this, tr("Warning"), tr("Maximum value is %1.").arg(impl->m_maximum));
		}
		return false;
	}

	impl->m_value = tmpVal;
	emit valueChanged(impl->m_value);
	return true;
}
