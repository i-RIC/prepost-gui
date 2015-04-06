#ifndef GRAPH2DHYBRIDWINDOWVIEW_H
#define GRAPH2DHYBRIDWINDOWVIEW_H

#include "graph2dhybrid_global.h"
#include <post/graph2d/graph2dwindowview.h>

class GRAPH2DHYBRID_EXPORT Graph2dHybridWindowView : public Graph2dWindowView
{
public:
	Graph2dHybridWindowView(QWidget* parent);
	virtual ~Graph2dHybridWindowView();
};

#endif // GRAPH2DHYBRIDWINDOWVIEW_H
