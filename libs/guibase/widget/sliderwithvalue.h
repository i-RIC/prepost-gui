#ifndef SLIDERWITHVALUE_H
#define SLIDERWITHVALUE_H

#include "../guibase_global.h"

#include <QSlider>
#include <QWidget>

class QLabel;
class QSpinBox;

/// Slider with label that displays the value that corresponds to the current selection
class GUIBASEDLL_EXPORT SliderWithValue : public QWidget
{
	Q_OBJECT

public:
	explicit SliderWithValue(QWidget* parent = nullptr);
	~SliderWithValue();
	/// The value

	int value() const;
	Qt::Orientation orientation() const;

	int	tickInterval() const;
	void setTickInterval(int ti);

	QSlider::TickPosition tickPosition() const;
	void setTickPosition(QSlider::TickPosition position);

	int minimum() const;
	void setMinimum(int min);

	int maximum() const;
	void setMaximum(int max);

	void setRange(int min, int max);

	void setValues(const QList<int>& values);
	void setValues(const QList<double>& values);

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
	void updateValues();
	void updateLabel();
	void updateSpinBox();

	QSlider* m_slider;
	QLabel* m_label;
	QSpinBox* m_spinBox;
	QList<double> m_values;
};

#endif // SLIDERWITHVALUE_H
