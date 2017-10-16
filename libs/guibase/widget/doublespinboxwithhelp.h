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

	double minimum() const;
	void setMinimum(double min);

	double maximum() const;
	void setMaximum(double max);

	double value() const;
	void setValue(double value);

	void setAlignment(Qt::Alignment a);
	void setDecimals(int prec);

private:
	void updateHelp();

private:
	Ui::DoubleSpinBoxWithHelp* ui;
};

#endif // DOUBLESPINBOXWITHHELP_H
