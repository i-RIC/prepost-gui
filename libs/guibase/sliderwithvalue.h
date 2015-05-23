#ifndef SLIDERWITHVALUE_H
#define SLIDERWITHVALUE_H

#include "guibase_global.h"

#include <QWidget>
#include <QSlider>

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
	int value();
	/// Tick interval to the slider
	int	tickInterval() const;
	/// Set tick interval to the slider
	void setTickInterval(int ti);
	/// Tick position of the slider
	QSlider::TickPosition tickPosition() const;
	/// Set tick position of the slider
	void setTickPosition(QSlider::TickPosition position);
	/// Minimum value of the slider
	int minimum() const;
	/// Set the minimum value of the slider
	void setMinimum(int min);
	/// Maximum value of the slider
	int maximum() const;
	/// Set the maximum value of the slider
	void setMaximum(int max);
	/// Set the minimum and maximum value of the slider
	void setRange(int min, int max);

	/// Set the values that corresponds to the slider selection
	void setValues(const QList<int>& values);
	/// Set the values that corresponds to the slider selection
	void setValues(const QList<double>& values);
	/// The orientation (horizontal or vertical) of the slider
	Qt::Orientation orientation() const;
	/// Set the position of the slider
	void setSliderPosition(int pos);
	/// Set slider invertedAppearance flag
	void setInvertedAppearance(bool b);

public slots:
	/// Set the slider value
	void setValue(int val);
	/// Set the orientation (horizontal or vertical) of the slider
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
