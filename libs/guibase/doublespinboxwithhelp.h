#ifndef DOUBLESPINBOXWITHHELP_H
#define DOUBLESPINBOXWITHHELP_H

#include "guibase_global.h"

#include <QWidget>

namespace Ui
{
	class DoubleSpinBoxWithHelp;
}

class GUIBASEDLL_EXPORT DoubleSpinBoxWithHelp : public QWidget
{
	Q_OBJECT

public:
	explicit DoubleSpinBoxWithHelp(QWidget* parent = nullptr);
	~DoubleSpinBoxWithHelp();

	void setMinimum(double min);
	void setMaximum(double max);
	void setValue(double value);
	double value() const;
	void setAlignment(Qt::Alignment a);
	void setDecimals(int prec);

private:
	void updateHelp();

private:
	Ui::DoubleSpinBoxWithHelp* ui;
};

#endif // DOUBLESPINBOXWITHHELP_H
