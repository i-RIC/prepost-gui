#ifndef GRAPH2DWINDOWVIEW_H
#define GRAPH2DWINDOWVIEW_H

#include "graph2d_global.h"
#include <qwt_plot.h>

class GRAPH2D_EXPORT Graph2dWindowView : public QwtPlot
{

public:
	/// Constructor
	Graph2dWindowView(QWidget* parent);
	virtual ~Graph2dWindowView();
	void setLegendVisible(bool visible);
};

#endif // GRAPH2DWINDOWVIEW_H
