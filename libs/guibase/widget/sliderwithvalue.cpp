#include "sliderwithvalue.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QSpinBox>

SliderWithValue::SliderWithValue(QWidget* parent) :
	QWidget(parent)
{
	m_slider = new QSlider(this);
	m_label = new QLabel(this);
	m_label->setFrameShape(QFrame::Panel);
	m_label->setFrameShadow(QFrame::Sunken);
	m_spinBox = new QSpinBox(this);

	connect(m_slider, SIGNAL(actionTriggered(int)), this, SIGNAL(actionTriggered(int)));
	connect(m_slider, SIGNAL(rangeChanged(int, int)), this, SIGNAL(rangeChanged(int, int)));
	connect(m_slider, SIGNAL(sliderMoved(int)), this, SLOT(handleSliderMove(int)));
	connect(m_slider, SIGNAL(sliderPressed()), this, SIGNAL(sliderPressed()));
	connect(m_slider, SIGNAL(sliderReleased()), this, SIGNAL(sliderReleased()));
	connect(m_slider, SIGNAL(valueChanged(int)), this, SLOT(handleValueChange(int)));

	connect(m_spinBox, SIGNAL(valueChanged(int)), this, SLOT(setValue(int)));

	QHBoxLayout* l = new QHBoxLayout(this);
	l->setMargin(0);
	setLayout(l);
}

SliderWithValue::~SliderWithValue()
{

}

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
