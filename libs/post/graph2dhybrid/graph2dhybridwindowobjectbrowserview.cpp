#include "graph2dhybridwindowobjectbrowserview.h"

Graph2dHybridWindowObjectBrowserView::Graph2dHybridWindowObjectBrowserView(QWidget* parent)
	: ObjectBrowserView(parent)
{
	// set no edit trigger is available.
	QAbstractItemView::EditTriggers triggers(QAbstractItemView::NoEditTriggers);
	setEditTriggers(triggers);
}
