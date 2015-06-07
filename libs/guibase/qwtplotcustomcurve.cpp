#include "qwtplotcustomcurve.h"

QwtPlotCustomCurve::QwtPlotCustomCurve()
	: QwtPlotCurve()
{
	initLegendAttribute();
}

QwtPlotCustomCurve::QwtPlotCustomCurve(const QString& title)
	: QwtPlotCurve(title)
{
	initLegendAttribute();
}

void QwtPlotCustomCurve::initLegendAttribute()
{
	QwtPlotCurve::LegendAttribute atts = static_cast<QwtPlotCurve::LegendAttribute>(QwtPlotCurve::LegendShowLine + QwtPlotCurve::LegendShowSymbol);
	setLegendAttribute(atts);
}
