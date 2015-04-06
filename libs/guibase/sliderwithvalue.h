#ifndef SLIDERWITHVALUE_H
#define SLIDERWITHVALUE_H

#include "guibase_global.h"

#include <QWidget>
#include <QSlider>

class QLabel;
class QSpinBox;

class GUIBASEDLL_EXPORT SliderWithValue : public QWidget
{
	Q_OBJECT

public:
	explicit SliderWithValue(QWidget *parent = 0);
	~SliderWithValue();
	int value();
	void setTickInterval(int ti);
	void setTickPosition(QSlider::TickPosition position);
	int	tickInterval() const;
	QSlider::TickPosition tickPosition() const;
	int minimum() const;
	int maximum() const;
	void setMinimum(int min);
	void setMaximum(int max);
	void setRange(int min, int max);

	void setValues(const QList<int>& values);
	void setValues(const QList<double>& values);
	Qt::Orientation orientation() const;
	void setSliderPosition(int pos);
	void setInvertedAppearance(bool b);

public slots:
	void setValue(int val);
	void setOrientation(Qt::Orientation o);

private slots:
	void handleSliderMove(int val);
	void handleValueChange(int val);

signals:
	void actionTriggered(int action);
	void rangeChanged(int min, int max);
	void sliderMoved(int value);
	void sliderPressed();
	void sliderReleased();
	void valueChanged(int value);

private:
	QSlider* m_slider;
	QLabel* m_label;
	QSpinBox* m_spinBox;
	void updateValues();
	void updateLabel();
	void updateSpinBox();
	QList<double> m_values;
};

#endif // SLIDERWITHVALUE_H
