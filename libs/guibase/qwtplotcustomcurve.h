#ifndef QWTPLOTCUSTOMCURVE_H
#define QWTPLOTCUSTOMCURVE_H

#include "guibase_global.h"

#include <qwt_plot_curve.h>

class GUIBASEDLL_EXPORT QwtPlotCustomCurve : public QwtPlotCurve
{
public:
	explicit QwtPlotCustomCurve();
	QwtPlotCustomCurve(const QString& title);

private:
	void initLegendAttribute();
};

#endif // QWTPLOTCUSTOMCURVE_H
