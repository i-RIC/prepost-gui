#ifndef QWTCANVASWITHPOSITIONSIGNAL_H
#define QWTCANVASWITHPOSITIONSIGNAL_H

#include <qwt_plot_canvas.h>

class QwtPlot;

class QwtCanvasWithPositionSignal : public QwtPlotCanvas
{
	Q_OBJECT

public:
	QwtCanvasWithPositionSignal(QwtPlot* plot);

signals:
	void positionChangedForStatusBar(const QPointF& position);

private:
	void mouseMoveEvent(QMouseEvent* event) override;
};

#endif // QWTCANVASWITHPOSITIONSIGNAL_H
