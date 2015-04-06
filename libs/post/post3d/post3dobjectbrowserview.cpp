#include "post3dobjectbrowserview.h"

Post3dObjectBrowserView::Post3dObjectBrowserView(QWidget* parent)
	: ObjectBrowserView(parent)
{
	// set no edit trigger is available.
	QAbstractItemView::EditTriggers triggers(QAbstractItemView::NoEditTriggers);
	setEditTriggers(triggers);
}
