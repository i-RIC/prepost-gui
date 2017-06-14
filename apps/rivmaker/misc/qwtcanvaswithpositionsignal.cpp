#include "qwtcanvaswithpositionsignal.h"

#include <qwt_plot.h>

#include <QMouseEvent>

QwtCanvasWithPositionSignal::QwtCanvasWithPositionSignal(QwtPlot *plot) :
	QwtPlotCanvas(plot)
{
	setMouseTracking(true);
}

void QwtCanvasWithPositionSignal::mouseMoveEvent(QMouseEvent *event)
{
	QwtPlotCanvas::mouseMoveEvent(event);

	QwtPlot* plot = dynamic_cast<QwtPlot*> (parentWidget());

	double x = plot->invTransform(QwtPlot::xBottom, event->x());
	double y = plot->invTransform(QwtPlot::yLeft, event->y());

	emit positionChangedForStatusBar(QPointF(x, y));
}
