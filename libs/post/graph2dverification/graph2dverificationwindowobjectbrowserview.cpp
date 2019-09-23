#include "graph2dverificationwindowobjectbrowserview.h"

Graph2dVerificationWindowObjectBrowserView::Graph2dVerificationWindowObjectBrowserView(QWidget* parent)
	: ObjectBrowserView(parent)
{
	// set no edit trigger is available.
	QAbstractItemView::EditTriggers triggers(QAbstractItemView::NoEditTriggers);
	setEditTriggers(triggers);
}
