#include "post2dobjectbrowserview.h"

Post2dObjectBrowserView::Post2dObjectBrowserView(QWidget* parent)
	: ObjectBrowserView(parent)
{
	// set no edit trigger is available.
	QAbstractItemView::EditTriggers triggers(QAbstractItemView::NoEditTriggers);
	setEditTriggers(triggers);
}
