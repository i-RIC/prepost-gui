#ifndef DOUBLESPINBOXWITHHELP_H
#define DOUBLESPINBOXWITHHELP_H

#include "../guibase_global.h"

#include <QWidget>

namespace Ui
{
	class DoubleSpinBoxWithHelp;
}

/**
 * @brief DoubleSpinBox with help about the minimum and maximum value
 */
class GUIBASEDLL_EXPORT DoubleSpinBoxWithHelp : public QWidget
{
	Q_OBJECT

public:
	explicit DoubleSpinBoxWithHelp(QWidget* parent = nullptr);
	~DoubleSpinBoxWithHelp();
	/// The minimum value
	double minimum() const;
	/// Set the minimum value
	void setMinimum(double min);
	/// The maximum value
	double maximum() const;
	/// Set the maximum value
	void setMaximum(double max);
	/// The value
	double value() const;
	/// Set the value
	void setValue(double value);
	/// Set the alighnment of the content
	void setAlignment(Qt::Alignment a);
	/// Set the decimal precision
	void setDecimals(int prec);

private:
	/// Update the help string shown as tooltop
	void updateHelp();

private:
	Ui::DoubleSpinBoxWithHelp* ui;
};

#endif // DOUBLESPINBOXWITHHELP_H
