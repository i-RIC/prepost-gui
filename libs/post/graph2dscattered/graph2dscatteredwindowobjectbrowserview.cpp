#include "graph2dscatteredwindowobjectbrowserview.h"

Graph2dScatteredWindowObjectBrowserView::Graph2dScatteredWindowObjectBrowserView(QWidget* parent)
	: ObjectBrowserView(parent)
{
	// set no edit trigger is available.
	QAbstractItemView::EditTriggers triggers(QAbstractItemView::NoEditTriggers);
	setEditTriggers(triggers);
}
