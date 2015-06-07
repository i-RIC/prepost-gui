#include "post2dbirdeyeobjectbrowserview.h"

Post2dBirdEyeObjectBrowserView::Post2dBirdEyeObjectBrowserView(QWidget* parent)
	: ObjectBrowserView(parent)
{
	// set no edit trigger is available.
	QAbstractItemView::EditTriggers triggers(QAbstractItemView::NoEditTriggers);
	setEditTriggers(triggers);
}
