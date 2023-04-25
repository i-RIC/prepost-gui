#include "sliderwithvalue.h"

#include <misc/qspinboxwithfocussignals.h>

#include <QHBoxLayout>
#include <QLabel>

SliderWithValue::SliderWithValue(QWidget* parent) :
	QWidget(parent)
{
	m_slider = new QSlider(this);
	m_label = new QLabel(this);
	m_label->setFrameShape(QFrame::Panel);
	m_label->setFrameShadow(QFrame::Sunken);
	m_spinBox = new QSpinBoxWithFocusSignals(this);

	connect(m_slider, &QSlider::actionTriggered, this, &SliderWithValue::actionTriggered);
	connect(m_slider, &QSlider::rangeChanged, this, &SliderWithValue::rangeChanged);
	connect(m_slider, &QSlider::sliderMoved, this, &SliderWithValue::handleSliderMove);
	connect(m_slider, &QSlider::sliderPressed, this, &SliderWithValue::sliderPressed);
	connect(m_slider, &QSlider::sliderReleased, this, &SliderWithValue::sliderReleased);
	connect(m_slider, &QSlider::valueChanged, this, &SliderWithValue::handleValueChange);

	connect(m_spinBox, &QSpinBoxWithFocusSignals::focusOutOccured, this, &SliderWithValue::applySpinBoxValue);
	connect(m_spinBox, &QSpinBoxWithFocusSignals::stepChanged, this, &SliderWithValue::applySpinBoxValue);
	connect(m_spinBox, &QSpinBoxWithFocusSignals::enterPressed, this, &SliderWithValue::applySpinBoxValue);

	QHBoxLayout* l = new QHBoxLayout(this);
	l->setMargin(0);
	setLayout(l);
}

SliderWithValue::~SliderWithValue()
{}

void SliderWithValue::setValue(int val)
{
	m_slider->setValue(val);
	updateLabel();
	updateSpinBox();
}

int SliderWithValue::value() const
{
	return m_slider->value();
}

void SliderWithValue::setTickInterval(int ti)
{
	m_slider->setTickInterval(ti);
}

void SliderWithValue::setTickPosition(QSlider::TickPosition position)
{
	m_slider->setTickPosition(position);
}

int	SliderWithValue::tickInterval() const
{
	return m_slider->tickInterval();
}

QSlider::TickPosition SliderWithValue::tickPosition() const
{
	return m_slider->tickPosition();
}

int SliderWithValue::minimum() const
{
	return m_slider->minimum();
}
int SliderWithValue::maximum() const
{
	return m_slider->maximum();
}
void SliderWithValue::setMinimum(int min)
{
	m_slider->blockSignals(true);
	m_slider->setMinimum(min);
	m_slider->blockSignals(false);
	updateValues();
}

void SliderWithValue::setMaximum(int max)
{
	m_slider->blockSignals(true);
	m_slider->setMaximum(max);
	m_slider->blockSignals(false);
	updateValues();
}

void SliderWithValue::setRange(int min, int max)
{
	m_slider->blockSignals(true);
	m_slider->setRange(min, max);
	m_slider->blockSignals(false);
	updateValues();
}

void SliderWithValue::setValues(const QList<int>& values)
{
	m_slider->blockSignals(true);
	m_slider->setMinimum(values[0]);
	m_slider->setMaximum(values[values.count() - 1]);
	for (int val : values) {
		m_values.append(val);
	}
	QLayout* l = layout();
	l->removeWidget(m_slider);
	l->removeWidget(m_label);
	l->removeWidget(m_spinBox);

	m_label->hide();
	m_spinBox->show();

	m_spinBox->setMinimum(m_slider->minimum());
	m_spinBox->setMaximum(m_slider->maximum());

	l->addWidget(m_slider);
	l->addWidget(m_spinBox);
	l->update();

	m_slider->blockSignals(false);
}

void SliderWithValue::setValues(const QList<double>& values)
{
	m_slider->blockSignals(true);
	m_slider->setMinimum(0);
	m_slider->setMaximum(values.count() - 1);
	m_values = values;

	QSize size;
	for (int i = 0; i < values.count(); ++i) {
		QString label("%1");
		m_label->setText(label.arg(values.at(i)));
		QSize sizeHint = m_label->sizeHint();
		if (i == 0 || sizeHint.width() > size.width()) {size.setWidth(sizeHint.width());}
		if (i == 0 || sizeHint.height() > size.height()) {size.setHeight(sizeHint.height());}
	}
	m_label->setFixedWidth(size.width());

	QLayout* l = layout();
	l->removeWidget(m_slider);
	l->removeWidget(m_label);
	l->removeWidget(m_spinBox);

	m_spinBox->hide();
	m_label->show();

	l->addWidget(m_slider);
	l->addWidget(m_label);
	l->update();
	m_slider->blockSignals(false);
}

void SliderWithValue::updateValues()
{
	QList<int> vals;
	for (int i = m_slider->minimum(); i <= m_slider->maximum(); ++i) {
		vals.append(i);
	}
	setValues(vals);
}

void SliderWithValue::updateLabel()
{
	if (m_slider->value() >= m_values.count() || m_slider->value() < 0) {
		m_label->setText("");
		return;
	}

	double val = m_values.at(m_slider->value());
	QString label("%1");
	m_label->setText(label.arg(val));
}

void SliderWithValue::updateSpinBox()
{
	m_spinBox->blockSignals(true);
	m_spinBox->setValue(m_slider->value());
	m_spinBox->blockSignals(false);
}

void SliderWithValue::handleSliderMove(int val)
{
	updateLabel();
	updateSpinBox();
	emit sliderMoved(val);
}

void SliderWithValue::handleValueChange(int val)
{
	updateLabel();
	updateSpinBox();
	emit valueChanged(val);
}

void SliderWithValue::applySpinBoxValue()
{
	setValue(m_spinBox->value());
}

void SliderWithValue::setOrientation(Qt::Orientation o)
{
	m_slider->setOrientation(o);
}

Qt::Orientation SliderWithValue::orientation() const
{
	return m_slider->orientation();
}

void SliderWithValue::setSliderPosition(int pos)
{
	m_slider->setSliderPosition(pos);
}

void SliderWithValue::setInvertedAppearance(bool b)
{
	m_slider->setInvertedAppearance(b);
}
