#include "graph2dwindowview.h"
#include <qwt_legend.h>

Graph2dWindowView::Graph2dWindowView(QWidget* parent)
	: QwtPlot(parent)
{
	setCanvasBackground(Qt::white);
	setLegendVisible(true);
}

Graph2dWindowView::~Graph2dWindowView()
{

}

void Graph2dWindowView::setLegendVisible(bool visible)
{
	if (visible) {
		QwtLegend* legend = new QwtLegend();
		insertLegend(legend, QwtPlot::RightLegend);
	} else {
		insertLegend(NULL);
	}
	updateLayout();
}
